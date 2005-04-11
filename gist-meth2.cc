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
	unsigned sc;
	gist r;
	r.typ = GT_SSTR;
	r.scnt = 0;

	switch (typ)
	{
	default:
		throw typeError("substr() expects a string");

	case GT_SSTR:
		if (s < 0)
			s = 0;
		sc = scnt;
		if (s < (int)sc && c > 0)
		{
			sc -= s;
			if (sc > c)
				sc = c;
			memcpy(&r.sstr[0], &sstr[s], sc);
			r.scnt = sc;
		}
		break;

	case GT_MSTR:
		if (s < 0)
			s = 0;
		sc = str.cnt;
		if (s < (int)sc && c > 0)
		{
			r.str.dat = str.dat + s;
			sc -= s;
			if (sc > c)
				sc = c;
			r.str.cnt = sc;
		}
		break;

	case GT_LSTR:
		if (s < -(int)str.skp)
			s = -(int)str.skp;
		if (s < (int)str.cnt && c > 0)
		{
			r.typ = GT_LSTR;
			r.unique = false;
			((gist *)this)->unique = false;
			r.str.idx = str.idx;

			r.str.skp = str.skp + s;
			r.str.cnt = str.cnt - s;
			if (r.str.cnt > c)
				r.str.cnt = c;
		}
		break;
	}

	return r;
}


void
gist::strtrim(int s, unsigned c)
{
	unsigned sc;

	switch (typ)
	{
	default:
		throw typeError("strtrim() expects a string");

	case GT_SSTR:
		if (s < 0)
			s = 0;
		sc = scnt;
		if (s < (int)sc && c > 0)
		{
			sc -= s;
			if (sc > c)
				sc = c;
			memcpy(&sstr[0], &sstr[s], sc);
			scnt = sc;
		}
		else
			scnt = 0;
		break;

	case GT_MSTR:
		if (s < 0)
			s = 0;
		sc = str.cnt;
		if (s < (int)sc && c > 0)
		{
			str.dat += s;
			sc -= s;
			if (sc > c)
				sc = c;
			str.cnt = sc;
		}
		else
			str.cnt = 0;
		break;

	case GT_LSTR:
		if (s < -(int)str.skp)
			s = -(int)str.skp;
		if (s < (int)str.cnt && c > 0)
		{
			str.skp += s;
			str.cnt -= s;
			if (str.cnt > c)
				str.cnt = c;
		}
		else
		{
			str.skp = 0;
			str.cnt = 0;
		}
		break;
	}
}

/**********************************************************************/

int
atoi(const gist & g, int base)
{
	return g._toInt(true, base);
}


gist
strlower(const gist & g)
{
	if (!g.isStr())
		throw gist::typeError("strlower expects a string");

	gist r;
	char * rp = r.strbuf(g._strlen());

	unsigned gl = 0;
	int gi = 0;
	const char * gp = 0;

	for (;;)
	{
		if (gl == 0)
			gl = g._strpiece(gi, gp);
		if (gl == 0)
			break;

		while (gl > 0)
		{
			gl--;
			int c = *gp++;
			if (c >= 'A' && c <= 'Z')
				*rp++ = c + ('a' - 'A');
			else
				*rp++ = c;
		}
	}

	return r;
}


gist
strupper(const gist & g)
{
	if (!g.isStr())
		throw gist::typeError("strupper expects a string");

	gist r;
	char * rp = r.strbuf(g._strlen());

	unsigned gl = 0;
	int gi = 0;
	const char * gp = 0;

	for (;;)
	{
		if (gl == 0)
			gl = g._strpiece(gi, gp);
		if (gl == 0)
			break;

		while (gl > 0)
		{
			gl--;
			int c = *gp++;
			if (c >= 'a' && c <= 'z')
				*rp++ = c + ('A' - 'a');
			else
				*rp++ = c;
		}
	}

	return r;
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

#warning "these could improve"


int
gist::strncmp(const char * r, int z) const
{
	const char * ll = CCS();
	if (z < 0)
		z = ::strlen(r);
	return ::strncmp(ll, r, (unsigned)z);
}


int
gist::strncmp(const gist & r, int z) const
{
	const char * ll = CCS();
	const char * rr = r.CCS();
	if (z < 0)
		z = r.len();
	return ::strncmp(ll, rr, (unsigned)z);
}


int
strncmp(const char * l, const gist & r, int z)
{
	const char * rr = r.CCS();
	if (z < 0)
		z = r.len();
	return ::strncmp(l, rr, (unsigned)z);
}

/**********************************************************************/

int
strncasecmp(const gist & l, const char * rp, int rl)
{
	if (!l.isStr())
		throw gist::typeError("strncasecmp expects a string");

	if (rl < 0)
		rl = ::strlen(rp);

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
		r._strsplit(str, sep, ::strlen(sep));
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
		unsigned l;
		char * sp = sep._strflatten(false, false, &l);
		r._strsplit(str, sp, l);
	}
	return r;
}

/**********************************************************************/

bool
strtrue(const gist & g)
{
	if (!g.isStr())
		return (bool)g;

	if (strncasecmp(g, "t") == 0 ||
	    strncasecmp(g, "y") == 0 ||
	    (isdigit(g) && (int)g != 0))
		return true;
	else
		return false;
}

/**********************************************************************/

void
strfill(gist & g, unsigned size, const char * pattern)
{
	char * cp = g.strbuf(size);

	if (size > 0 && pattern)
	{
		char * ep = &cp[size];
		const char * pp = pattern;
		while (cp < ep)
		{
			if (!*pp)
				pp = pattern;
			*cp++ = *pp++;
		}
	}
}


void
strfill(gist & g, unsigned size, const gist & pattern)
{
	strfill(g, size, pattern.CCS());
}

/**********************************************************************/

gist
strstrip(const gist & g)
{
	if (!g.isStr())
		throw gist::typeError("strstrip expects a string");

	unsigned gl = 0;
	int gi = 0;
	const char * gp;
	unsigned front = 0;

	for (;;)
	{
		if (gl == 0)
			gl = g._strpiece(gi, gp);
		if (gl == 0)
			break;

		while (gl > 0)
		{
			gl--;
			int c = *gp++;
			if (c != ' ' && c != '\t' && c != '\n')
				goto brk1;
			front++;
		}
	}
  brk1:
	if (front >= g.len())
	{
		gist r("");
		return r;
	}

	gl = 0;
	gi = g.len() - 1;
	unsigned back = 0;

	while (gi >= 0)
	{
		if (gl == 0)
			gl = g._strpiece(gi, gp);
		if (gl == 0)
			break;
		gi -= gl+1;

		while (gl > 0)
		{
			gl--;
			int c = gp[gl];
			if (c != ' ' && c != '\t' && c != '\n')
				goto brk2;
			back++;
		}
	}
  brk2:

	if (front > 0 || back > 0)
	{
		gist r = g;
		r.strtrim(front, r.len() - front - back);
		return r;
	}
	else
		return g;
}
