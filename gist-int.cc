/*
 *	Gist -- 32-bit integers.
 *
 *	Contains:	(int)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist::operator long() const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		return 0;

	case GT_STR:
		{
			try
			{
				_strflatten();
				return ((giStr *)intern)->toInt(true, 10);
			}
			catch (valueError)	{}
			catch (overflowError)	{}

			return cnt > 0;
		}

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (long)dval;
	}
}


gist::operator int() const
{
	return (int)(long)*this;
}


gist::operator unsigned long() const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		return 0;

	case GT_STR:
		{
			try
			{
				_strflatten();
				return ((giStr *)intern)->toInt(false, 10);
			}
			catch (valueError)	{}
			catch (overflowError)	{}

			return cnt > 0;
		}

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (unsigned long)dval;
	}
}


gist::operator unsigned() const
{
	return (unsigned)(unsigned long)*this;
}


long
gist::toInt(unsigned base) const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		throw valueError("toInt");

	case GT_STR:
		_strflatten();
		return ((giStr *)intern)->toInt(true, base);

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (long)dval;
	}
}
