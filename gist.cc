/*
 *	Gist -- Main / general.
 */

#include	<exception>
#include	<new>
#include	<gc.h>

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/
/*
 *	Memory allocation.
 */


static int	gcInitialized = 0;

/*
 *	Initialize the garbage collector if needed.
 */
static inline void
gcInit()
{
	if (!gcInitialized)
	{
		extern int GC_is_initialized;
		if (!GC_is_initialized)
		{
			GC_all_interior_pointers = 0;
			GC_init();
		}

		gcInitialized = 1;
	}
}


void *
gist::operator new(unsigned sz)
{
	gcInit();

	/*
	 *	Allocate it through the Boehm garbage collector.
	 *	The allocation size here should almost always be 8 (or
	 *	is it 12?), the size of a gist.	 The only time it will be
	 *	different is if someone derives from gist.
	 */
	void * ptr = GC_malloc(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
gistInternal::operator new(unsigned sz)
{
	gcInit();

	void * ptr = GC_malloc(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
gistInternal::alloc(unsigned sz)
{
	void * ptr = GC_malloc(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void
gistInternal::free(void * p)
{
	GC_free(p);
}
