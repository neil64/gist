/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	set(char *), cons(char *), (char *)
 */

#include	<string.h>
#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist
giStr::concat(const gist & a, const gist & b)
{
	throw gist::notYetError("giStr::concat");
}


char *
giStr::cast(gist * g, int multi)
{
#if 0
	gist x;
	gist * gp;

	if (g->isStr())
		gp = g;
	else
	{
		x = g->toString();
		gp = &x;
	}

	giStr * sp = (giStr *)gp->ptr;
#warning "wrong here"

	if (sp->index)
	{
		x = sp->flatten();
		gp = &x;
	}

#endif



	// if this is not a string,
		// convert with toString
	// if this string is a multi string (index)
		// flatten (flatten should always add a '\0'
	// get the string pointer and return it.

	return 0;
}

/**********************************************************************/

gist &
gist::set(const char * s, int l)
{
	giStore * st = giStore::alloc(0);
	st->data = (char *)s;		// Loose const, but multiRef is set.
	st->multiRef = 1;
	if (l < 0)
	{
		st->hasNull = 1;
		l = strlen(s);
	}
	st->size = l;

	giStr * sp = new giStr;
	sp->str = st;
	sp->index = 0;
	sp->skip = 0;

	ptr = sp;
	cnt = l;

	return *this;
}


gist::gist(const char * s, int l)
{
	set(s, l);
}


gist::operator const char *() const
{
	// char * s = giStr::cast(this, 0);
	return 0;
}


gist::operator char *() const
{
	// call (const char *)
	// set the nultiRef flag (the gist is now a flat string).

	return 0;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

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
