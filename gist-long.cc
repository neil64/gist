/*
 *	Gist -- Arbitrary length integers.
 *
 *	Contains:	cons(long long) (long long) (unsigned long long)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist::gist(long long l)
{
	throw notYetError("long long constructor");
}


gist::gist(unsigned long long l)
{
	throw notYetError("unsigned long long constructor");
}


gist &
gist::operator =(long long)
{
	throw notYetError("long long assignment");
}


gist &
gist::operator =(unsigned long long)
{
	throw notYetError("unsigned long long assignment");
}


void
gist::set(long long)
{
	throw notYetError("long long set");
}


void
gist::set(unsigned long long)
{
	throw notYetError("unsigned long long set");
}


gist::operator long long() const
{
	throw notYetError("long long cast");
}


gist::operator unsigned long long() const
{
	throw notYetError("unsigned long long cast");
}
