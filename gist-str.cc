/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	set(char *), cons(char *), (char *)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist &
gist::set(const char * s, int l)
{
	return *this;
}


gist::gist(const char * s, int l)
{
	set(s, l);
}


gist::operator const char *() const
{
	return 0;
}


gist::operator char *() const
{
	return 0;
}
