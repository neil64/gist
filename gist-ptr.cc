/*
 *	Gist -- pointer.
 *
 *	Contains:	ptrget()
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

void *
gist::ptrget() const
{
	if (typ != GT_PTR)
		throw ptrError("ptrget() expects a pointer object");
	return ptr;
}
