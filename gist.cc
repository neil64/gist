/*
 *	Gist -- Main.
 */

#include	<exception>
#include	<new>
#include	<gc.h>

#include	"gist.h"
#include	"gist-internal.h"

/**********************************************************************/

gist::gistInternal	gist::Nil = { GT_NIL };
gist::gistInternal	gist::Int = { GT_INT };

/**********************************************************************/

/*
 *	Memory allocation.
 */

void *
gist::operator new(unsigned sz)
{
	/*
	 *	Allocate it through the Boehm garbage collector.  The
	 *	allocation size here should almost always be 8, the size
	 *	of a gist.  The only time it will be different is if
	 *	someone derives from gist.
	 */
	void * ptr = GC_malloc(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
giBase::operator new(unsigned sz)
{
	void * ptr = GC_malloc(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}
