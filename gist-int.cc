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
		return 0;

	case GT_STR:
		{
			long n;

			try
			{
				n = ((giStr *)gi)->toInt(1);
			}
			catch (valueError)
			{
				n = 1;
			}

			return n;
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
		return 0;

	case GT_STR:
		{
			unsigned long n;

			try
			{
				n = ((giStr *)gi)->toInt(0);
			}
			catch (valueError)
			{
				n = 1;
			}

			return n;
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
gist::toInt() const
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
		throw valueError("toInt");

	case GT_STR:
		return ((giStr *)gi)->toInt(1);

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (long)dval;
	}
}
