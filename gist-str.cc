/*
 *	Gist -- Strings.
 *
 *	Contains:	set(char *), cons(char *), (char *)
 */

#include	<stdio.h>
#include	<string.h>
#include	<memory.h>
#include	<stdlib.h>
#include	<errno.h>
#include	"gist.h"
#include	"gist-internal.h"


/*
 *	The maximum number of characters that we will copy when concatenating
 *	strings.  If greater than this, we create a "multi" string.
 */
static const unsigned	maxCopy = 32;

/*
 *	String space is allocated in multiples of this value.
 */
static const unsigned	strChunk = 64;

/**********************************************************************/
/*
 *	Internal gist member functions.
 */


long
giStr::toInt(bool sign, unsigned base)
{
	const char * cp = data;
	unsigned long n = 0;
	bool neg = false;
	bool seen = false;
	int c;

	if (base == 1 || base > 36)
		throw gist::valueError("bad base in toInt");

	while (*cp == ' ' || *cp == '\t')
		cp++;

	if (sign && *cp == '-')
	{
		neg = true;
		cp++;
	}
	else if (base == 0 && *cp == '0')
	{
		c = cp[1];
		if (c == 'x' || c == 'X')
		{
			base = 16;
			cp += 2;
		}
		else
		{
			base = 8;
			cp++;
			seen = true;
		}
	}

	while ((c = *cp++))
	{
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			break;

		if (c >= (signed)base)
			goto value;

		seen = true;

		unsigned long n1 = n * base;
		if (n1 < n)
			goto overflow;
		n = n1 + c;
	}

	while (c == ' ' || c == '\t' || c == '\n')
		c = *cp++;
	if (*cp != '\0')
		goto value;

	if (sign)
	{
		if (neg && n == (~0U>>1) + 1)
			return n;
		if (n > (~0U>>1))
			goto overflow;
		if (neg)
			return -(long)n;
		else
			return n;
	}
	else
		return n;

  value:
	throw gist::valueError("bad digit in toInt");
  overflow:
	throw gist::overflowError("toInt");
}


double
giStr::toFloat()
{
	int err = errno;
	errno = 0;
	char * ep;

	double n = strtod(data, &ep);

	int e = errno;
	errno = err;

	if (e == ERANGE)
		throw gist::overflowError("toFloat");

	if (ep == data)
		goto value;
	while (*ep == ' ' || *ep == '\t' || *ep == '\n')
		ep++;
	if (*ep != '\0')
		goto value;

	return n;

  value:
	throw gist::valueError("toFloat");
}


/**********************************************************************/
/**********************************************************************/
/*
 *	Private gist member functions.
 */

/*
 *	Convert a gist object to a null terminated string and return it.
 *	Note that we promise not to change the object (const), but we may
 *	do anyhow, except that the change will not effect the value of the
 *	object (that is, we may flatten the string or move it to add a '\0',
 *	but the value will remain the same).
 */
char *
gist::_strcast(bool rw) const
{
	gist x;
	gist * gp;

	if (isStr())
		gp = (gist *)this;
	else
	{
		x = toString();
		gp = &x;
	}

	giStr * sp = (giStr *)gp->intern;

	if (sp->index || !sp->hasNull || (!unique && rw))
		_strflatten();

	((gist *)this)->unique = false;
	return sp->data;
}


void
gist::_strflatten() const
{
	/*
	 *	We will change this gist, but the resulting value will be
	 *	the same (const in principal).
	 */
	gist * gp = (gist *)this;
	giStr * sp = (giStr *)gp->intern;
	unsigned l = gp->cnt;

	if (!sp->index && gp->unique)
	{
		if (sp->hasNull)
			return;
		if (gp->skip + l < sp->size)
		{
			sp->data[gp->skip + l] = '\0';
			sp->hasNull = true;
			return;
		}
	}

	l++;
	char * cp = (char *)gistInternal::alloc(l);

	(void)strcpy(cp, *gp);

	gp->unique = true;
	sp->index = 0;
	sp->data = cp;
	sp->size = l;
	sp->hasNull = true;
}


