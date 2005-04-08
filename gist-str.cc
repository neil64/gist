/*
 *	Gist -- Strings.
 *
 *	Contains:	set(char *), cons(char *), (char *)
 */

#include	<stdio.h>
#include	<memory.h>
#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/
/**********************************************************************/
/*
 *	Private gist member functions.
 */

/*
 */
unsigned
gist::_strlen() const
{
	if (!isStr())
		return 0;

	if (typ == GT_SSTR)
		return scnt;

	return str.cnt;
}


/*
 *	Return a pointer to the string.  Ensure that the string is '\0'
 *	terminated.  The returned pointer is only valid so long as the
 *	original string is not changed.  The returned string does not
 *	necessarily reflect changes made to the original;  if the original
 *	is changed, CCS() should be called again.
 */
const char *
gist::CCS() const
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

	/*
	 *	Make sure the string is contiguous in memory and has a
	 *	'\0' on the end of it.  Note that we do no change `unique'.
	 */
	return gp->_strflatten(false, true, 0);
}


/*
 *	Flatten a string.  Once complete, the string storage for this
 *	gist will be one contiguous area of memory.  If `rw' is set, the
 *	resulting string will only be referenced by this gist, and thus
 *	can be modified in place.  If `need0' is true, the string will
 *	also have a '\0' appended to it (if necessary), so that it can be
 *	used directly as a C string.  A pointer to the first character in
 *	the string storage is returned.	 If `len' is non-NIL, the string
 *	length is stored there.	 Note that the resulting string could be
 *	a GT_SSTR or a GT_MSTR but never a GT_LSTR.
 */
