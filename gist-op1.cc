/*
 *	Gist -- Operators part 1.
 *
 *	Contains:	+ +=
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist
operator +(const gist & a, const gist & b)
{
	if (a.isInt())
	{
		if (b.isInt())
		{
			return gist(a.val + b.val);
		}
	}

	throw gist::typeError("operator +");
}


gist &
gist::operator +=(const gist & b)
{
	if (isInt())
	{
		if (b.isInt())
		{
			val += b.val;
			return *this;
		}
	}

	throw gist::typeError("operator +=");
}


gist &
gist::operator +=(const char *)
{
	return *this;
}
