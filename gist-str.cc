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


void
giStr::flatten()
{
	if (!index && str->hasNull && str->data)
		return;

	throw gist::notYetError("giStr::flatten");

	// always add a '\0'
	// set multiRef and hasNull
	// may need to move a non-index string if there is no '\0'.
}


long
giStr::toInt(int sign)
{
	throw gist::notYetError("giStr::toInt");
}


double
giStr::toFloat()
{
	throw gist::notYetError("giStr::toFloat");
}

/**********************************************************************/

/*
 *	Convert a gist object to a null terminated string and return it.
 *	Note that we promise not to change the object (const), but we may
 *	do anyhow, except that the change will not effect the value of the
 *	object (that is, we may flatten the string or move it to add a '\0',
 *	but the value will remain the same).
 */
char *
gist::strCast(int multi) const
{
	gist x;
	gist * gp;

	if (isStr())
		gp = (gist *)this;
	else
	{
		x = toString();
		gp = &x;
	}

	giStr * sp = (giStr *)gp->ptr;

	sp->flatten();

	return (char *)sp->str->data;
}


gist &
gist::set(const char * s, int l)
{
	giStore * st = giStore::alloc(0);
	st->data = (char *)s;		// Drop const, but multiRef is set.
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

	ptr = sp;
	cnt = l;
	skip = 0;

	return *this;
}


gist::gist(const char * s, int l)
{
	set(s, l);
}


gist
gist::toString() const
{
	throw notYetError("giStr::toString");
}


int
gist::cmp(const char * s) const
{
	gist x;
	gist * gp;

	if (isStr())
		gp = (gist *)this;
	else
	{
		x = toString();
		gp = &x;
	}

	giStr * sp = (giStr *)gp->ptr;

#warning "here"
	// scan through the string doing the comparison
	throw notYetError("cmp");
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