void
gist::_strzero()
{
	if (!isStr())		// (should never happen)
		return;

	giStr * sp = (giStr *)intern;

	sp->index = 0;
	sp->data = 0;
	sp->hasNull = false;
	sp->size = 0;

	cnt = 0;
	skip = 0;

	unique = false;		// (doesn't matter, but false makes strcat()
				//  slightly more efficient)
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*
 *	Public gist member functions.
 */

/*
 *	gist::set() will be a very commonly used function, particularly
 *	the first "C string" version.  It is coded fairly carefully.
 *	In particular we could have a single version that took a default
 *	length argument, but that would cause the compiler to provide the
 *	default in application code for every call.  So, there are two
 *	versions of set().
 */
void
gist::set(const char * s)
{
	giStr * sp = new giStr;
	unsigned l;

	if (!s || !*s)
	{
		sp->data = "";
		l = 0;
		goto hop;
	}

	l = ::strlen(s);

	if (gistInternal::isReadOnlyData(s))
	{
		/*
		 *	If the C string is in read only memory, we can
		 *	refer to it directly.  With compilers like GCC,
		 *	strings constants are always in the .rodata
		 *	section, so this should be a big win.  In the
		 *	following assignment, we deliberately cast away
		 *	the const, but since `unique' is set to false
		 *	below, we will never attempt to modify the data.
		 */
		sp->data = (char *)s;
	}
	else
	{
		sp->data = (char *)gistInternal::alloc(l+1);
		memcpy(sp->data, s, l);
		sp->data[l] = '\0';
	}

  hop:
	// sp->index = 0;
	sp->size = l;
	sp->hasNull = true;

	typ = GT_STR;
	unique = false;		// 'cause the caller may use it for other
	intern = sp;		// things, and 'cause it could be in read-
	cnt = l;		// only memory, and other reasons.
	skip = 0;
}

/******************************/

void
gist::set(const char * s, int l)
{
	if (l < 0)
	{
		set(s);
		return;
	}

	giStr * sp = new giStr;

	if (!s || l == 0)
	{
		/*
		 *	If we were not given any data, just be nice.
		 */
		sp->data = "";
		sp->hasNull = true;
		l = 0;
		goto hop;
	}

	if (gistInternal::isReadOnlyData(s))
	{
		/*
		 *	If the C string is in read only memory, we can
		 *	refer to it directly.  With compilers like GCC,
		 *	strings constants are always in the .rodata
		 *	section, so this should be a big win.  In the
		 *	following assignment, we deliberately cast away
		 *	the const, but since `unique' is set to false
		 *	below, we will never attempt to modify the data.
		 */
		sp->data = (char *)s;
	}
	else
	{
		sp->data = (char *)gistInternal::alloc(l+1);
		memcpy(sp->data, s, l);
		sp->data[l] = '\0';
		sp->hasNull = true;
	}

  hop:
	// sp->index = 0;
	sp->size = l;

	typ = GT_STR;
	unique = false;		// 'cause the caller may use it for other
	intern = sp;		// things, and 'cause it could be in read-
	cnt = l;		// only memory, and other reasons.
	skip = 0;
}

/******************************/

void
gist::copy(const char * s, int l)
{
	giStr * sp = new giStr;
	sp->index = 0;
	if (l < 0)
		l = ::strlen(s);
#warning "should allocate minimum of strChunk in size"
		// could also split this function to allocate space
		// for functions like strlower()
	sp->data = (char *)gistInternal::alloc(l+1);
	memcpy(sp->data, s, l);
	sp->data[l] = '\0';
	sp->hasNull = true;

	typ = GT_STR;
	unique = true;
	intern = sp;
	cnt = l;
	skip = 0;
}

/******************************/

gist
gist::toString() const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		throw typeError("toString");

	case GT_STR:
		return *this;

	case GT_INT:
		{
			char a[12];
			char * ap = &a[sizeof a];
			long n = val;
			bool sign = n < 0;
			if (sign)
				n = -n;

			*--ap = '\0';
			if (n == 0)
				*--ap = '0';
			else
				while (n)
				{
					*--ap = '0' + (n % 10);
					n /= 10;
				}

			if (sign)
				*--ap = '-';

			gist r;
			r.copy(ap, &a[sizeof a] - ap);

			return r;
		}

	case GT_FLOAT:
		{
			int x;
			char a[64];

			double d = dval;
			if (d < 0.0)
				d = -d;

			if (d < 1e-32)
				x = snprintf(a, sizeof a, "%.48g", dval);
			else
				x = snprintf(a, sizeof a, "%.48f", dval);

			if (x < 0)
				x = 0;
			else if ((unsigned)x >= sizeof a)
				x = sizeof a - 1;

			gist r;
			r.copy(a, x);

			return r;
		}
	}
}

