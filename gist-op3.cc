/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	==  !=  <  >  <=  >=
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

int
operator ==(const gist & g, const char * s)
{
	try
	{
		return g.cmp(s) == 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator ==(const char * s, const gist & g)
{
	try
	{
		return g.cmp(s) == 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator !=(const gist & g, const char * s)
{
	try
	{
		return g.cmp(s) != 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator !=(const char * s, const gist & g)
{
	try
	{
		return g.cmp(s) != 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator <(const gist & g, const char * s)
{
	try
	{
		return g.cmp(s) < 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator <(const char * s, const gist & g)
{
	try
	{
		return g.cmp(s) > 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator >(const gist & g, const char * s)
{
	try
	{
		return g.cmp(s) > 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator >(const char * s, const gist & g)
{
	try
	{
		return g.cmp(s) < 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator <=(const gist & g, const char * s)
{
	try
	{
		return g.cmp(s) <= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator <=(const char * s, const gist & g)
{
	try
	{
		return g.cmp(s) >= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator >=(const gist & g, const char * s)
{
	try
	{
		return g.cmp(s) >= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


int
operator >=(const char * s, const gist & g)
{
	try
	{
		return g.cmp(s) <= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}
