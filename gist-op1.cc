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
	if (a.isFloat() || b.isFloat())
		return gist(a.toFloat() + b.toFloat());
	else if (a.isInt() || b.isInt())
		return gist(a.toInt() + b.toInt());
	else if (a.isStr() && b.isStr())
		return giStr::concat(a, b);
	else
		throw gist::typeError("operator +");
}


gist
operator +(const gist & a, int b)
{
	if (a.isFloat())
		return gist(a.toFloat() + (double)b);
	else if (a.isInt())
		return gist(a.val + b);
	else
		throw gist::typeError("operator +");
}


gist
operator +(const gist & a, unsigned b)
{
	return a + (int)b;
}

/******************************/

gist
operator -(const gist & a, const gist & b)
{
	if (a.isFloat() || b.isFloat())
		return gist(a.toFloat() - b.toFloat());
	else if (a.isInt() || b.isInt())
		return gist(a.toInt() - b.toInt());
	else
		throw gist::typeError("operator -");
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
