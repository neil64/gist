/*
 *	Gist -- Storage handler for strings and arrays.
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

giStore *
giStore::alloc(unsigned sz)
{
	unsigned sz1 = sz + sizeof (giStore);

	giStore * st = (giStore *)gistInternal::alloc(sz1);

	st->data = (void *)(st + 1);
	st->size = sz;
	st->flags = 0;

	return st;
}


/*
 *	Make a key from an `int' that the skip list code can use
 *	(must be big-endian, and unsigned).
 */
void
giStore::mkKey(char * key, int idx)
{
	unsigned i = idx + ((~0U >> 1) + 1);

	key[3] = i & 0xff;
	i >>= 8;
	key[2] = i & 0xff;
	i >>= 8;
	key[1] = i & 0xff;
	i >>= 8;
	key[0] = i & 0xff;
}


int
giStore::gtKey(const char * key)
{
	unsigned i;

	i = key[0];
	i <<= 8;
	i |= key[1];
	i <<= 8;
	i |= key[2];
	i <<= 8;
	i |= key[3];

	return (int)(i - ((~0U >> 1) + 1));
}


#if 0

struct str
{
	-  multi-ref flag, set and never reset.
	-  buffer
	-  buffer size
};


set string:

	-  allocate a str
	-  set the multi-ref flag
	-  set the reference to the passed string.




string buffer management
------------------------

allocate a string:

	-  round up the amount requested
	-  allocate space for the buffer
	-  allocate a str
	-  set values

concatenate (append) two strings (a += operation):

	-  if the left is not multi-ref
		-  if there is space to append the right onto the left,
			copy the data
	-  if the left is not already a multi-str,
		-  allocate a multi-str
		-  insert the left onto the string with zero offset
	-  insert the right onto the multi-str, with correct offset.


prepend (right is prepended onto the left):

	-  similar to append


index a value in the string:

	-  find the str block
	-  calculate the pointer
	-  return the pointer and the number of bytes remaining from there.


#endif // 0