char *
gist::_strflatten(bool rw, bool need0, unsigned * len) const
{
	/*
	 *	We cast away the const.	 We will change this gist, but the
	 *	resulting value will be the same (const in principal).
	 */
	gist * gp = (gist *)this;
	unsigned l;
	char * p;

	switch (gp->typ)
	{
	case GT_SSTR:
		p = &gp->sstr[0];
		l = gp->scnt;

		if (need0)
		{
			if (l >= sizeof gp->sstr)
				goto make_mstr;
			p[l] = '\0';
		}

		goto done;

	case GT_MSTR:
		p = gp->str.dat;
		l = gp->str.cnt;

		/*
		 *	Figure out if we can use the current storage or if
		 *	we need to make a copy.
		 */
		if (rw && !gp->unique)
		{
			/*
			 *	Copy, because the caller needs to write to
			 *	the storage and it appears that we share the
			 *	storage with another string.
			 */
			goto make_mstr;
		}

		if (need0)
		{
			/*
			 *	The caller expects a C string.
			 */
			if (l >= gp->str.sz)
			{
				/*
				 *	Copy, because there is no space to
				 *	put a '\0'.
				 */
				goto make_mstr;
			}

			if (p[l] != '\0')
			{
				if (!gp->unique)
				{
					/*
					 *	Copy, because there is no
					 *	'\0', and the storage appears
					 *	to be shared, so we can't
					 *	just put a '\0' there.
					 */
					goto make_mstr;
				}

				/*
				 *	We can modify our own storage, and
				 *	there is space, so put a '\0' at the
				 *	end of the string.
				 */
				p[l] = '\0';
			}
		}

		/*
		 *	It appears that we can just return the current
		 *	storage;  no need to copy.
		 */
		goto done;

		/*
		 *	Make a new copy of the string;	make it a GT_MSTR.
		 *	This could be as a result of less than ideal
		 *	conditions from above, or because the string is a
		 *	GT_LSTR, which we can't deal with at all.
		 */
	make_mstr:
	case GT_LSTR:
		p = (char *)gistInternal::strAlloc(l+1);
		strcpy(p, *gp, 0, l + 1);	// This adds a '\0' for us.

		gp->str.cnt = l;
		gp->str.sz = l + 1;
		gp->str.dat = p;
		gp->typ = GT_MSTR;
		gp->unique = true;

		goto done;

	default:
		throw typeError("strflatten");
	}

  done:
	if (len)
		*len = l;
	return p;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*
 *	Public gist member functions.
 */

/*
 *	gist::set() is a very commonly used function, particularly the first
 *	"C string" version.  It is coded fairly carefully.  In particular we
 *	could have a single version that took a default length argument, but
 *	that would cause the compiler to provide the default in application
 *	code for every call.  So, there are two versions of set().
 */
void
gist::set(const char * s)
{
	unsigned l;

	if (!s)
	{
		l = 0;
		goto ss;
	}

	l = ::strlen(s);

	if (l <= sizeof sstr)
	{
  ss:
		typ = GT_SSTR;
		scnt = l;
		if (l > 0)
			memcpy(sstr, s, l);
		if (l < sizeof sstr)
			sstr[l] = '\0';
	}
	else if (gistInternal::isReadOnlyData(s))
	{
		/*
		 *	If the C string is in read only memory, we can
		 *	refer to it directly.  With compilers like GCC,
		 *	strings constants are always in the .rodata
		 *	section, so this should be a big win.  In the
		 *	following assignment, we deliberately cast away
		 *	the const, but since `unique' is set to false
		 *	below, we will never attempt to modify the data.
		 *
		 *	We mark the string as not unique because the
		 *	caller may use it for other reasons and because
		 *	it is in read-only memory.
		 */
		str.cnt = l;
		str.sz = l + 1;
		str.dat = (char *)s;
		typ = GT_MSTR;
		unique = false;
	}
	else
	{
		str.cnt = l;
		str.sz = l + 1;
		str.dat = (char *)gistInternal::strAlloc(l+1);
		memcpy(str.dat, s, l);
		str.dat[l] = '\0';
		typ = GT_MSTR;
		unique = true;
	}
}

/******************************/

void
gist::set(const char * s, int l)
{
	if (!s)
	{
		l = 0;
		goto ss;
	}

	if (l < 0)
		l = ::strlen(s);

	if ((unsigned)l <= sizeof sstr)
	{
  ss:
		typ = GT_SSTR;
		scnt = l;
		if (l > 0)
			memcpy(sstr, s, l);
		if ((unsigned)l < sizeof sstr)
			sstr[l] = '\0';
	}
	else if (gistInternal::isReadOnlyData(s))
	{
		/*
		 *	If the C string is in read only memory, we can
		 *	refer to it directly.  With compilers like GCC,
		 *	strings constants are always in the .rodata
		 *	section, so this should be a big win.  In the
		 *	following assignment, we deliberately cast away
		 *	the const, but since `unique' is set to false
		 *	below, we will never attempt to modify the data.
		 *
		 *	We mark the string as not unique because the
		 *	caller may use it for other reasons and because
		 *	it is in read-only memory.
		 */
		str.cnt = l;
		str.sz = l + 1;
		str.dat = (char *)s;
		typ = GT_MSTR;
		unique = false;
	}
	else
	{
		str.cnt = l;
		str.sz = l + 1;
		str.dat = (char *)gistInternal::strAlloc(l+1);
		memcpy(str.dat, s, l);
		str.dat[l] = '\0';
		typ = GT_MSTR;
		unique = true;
	}
}

/******************************/

void
gist::copy(const char * s, int l)
{
	if (l < 0)
		l = ::strlen(s);

	if ((unsigned)l <= sizeof sstr)
	{
		memcpy(sstr, s, l);
		if ((unsigned)l < sizeof sstr)
			sstr[l] = '\0';
		typ = GT_SSTR;
		scnt = l;
	}
	else
	{
		str.cnt = l;
		str.sz = l + 1;
		str.dat = (char *)gistInternal::strAlloc(l+1);
		memcpy(str.dat, s, l);
		str.dat[l] = '\0';
		typ = GT_MSTR;
		unique = true;
	}
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

	case GT_SSTR:
	case GT_MSTR:
	case GT_LSTR:
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
	if (!isStr() || !r.isStr())
		throw typeError("strcmp");

	if (this == &r)
		return 0;

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
	if (i < 0)
		return 0;

	if (typ == GT_SSTR)
	{
		if ((unsigned)i >= scnt)
			return 0;
		pt = &sstr[0];
		ix = scnt;
		return scnt;
	}

	if (typ == GT_MSTR)
	{
		if ((unsigned)i >= str.cnt)
			return 0;
		pt = str.dat;
		ix = str.cnt;
		return str.cnt;
	}

	/*
	 *	The string is GT_LSTR.  Search for the string chunk that
	 *	contains the required index.  We search for index + 1 because
	 *	giIndexInt::previous() is a "less-than" operation and we want
	 *	"less-or-equal".
	 */
	giStr * sp = str.idx;
	int i1 = i + str.skp + sp->index.min;
	intKey * kp = sp->index.previous(i1 + 1);
	if (!kp)
	{
  bogus:
		throw gist::internalError("bogus index in gist::strpiece");
	}

	giSChunk * cp = kp->schunk;
	if (i1 < kp->key || i1 >= kp->key + (int)cp->len)
		goto bogus;

	pt = (const char *)cp->data;

	i1 -= kp->key;
	i1 = cp->len - i1;
	ix = i + i1;

	return cp->len;
}


unsigned
gist::strpiece(int & ix, const char *& pt) const
{
	if (!isStr())
		throw typeError("strpiece expects a string");

	unsigned l = _strpiece(ix, pt);
	if (l > 0)
		((gist *)this)->unique = false;
	return l;
}


unsigned
strpiece(const gist & g, int & ix, const char *& pt)
{
	if (!g.isStr())
		throw gist::typeError("strpiece expects a string");

	unsigned l = g._strpiece(ix, pt);
	if (l > 0)
		((gist &)g).unique = false;
	return l;
}

/******************************/

int
gist::_stridx(long idx) const
{
	if (idx < 0)
  tantrum:
		throw indexError("string index out of range");

	if (typ == GT_SSTR)
	{
		if ((unsigned)idx >= scnt)
			goto tantrum;
		return sstr[idx];
	}

	if ((unsigned)idx >= str.cnt)
		goto tantrum;

	if (typ == GT_MSTR)
		return str.dat[idx];

	giStr * sp = str.idx;

	/*
	 *	The string is multi.  Search for the string chunk that
	 *	contains the required index.  We search for index + 1 because
	 *	giIndexInt::previous() is a "less-than" operation and we want
	 *	"less-or-equal".
	 */
	int i1 = idx + str.skp + sp->index.min;
	intKey * kp = sp->index.previous(i1 + 1);
	if (!kp || kp->key > i1)
		throw gist::internalError("bogus index in gist::_strindex");

	i1 -= kp->key;
	giSChunk * cp = kp->schunk;
	return cp->data[i1];
}


int
gist::stridx(long idx) const
{
	if (!isStr())
		throw typeError("stridx expects a string");

	return _stridx(idx);
}


int
stridx(const gist & g, long idx)
{
	if (!g.isStr())
		throw gist::typeError("stridx expects a string");

	return g._stridx(idx);
}

/******************************/

char *
gist::strbuf(unsigned size)
{
	if (size <= sizeof sstr)
	{
		typ = GT_SSTR;
		scnt = size;
		return &sstr[0];
	}

	str.cnt = size;
	str.sz = size;
	str.dat = (char *)gistInternal::strAlloc(size);

	typ = GT_MSTR;
	unique = true;

	return str.dat;
}

/******************************/

char *
gist::strdup(unsigned start, unsigned count) const
{
	gist x;
	gist * gp;

	/*
	 *	Make sure it's a string.
	 */
	if (isStr())
		gp = (gist *)this;
	else
	{
		x = toString();
		gp = &x;
	}

	/*
	 *	Get the length, and return a static zero width string
	 *	if the source string is zero width.
	 */
	unsigned len = gp->strlen();
	if (len == 0)
		return "";

	/*
	 *	Allocate space for a string copy (using the GC allocator --
	 *	the gist::operator new), then copy the string there.
	 */
	char * sp = (char *)gistInternal::strAlloc(len+1);
	strcpy(sp, *gp, start, count);

	return sp;
}

/************************************************************/
/*
 *	String concatenation.
 */


void
giStr::makeMulti(unsigned len)
{
#if 0
	giSChunk * cp = (giSChunk *)gistInternal::alloc(sizeof (giSChunk));
	cp->data = data;
	cp->data0 = data;
	cp->len = len;

	index = new (giIndexInt::StrLevels) giIndexInt;
	index->insert(0, cp);
	index->min = 0;
	index->max = len;
#endif // 0
}


void
gist::strcat(const gist & r)
{
	throw notYetError("strcat");
#if 0
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
	if (rp->cnt <= giStr::maxCopy)
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
			strncpy(&ls->data[o], *rp, ls->size - o);
			cnt += l;
			if (ls->index)
			{
				ls->chunk->len += l;
				ls->index->max += l;
			}
			return;

		} while (0);

		/*
		 *	There was not enough space at the end of the chunk.
		 *	Try for a total length that might be less than the
		 *	copy limit.
		 */
		if (cnt + rp->cnt <= giStr::maxCopy)
		{
			/*
			 *	Create a new chunk and copy both the left
			 *	and right strings to it.
			 */
			giStr * nls = (giStr *)gistInternal::alloc(
							sizeof (giStr));
			nls->data = (char *)gistInternal::strAlloc(
							giStr::strChunk);
			nls->size = giStr::strChunk;
			nls->index = 0;

			strncpy(&nls->data[0], *this, giStr::strChunk);
			strncpy(&nls->data[cnt], *rp, giStr::strChunk - cnt);

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
		cp->data = (char *)gistInternal::strAlloc(giStr::strChunk);
		cp->data0 = cp->data;
		cp->len = rp->cnt;
		strncpy(cp->data, *rp, giStr::strChunk);

		ls->index->insert(ls->index->max, cp);
		ls->index->max += rp->cnt;

		ls->data = cp->data;
		// ls->len = rp->cnt;
		ls->chunk = cp;
		ls->size = giStr::strChunk;

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
	cnt += rp->cnt;

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
#endif // 0
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
	gist rx(r, count);
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
	unsigned c;

	if (!src.isStr())
		throw gist::typeError("strcpy expects a string");

	l = src._strpiece(ix, p);
	c = start - (ix - l);
	p += c;
	l -= c;
	c = 0;

	while (count > 0 && l > 0)
	{
		if (l > count)
			l = count;

		memcpy(dest, p, l);

		dest += l;
		c += l;
		count -= l;

		l = src._strpiece(ix, p);
	}

	if (count > 0)
	{
		*dest++ = '\0';
		c++;
	}

	return c;
}