/******************************/

int
gist::strcmp(const gist & r) const
{
	if (this == &r)
		return 0;

	if (intern == r.intern && skip == r.skip)
	{
		if (cnt == r.cnt)
			return 0;
		else if (cnt < r.cnt)
			return -1;
		else
			return 1;
	}

	unsigned ll = 0, rl = 0;
	int li = 0, ri = 0;
	const char * lp = 0, * rp = 0;

	for (;;)
	{
		if (ll == 0)
			ll = _strpiece(li, lp);
		if (rl == 0)
			rl = r._strpiece(ri, rp);

		if (ll == 0 || rl == 0)
			break;

		unsigned l = ll;
		if (l > rl)
			l = rl;

		int x = memcmp(lp, rp, l);
		if (x)
			return x;

		lp += l;
		rp += l;
		ll -= l;
		rl -= l;
	}

	if (ll == rl)
		return 0;
	else if (ll < rl)
		return -1;
	else
		return 1;
}


int
gist::strcmp(const char * s) const
{
	gist l;
	const gist * lp;
	gist r(s);

	/*
	 *	Grab the string pointer.  If the object is not a string
	 *	try to make it into one.
	 */
	if (isStr())
		lp = (gist *)this;
	else
	{
		l = toString();
		lp = &l;
	}

	return lp->strcmp(r);
}


int
strcmp(const gist & l, const char * r)
{
	gist lx;
	const gist * lp;
	gist rx(r);

	/*
	 *	If the left is not a string try to make it into one.
	 */
	if (l.isStr())
		lp = &l;
	else
	{
		lx = l.toString();
		lp = &lx;
	}

	return lp->strcmp(rx);
}


int
strcmp(const char * l, const gist & r)
{
	gist lx(l);
	gist rx;
	const gist * rp;

	/*
	 *	If the right is not a string try to make it into one.
	 */
	if (r.isStr())
		rp = &r;
	else
	{
		rx = r.toString();
		rp = &rx;
	}

	return lx.strcmp(*rp);
}


int
strcmp(const gist & l, const gist & r)
{
	gist lx;
	const gist * lp;
	gist rx;
	const gist * rp;

	/*
	 *	Try to make both objects into strings.
	 */
	if (l.isStr())
		lp = &l;
	else
	{
		lx = l.toString();
		lp = &lx;
	}
	if (r.isStr())
		rp = &r;
	else
	{
		rx = r.toString();
		rp = &rx;
	}

	return lp->strcmp(*rp);
}

/******************************/

