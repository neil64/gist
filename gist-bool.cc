/*
 *	Gist -- boolean.
 *
 *	Contains:	(bool)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist::operator bool() const
{
	switch (typ)
	{
	case GT_NIL:
		return false;

	case GT_ARRAY:
	case GT_TABLE:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		return false;

	case GT_STR:
		return cnt > 0;

	case GT_INT:
		return val != 0;

	case GT_FLOAT:
		return dval != 0.0;
	}
}
