/*
 *	Gist -- Methods part 1.
 *
 *	Contains:	len
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/


unsigned
gist::len() const
{
	if (typ == GT_ARRAY)
		return arr->len;
	else if (isStr())
		return strlen();
	else
		throw typeError("len() of unsized object");
}


unsigned
len(const gist & g)
{
	if (g.typ == gist::GT_ARRAY)
		return g.arr->len;
	else if (g.isStr())
		return g.strlen();
	else
		throw gist::typeError("len() of unsized object");
}
