/*
 *	Gist -- IEEE-748 floating point.
 *
 *	Contains:	cons(double) (double)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist::operator double() const
{
	giBase * gi = (giBase *)ptr;

	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		return 0.0;

	case GT_STR:
		{
			if (cnt == 0)
				return 0.0;
			giStr * s = (giStr *)gi;

			// XXX
			// try to parse the string as a number.
			// if we can't return 1 anyhow (boolean conversion)
		}
		return 1;

	case GT_INT:
		return (double)val;

	case GT_FLOAT:
		return dval;
	}
}


double
gist::toFloat() const
{
	giBase * gi = (giBase *)ptr;

	switch (gi->type)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		throw valueError("toFloat");

	case GT_STR:
		// XXX
		// try to parse the string as a number.
		// if we can't return 1 anyhow (boolean conversion)
		return 1;

	case GT_INT:
		return (double)val;

	case GT_FLOAT:
		return dval;
	}
}
