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
	if (l.isFloat() || r.isFloat())
		return l.toFloat() == r.toFloat();
	else if (l.isInt() || r.isInt())
		return l.toInt() == r.toInt();
	else if (l.isStr() && r.isStr())
		return l.strcmp(r) == 0;
	else
		throw gist::typeError("operator ==");
}


bool
operator ==(const gist & l, int r)
{
	if (l.isFloat())
		return l.toFloat() == (double)r;
	else if (l.isInt())
		return l.val == r;
	else
		throw gist::typeError("operator -");
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
