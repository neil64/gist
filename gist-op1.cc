/*
 *	Gist -- Operators part 1.
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist::operator int() const
{
	giStr * s;

	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		return 0;

	case GT_STR:
		if (cnt == 0)
			return 0;
		s = (giStr *)ptr;
		goto str;

	case GT_STR32:
		{
			giStr32 * s32 = (giStr32 *)ptr;
			if (s32->cnt == 0)
				return 0;
			s = s32->str;
		}

	str:
		// try to parse the string as a number.
		// if we can't return 1 anyhow (boolean conversion)
		return 1;

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (int)((giFloat *)ptr)->val;
	}
}


gist
operator +(const gist & a, const gist & b)
{
	if (a.type() == gist::GT_INT)
	{
		if (b.type() == gist::GT_INT)
		{
			return gist(a.val + b.val);
		}
	}

	throw gist::typeError("operator +");
}


gist &
gist::operator +=(const gist & b)
{
	if (type() == gist::GT_INT)
	{
		if (b.type() == gist::GT_INT)
		{
			val += b.val;
			return *this;
		}
	}

	throw gist::typeError("operator +=");
}
