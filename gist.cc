/*
 *	Gist -- Main / general.
 */

#include	<exception>
#include	<new>
#include	<gc.h>

#include	"gist.h"
#include	"gist-internal.h"

/**********************************************************************/

gist::gistInternal	gist::Nil = { GT_NIL };
gist::gistInternal	gist::Int = { GT_INT };
gist::gistInternal	gist::Float = { GT_FLOAT };

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

		gist x(0);

		gcInitialized = x.ty;
		// gcInitialized = 1;
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
giBase::operator new(unsigned sz)
{
	gcInit();

	void * ptr = GC_malloc(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void *
giBase::alloc(unsigned sz)
{
	void * ptr = GC_malloc(sz);
	if (!ptr)
		throw std::bad_alloc();
	return ptr;
}


void
giBase::free(void * p)
{
	GC_free(p);
}

/******************************/

/*
 *	This is mostly to ensure that strings have their `multiRef'
 *	flag reset.
 */
void
gist::gistCopy(const gist & from)
{
	ptr = from.ptr;
	all[0] = from.all[0];
	all[1] = from.all[1];

	if (isStr())
	{
		giStr * st = (giStr *)ptr;
		if (!st->index)
			st->str->multiRef = 1;
	}
}
