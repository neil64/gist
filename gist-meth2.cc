/*
 *	Gist -- Methods part 2.
 *
 *	Contains:	substr, strtrim, atoi, tolower, toupper
 *		(all non-core string methods)
 */

#include	<string.h>

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
	{
		skip = 0;
		cnt = 0;
	}
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

bool
isdigit(const gist & l)
{
	if (!l.isStr())
		return false;

	unsigned ll = 0;
	int li = 0;
	const char * lp = 0;

	for (;;)
	{
		if (ll == 0)
			ll = l._strpiece(li, lp);

		if (ll == 0)
			return true;

		while (ll-- > 0)
		{
			int c = *lp++;
			if (c < '0' || c > '9')
				return false;
		}
	}
}

/**********************************************************************/

int
strncasecmp(const gist & l, const char * rp, int rl)
{
	if (!l.isStr())
		throw gist::typeError("strncasecmp expects a string");

	if (rl < 0)
		rl = strlen(rp);

	unsigned ll = 0;
	int li = 0;
	const char * lp = 0;

	for (;;)
	{
		if (ll == 0)
			ll = l._strpiece(li, lp);

		if (ll == 0 || rl == 0)
			break;

		unsigned l = ll;
		if (l > (unsigned)rl)
			l = rl;

		int x = strncasecmp(lp, rp, l);
		if (x)
			return x;

		lp += l;
		rp += l;
		ll -= l;
		rl -= l;
	}

	if (rl == 0)
		return 0;
	else
		return -1;
}

/**********************************************************************/
/*
 *	Split a string.	 This works the same way as the python
 *	string.split().	 Split with no separator is implemented in the first
 *	function, and split with the separator in the second function.
 *	The two are semantically different -- the first looks for words
 *	separated by arbitrary amounts of white space, and the second splits
 *	around single non-overlapping occurrences of the separator string.
 */

void
gist::_strsplit(const gist & str)
{
	/*
	 *	Make the result into an empty array.
	 */
	array(0);

	int si = 0;
	unsigned sl = 0;
	const char * sp;
	unsigned o = 0;
	unsigned l = 0;
	gist st;

	for (;;)
	{
		/*
		 *	Skip white space.
		 */
		for (;;)
		{
			if (sl == 0)
				sl = str._strpiece(si, sp);
			if (sl == 0)
				return;

			int c = *sp;
			if (c != ' ' && c != '\t' && c != '\n' &&
			    c != '\r' && c != '\f')
				break;

			sp++;
			sl--;
			o++;
		}

		/*
		 *	Pass over the word.
		 */
		for (;;)
		{
			if (sl == 0)
				sl = str._strpiece(si, sp);
			if (sl == 0)
				break;

			int c = *sp;
			if (c == ' ' || c == '\t' || c == '\n' ||
			    c == '\r' || c == '\f')
				break;

			sp++;
			sl--;
			l++;
		}

		if (l > 0)
		{
			st = str;
			st.strtrim(o, l);
			push(st);
			o += l;
			l = 0;
		}

		if (sl == 0)
			return;
	}
}

void
gist::_strsplit(const gist & str, const char * sep, int seplen)
{
	/*
	 *	Make the result into an empty array.
	 */
	array(0);

	int si = 0;
	unsigned sl = 0;
	const char * sp;
	unsigned o = 0;
	unsigned l = 0;
	gist st;

	if (seplen != 1)
#warning "multi character separators"
		throw gist::notYetError("strsplit(str, sep) can't handle "
					"more than one character in the "
					"separator");

	int sepp = *sep;

	/*
	 *	Pass over the word looking for the next separator.
	 */
	for (;;)
	{
		if (sl == 0)
			sl = str._strpiece(si, sp);
		if (sl == 0)
		{
			st = str;
			st.strtrim(o, l);
			push(st);
			return;
		}

		if (*sp == sepp)
		{
			st = str;
			st.strtrim(o, l);
			push(st);
			o += l + 1;
			l = 0;
		}
		else
			l++;

		sp++;
		sl--;
	}
}


gist
strsplit(const gist & str, const char * sep)
{
	gist r;
	if (!sep || !*sep)
		r._strsplit(str);
	else
		r._strsplit(str, sep, strlen(sep));
	return r;
}


gist
strsplit(const gist & str, const gist & sep)
{
	gist r;
	if (len(sep) == 0)
		r._strsplit(str);
	else
	{
		sep._strflatten();
		r._strsplit(str, sep.CCS(), sep.strlen());
	}
	return r;
}

/**********************************************************************/

bool
strtrue(const gist & g)
{
	if (g.typ != gist::GT_STR)
		return (bool)g;

	if (strncasecmp(g, "t") == 0 ||
	    strncasecmp(g, "y") == 0 ||
	    (isdigit(g) && (int)g != 0))
		return true;
	else
		return false;
}