unsigned
gist::_strpiece(int & ix, const char *& pt) const
{
	int i = ix;
	if (i < 0 || (unsigned)i >= cnt)
		return 0;

	giStr * sp = (giStr *)intern;

	/*
	 *	If the string is single, it's easy.
	 */
	if (!sp->index)
	{
		pt = &((const char *)sp->data)[i + skip];
		ix = cnt;
		return cnt - i;
	}

	/*
	 *	The string is multi.  Search for the string chunk that
	 *	contains the required index.  We search for index + 1 because
	 *	giIndexInt::previous() is a "less-than" operation and we want
	 *	"less-or-equal".
	 */
	int i1 = i + skip + sp->index->min;
	intKey * kp = sp->index->previous(i1 + 1);
	if (!kp || kp->key > i1)
		throw gist::internalError("bogus index in gist::strpiece");

	i1 -= kp->key;
	giSChunk * cp = kp->schunk;
	pt = &((const char *)cp->data)[i1];

	i1 = cp->len - i1;
	ix = i + i1;

	return i1;
}


unsigned
gist::strpiece(int & ix, const char *& pt) const
{
	unsigned l = _strpiece(ix, pt);
	if (l > 0)
		((gist *)this)->unique = false;
	return l;
}


unsigned
strpiece(const gist & g, int & ix, const char *& pt)
{
	unsigned l = g._strpiece(ix, pt);
	if (l > 0)
		((gist &)g).unique = false;
	return l;
}

/************************************************************/
/*
 *	String concatenation.
 */

void
giStr::makeMulti(unsigned len)
{
	giSChunk * cp = (giSChunk *)gistInternal::alloc(sizeof (giSChunk));
	cp->data = data;
	cp->data0 = data;
	cp->len = len;

	index = new (giIndexInt::StrLevels) giIndexInt;
	index->insert(0, cp);
	index->min = 0;
	index->max = len;
}


void
gist::strcat(const gist & r)
{
	const gist * rp;

	/*
	 *	Ensure that the left (this) is a string, and convert the
	 *	right to a string if necessary.
	 */
	if (!isStr())
		throw typeError("strcat");
	if (r.isStr())
		rp = &r;
	else
		rp = new gist(r.toString());

	/*
	 *	Check for the degenerate cases.
	 */
	if (rp->cnt == 0)
		return;
	if (cnt == 0)
	{
		*this = *rp;
		return;
	}

	giStr * ls = (giStr *)intern;
	giStr * rs = (giStr *)rp->intern;

	/*
	 *	If the right side length is less than the copy limit,
	 *	try for various optimizations.
	 */
	if (rp->cnt <= maxCopy)
	{
		/*
		 *	NOTE:	There is code that expects strcat to do a
		 *		copy no matter what if the right is below
		 *		the copy limit.  For instance, strcat(int)
		 *		(below) creates a gist string using a local
		 *		variable buffer, so that gist must be
		 *		completely invalid once strcat finishes.
		 */
		do
		{
			if (!unique)
				break;

			/*
			 *	The left is unique, so we can try to copy
			 *	the right directly into the left, if there
			 *	is enough space available.
			 */
			unsigned o;
			if (!ls->index)
				o = skip + cnt;
			else if (!ls->chunk)
				break;
			else
				o = ls->chunk->len;

			unsigned l = rp->cnt;
			if (l > (ls->size - o))
				break;

			/*
			 *	We are allowed to write to the left directly,
			 *	and there is space available.  Go for it.
			 */
			strcpy(&ls->data[o], *rp);
			cnt += l;
			if (!ls->index)
				ls->hasNull = false;
			else
				ls->chunk->len += l;
			return;

		} while (0);

		/*
		 *	There was not enough space at the end of the chunk.
		 *	Try for a total length that might be less than the
		 *	copy limit.
		 */
		if (cnt + rp->cnt <= maxCopy)
		{
			/*
			 *	Create a new chunk and copy both the left
			 *	and right strings to it.
			 */
			giStr * nls = (giStr *)gistInternal::alloc(
							sizeof (giStr));
			nls->data = (char *)gistInternal::alloc(strChunk);
			nls->size = strChunk;
			nls->hasNull = false;
			nls->index = 0;

			strcpy(&nls->data[0], *this);
			strcpy(&nls->data[cnt], *rp);

			unique = true;
			intern = nls;
			skip = 0;
			cnt += rp->cnt;

			return;
		}

		/*
		 *	Simple copies failed, so now allocate a new chunk
		 *	and add it to the end of the current chunk list,
		 *	making the left into a multi if needed.
		 */
		if (!ls->index)
			ls->makeMulti(skip + cnt);

		giSChunk * cp =
			(giSChunk *)gistInternal::alloc(sizeof (giSChunk));
		cp->data = (char *)gistInternal::alloc(strChunk);
		cp->data0 = cp->data;
		cp->len = rp->cnt;
		strcpy(cp->data, *rp);

		ls->index->insert(ls->index->max, cp);
		ls->index->max += rp->cnt;

		ls->data = cp->data;
		// ls->len = rp->cnt;
		ls->chunk = cp;
		ls->size = strChunk;

		cnt += rp->cnt;

		/*
		 *	Setting `unique' here is subtle -- unique on a multi
		 *	string really means that only the last chunk is
		 *	unique, and then only if the chunk uses the `data'
		 *	and `size' references in the giStr.  This allows
		 *	the next concatenation to copy into that last chunk
		 *	if needed.  The index structure itself, and all of
		 *	the other chunks in it are never touched.
		 */
		unique = true;

		return;
	}

	/*
	 *	The right was too big to copy, so we will place references
	 *	to it in our index.  If we (the left) don't have an index,
	 *	make one.  Since there will be cross referencing, neither
	 *	the left nor the right will remain unique.
	 */
	if (!ls->index)
		ls->makeMulti(skip + cnt);

	// ls->data = 0;
	ls->chunk = 0;
	((gist *)this)->unique = false;
	((gist *)rp)->unique = false;

	if (!rs->index)
	{
		/*
		 *	The right is a (big) single.  Create a reference
		 *	(giSChunk) to it and store it in our index.
		 */
		giSChunk * cp =
			(giSChunk *)gistInternal::alloc(sizeof (giSChunk));
		cp->data0 = rs->data;
		cp->data = rs->data + rp->skip;
		cp->len = rp->cnt;

		ls->index->insert(ls->index->max, cp);
		ls->index->max += cp->len;
	}
	else
	{
		/*
		 *	The right is a multi.  Insert all of its chunks
		 *	into our index.
		 */
		int i = ls->index->max;
		intKey * kp;

#warning "broken if left is the same gist as the right"
		for (kp = rs->index->first();
		     kp;
		     kp = rs->index->next(kp->key))
		{
			ls->index->insert(i, kp->chunk);
			i += kp->schunk->len;
		}

		ls->index->max = i;
	}
}


