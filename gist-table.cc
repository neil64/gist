/*
 *	Gist -- Tables.
 */

#include	<stdio.h>
#include	<string.h>
#include	<memory.h>
#include	<stdlib.h>
#include	<errno.h>

#include	"gist.h"
#include	"gist-internal.h"


/*
 *	The maximum number of characters that we will copy when concatenating
 *	strings.  If greater than this, we create a "multi" string.
 */
const unsigned	maxCopy = 32;

/*
 *	String space is allocated in multiples of this value.
 */
const unsigned	strChunk = 64;

/**********************************************************************/

gist &
gist::_tableindex(long i)
{
	throw notYetError("array index");
}


gist &
gist::_tableindex(double i)
{
	throw notYetError("array index");
}


gist &
gist::_tableindex(const char * i)
{
	throw notYetError("array index");
}


gist &
gist::_tableindex(const gist & i)
{
	throw notYetError("array index");
}
