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

/******************************/

#if 0

/*
 *	Coerce the types of the operands into suitable types for one of
 *	the operators +, -, *, /, %, ==, !=, <, >, <=, >=.
 */
void
gist::_coerce1(const gist & l, const gist *& lp,
		const gist & r, const gist *& rp,
		gist & x, const char * op)
{
	if (l.isFloat())
	{
		lp = &l;
		if (r.isFloat())
		{
			rp = &r;
			return;
		}
		else if (r.isInt() || r.isStr())
		{
			x = r.toFloat();
			rp = &x;
			return;
		}
	}
	else if (r.isFloat())
	{
		rp = &r;
		if (l.isInt() || l.isStr())
		{
			x = l.toFloat();
			lp = &x;
			return;
		}
	}
	else if (l.isInt())
	{
		lp = &l;
		if (r.isInt())
		{
			rp = &r;
			return;
		}
		else if (r.isStr())
		{
			x = r.toInt();
			rp = &x;
			return;
		}
	}
	else if (l.isStr())
	{
		if (r.isInt())
		{
			rp = &r;
			x = l.toInt();
			lp = &x;
			return;
		}
		if (r.isStr())
		{
			lp = &l;
			rp = &r;
			return;
		}
	}

	throw typeError(op);
}

#endif
