/*
 *	Gist -- IEEE-748 floating point.
 *
 *	Contains:	cons(double) (double)
 */

#include	<stdlib.h>
#include	<errno.h>

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

double
gist::toFloat() const
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
		throw valueError("toFloat");

	case GT_SSTR:
	case GT_MSTR:
	case GT_LSTR:
		{
			char * sp = _strflatten(false, true, 0);
			char * ep;
			int err = errno;
			errno = 0;

			double n = strtod(sp, &ep);

			int e = errno;
			errno = err;

			if (e == ERANGE)
				throw gist::overflowError("toFloat");

			if (ep == sp)
				goto value;
			while (*ep == ' ' || *ep == '\t' || *ep == '\n')
				ep++;
			if (*ep != '\0')
				goto value;

			return n;

  value:
			throw gist::valueError("toFloat");
		}


	case GT_INT:
		return (double)val;

	case GT_FLOAT:
		return dval;
	}
}
