/*
 *	Gist -- OS dependent stuff.
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/
/*
 *	Read only data memory area test.  If this returns true, a C string
 *	can be used directly, rather than having to copy.  On OS's that
 *	we can't determine this, we always return false, causing strings
 *	to always be copied.
 */

static void *	roStart = 0;
static void *	roEnd = 0;


/*
 *	Initialise the read only data test.
 */
void
gistInternal::readOnlyDataTestInit()
{

#if defined(LINUX) || defined(linux)

#pragma weak __data_start
	extern int __data_start;
#pragma weak data_start
	extern int data_start;
#pragma weak _fp_hw
	extern int _fp_hw;

	/*
	 *	Some linux distributions define __data_start.  Some define
	 *	data_start as a weak symbol.  The latter is technically
	 *	broken, since the user program may define data_start, in
	 *	which case we lose.  Nonetheless, we try both, preferring
	 *	__data_start.  We assume GCC style pragmas.  Linux systems
	 *	also define `_fp_hw' in crtbegin.S (for compatibility with
	 *	the SYSV ABI), so if it exists it can be used as the start
	 *	of the read only data area.
	 */
	void * loc = (void *)&__data_start;
	if (!loc)
		loc = (void *)&data_start;
	if (loc)
	{
		roEnd = loc;

		loc = (void *)&_fp_hw;
		if (loc)
			roStart = loc;
	}

#endif // linux


#if 0
#if defined(__GNUC__)
#warning "using GCC version"

	/*
	 *	Gist always uses the Boehm GC.  The GC detects the start
	 *	of the writable data area so that it knows where to search
	 *	for pointers during collection.  We assume that anything
	 *	before this address is read only.  So, if the symbol
	 *	`GC_data_start' is defined, it should contain the address
	 *	of the start of the writable data area.
	 */

#pragma weak GC_data_start
	extern char * GC_data_start;

	static int blah;

	/*
	 *	See if the variable exists.  The above pragma ensures that
	 *	the linker will always accept the symbol, but if nothing
	 *	defines it for real, it will end up at address 0.
	 */
	if (&GC_data_start > 0)
	{
		void * ds = (void *)GC_data_start;

		/*
		 *	See if the variable contains a reasonable value.
		 *	For a sanity check, we make sure that the address is
		 *	between the address of this function and the variable
		 *	`blah' above.
		 */
		if (ds < (void *)&readOnlyDataTestInit)
			return;
		if (ds > (void *)&blah)
			return;

		roStart = 0;
		roEnd = ds;
	}


#endif // __GNUC
#endif // 0
}


bool
gistInternal::isReadOnlyData(const void * ptr)
{
	if (ptr >= roStart && ptr < roEnd)
		return true;
	else
		return false;
}
