/*
 *	Gist -- Methods part 2.
 *
 *	Contains:	substr, strtrim, atoi, tolower, toupper
 *		(all non-core string methods)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/


gist
gist::substr(int s, unsigned c) const
{
	if (!isStr())
		throw typeError("substr() expects a string");

	gist r;

	if (s < -(int)skip)
		s = -(int)skip;
	if (s < (int)cnt && c > 0)
	{
		r.typ = GT_STR;
		r.unique = false;
		((gist *)this)->unique = false;
		r.intern = intern;

		r.skip = skip + s;
		r.cnt = cnt - s;
		if (r.cnt > c)
			r.cnt = c;
	}

	return r;
}


void
gist::strtrim(int s, unsigned c)
{
	if (!isStr())
		throw typeError("substr() expects a string");

	if (s < -(int)skip)
		s = -(int)skip;
	if (s < (int)cnt && c > 0)
	{
		skip += s;
		cnt -= s;
		if (cnt > c)
			cnt = c;
	}
	else
		_strzero();
}

/**********************************************************************/

int
atoi(const gist & g, int base)
{
	g._strflatten();
	return ((giStr *)g.intern)->toInt(true, base);
}


gist
strlower(const gist & g)
{
	if (!g.isStr())
		throw gist::typeError("strlower expects a string");

	// look at gist-str.cc:copy() for space allocation

	throw gist::notYetError("strlower");
}


gist
strupper(const gist & g)
{
	if (!g.isStr())
		throw gist::typeError("strupper expects a string");

	// look at gist-str.cc:copy() for space allocation

	throw gist::notYetError("strupper");
}

/**********************************************************************/

void
gist::_strsplit(const gist & str, const char * sep, int seplen)
{
	throw gist::notYetError("strsplit");
}


gist
strsplit(const gist & str, const char * sep)
{
	gist r;
	r._strsplit(str, sep, strlen(sep));
	return r;
}


gist
strsplit(const gist & str, const gist & sep)
{
	gist r;
	sep._strflatten();
	r._strsplit(str, sep.CCS(), sep.strlen());
	return r;
}
