/*
 *	Gist -- Main / general.
 */

// #include <stdio.h>
// #define GC_DEBUG

#include	<exception>
#include	<new>
#include	<gc.h>

#include	"gist.h"
#include	"gist-internal.h"


/*
 *	The NIL gist.
 */
gist		gist::nil;

/*
 *	Gist version information.
 */

#include "gist-version.h"
gist		gist::version(VERSION_STRING);

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
			// GC_all_interior_pointers = 0;
			GC_init();
		}

		/*
		 *	Try to find the address bounds of the read only
		 *	data section.  This is used to optimise setting
		 *	of strings to constant values from C strings.
		 */
		gistInternal::readOnlyDataTestInit();

		/*
		 *	Don't try this again.
		 */
		gcInitialized = 1;
	}
}


void *
gist::operator new(size_t sz)
{
	gcInit();

	/*
	 *	Allocate it through the Boehm garbage collector.
	 *	The allocation size here should almost always be 16,
	 *	the size of a gist.  The only time it will be
	 *	different is if someone derives from gist.
	 *
	 *	If the size is 16, we assume that we are only allocating
	 *	a gist, in which case we will reduce the size to 15 bytes,
	 *	so that GC_malloc() won't round the size up to 32 bytes.
	 *	With `GC_all_interior_pointers' enabled (see the GC
	 *	documentation), the allocator wants to put an extra byte
	 *	at the end of everything, so that a pointer to just past
	 *	the end of the object still refers to the object.  The
	 *	allocator will round the size back up to 16.
	 *
	 *	Much of gist assumes that the new memory is cleared.
	 */
	if (sz == 16)
		sz = 15;
	void * ptr = GC_MALLOC(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
gistInternal::operator new(size_t sz)
{
	gcInit();

	/*
	 *	Much of gist assumes that the new memory is cleared.
	 */
	void * ptr = GC_MALLOC(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
gistInternal::alloc(unsigned sz)
{
	/*
	 *	Much of gist assumes that the new memory is cleared.
	 */
	void * ptr = GC_MALLOC(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
gistInternal::allocAtomic(unsigned sz)
{
	/*
	 *	Allocate space for an object containing no pointers, and
	 *	pointers to it will be near its start.  It is not cleared.
	 */
	void * ptr = GC_MALLOC_ATOMIC_IGNORE_OFF_PAGE(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
gistInternal::strAlloc(unsigned sz)
{
	/*
	 *	Like `allocAtomic()' but will round the size up to a
	 *	multiple of `strChunk'.
	 */
	sz += giStr::strChunk - 1;
	sz &= ~(giStr::strChunk - 1);

	return allocAtomic(sz);
}


void
gistInternal::free(void * p)
{
	GC_FREE(p);
}

/**********************************************************************/

/*
 *	The Boehm GC will not scan into non GC controlled heap.  So we
 *	replace the system versions of the standard C++ allocators
 *	(operator new and delete) with ones that call the GC.
 */

void *
operator new(size_t sz)
{
	gcInit();

	void * ptr = GC_MALLOC(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void
operator delete(void * ptr)
{
	// GC_FREE(ptr);
}
