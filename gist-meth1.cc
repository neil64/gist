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
		return ((giArray *)ptr)->len;
	else if (typ == GT_STR)
		return strlen();
	else
		throw typeError("len() of unsized object");
}


unsigned
len(const gist & g)
{
	if (g.typ == gist::GT_ARRAY)
		return ((giArray *)g.ptr)->len;
	else if (g.typ == gist::GT_STR)
		return g.strlen();
	else
		throw gist::typeError("len() of unsized object");
}
