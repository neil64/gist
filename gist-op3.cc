/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	==  !=  <  >  <=  >=
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

int
operator ==(const gist & l, const gist & r)
{
	throw gist::notYetError("==(gist, gist)");
}


int
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


int
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


int
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


int
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


int
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


int
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


int
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


int
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


int
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


int
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


int
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


int
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
