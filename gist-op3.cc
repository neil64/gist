/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	==  !=  <  >  <=  >=
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

bool
operator ==(const gist & l, const gist & r)
{
	gist x;
	const gist * lp;
	const gist * rp;

	gist::_coerce1(l, lp, r, rp, x, "==");

	if (lp->isInt())
		return lp->val == rp->val;
	else if (lp->isFloat())
		return lp->dval == rp->dval;
	else // if (lp->isStr())
		return lp->strcmp(*rp) == 0;
}


bool
operator ==(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) == 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator ==(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) == 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator !=(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) != 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator !=(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) != 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) < 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) > 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) > 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) < 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <=(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) <= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <=(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) >= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >=(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) >= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >=(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) <= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}