void
gist::strcat(int c)
{
	char a[2] = { c, '\0' };
	gist cx(a);
	strcat(cx);
}


void
gist::strcat(const char * r, int count)
{
	if (count == 0)
		return;
	gist rx(r);
	if (count > 0)
		rx.strtrim(0, count);
	strcat(rx);
}


void
strcat(gist & g, int c)
{
	char a[2] = { c, '\0' };
	gist cx(a);
	g.strcat(cx);
}


void
strcat(gist & g, const char * r, int count)
{
	if (count == 0)
		return;
	gist rx(r);
	if (count > 0)
		rx.strtrim(0, count);
	g.strcat(rx);
}


void
strcat(char * l, const gist & r)
{
	throw gist::notYetError("strcat");
}


void
strncat(char * l, const gist & r, unsigned c)
{
	throw gist::notYetError("strncat");
}

/************************************************************/

unsigned
strcpy(char * dest, const gist & src, unsigned start, unsigned count)
{
	int ix = start;
	unsigned l;
	const char * p;
	unsigned c = 0;

	while (count > 0 && (l = src._strpiece(ix, p)) > 0)
	{
		if (l > count)
			l = count;

		memcpy(dest, p, l);

		dest += l;
		c += l;
		count -= l;
	}

	if (c > 0)
		((gist &)src).unique = false;

	if (count > 0)
	{
		*dest++ = '\0';
		c++;
	}

	return c;
}
