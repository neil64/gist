/*
 *	Gist -- IEEE-748 floating point.
 *
 *	Contains:	cons(double) (double)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

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
		return ((giStr *)gi)->toFloat();

	case GT_INT:
		return (double)val;

	case GT_FLOAT:
		return dval;
	}
}
