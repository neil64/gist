/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	set(char *), cons(char *), (char *)
 */

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
const unsigned	maxCopy = 32;

/*
 *	String space is allocated in multiples of this value.
 */
const unsigned	strChunk = 64;

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

	while ((c == *cp++))
	{
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A';
		else if (c >= 'a' && c <= 'z')
			c -= 'a';
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

	giStr * sp = (giStr *)gp->ptr;

	if (sp->index || !sp->hasNull || (!unique && rw))
		_strflatten();

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
	giStr * sp = (giStr *)gp->ptr;
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

	(void)strcopy(cp, *gp);

	gp->unique = true;
	sp->index = 0;
	sp->data = cp;
	sp->size = l;
	sp->hasNull = true;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*
 *	Public gist member functions.
 */

gist::gist(const char * s, int l)
{
	set(s, l);
}


void
gist::set(const char * s, int l)
{
	giStr * sp = new giStr;

	sp->index = 0;
	sp->data = (char *)s;		// Drop const, but unique is false.
	if (l < 0)
	{
		sp->hasNull = true;
		l = strlen(s);
	}
	sp->size = l;

	typ = GT_STR;
	unique = false;		// 'cause the caller may use it for other
	ptr = sp;		// things, and 'cause it could be in read-
	cnt = l;		// only memory, and other reasons.
	skip = 0;
}


void
gist::copy(const char * s, int l)
{
	giStr * sp = new giStr;
	sp->index = 0;
	if (l < 0)
		l = strlen(s);
	sp->data = (char *)gistInternal::alloc(l+1);
	memcpy(sp->data, s, l);
	sp->data[l] = '\0';
	sp->hasNull = true;

	typ = GT_STR;
	unique = true;
	ptr = sp;
	cnt = l;
	skip = 0;
}


gist
gist::toString() const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
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
			r.copy(a, &a[sizeof a] - ap);

			return r;
		}

	case GT_FLOAT:
		throw notYetError("toString");
	}
}


int
gist::strcmp(const gist & r) const
{
	if (this == &r)
		return 0;

	if (ptr == r.ptr && skip == r.skip)
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
	gist * lp;
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


unsigned
gist::_strpiece(int & ix, const char *& pt) const
{
	int i = ix;
	if (i < 0 || (unsigned)i >= cnt)
		return 0;

	giStr * sp = (giStr *)ptr;

	/*
	 *	If the string is single, it's easy.
	 */
	if (!sp->index)
	{
		pt = &((const char *)sp->data)[i + skip];
		ix = cnt;
		return cnt - i;
	}

	throw gist::notYetError("gist::_strpiece");

	/*
	 *	The string is multi.  Search for the string chunk that
	 *	contains the required index.  We search for index + 1 because
	 *	giIndexInt::previous() is a "less-than" operation and we want
	 *	"less-or-equal".
	 */
	i = i + skip + sp->index->min;
	intKey * kp = sp->index->previous(i + 1);
	if (!kp || kp->key > i)
		throw gist::internalError("bogus index in gist::strpiece");

	// i += min;
	// char k[sizeof (int)];
	// giStore::mkKey(k, i+1);
	// const char * kp = &k[0];
	// unsigned l = sizeof k;

	// {
		// intKey * kp = index->previous(i + 1);
		// st = key.str;
		// st = 0;
	// }
	// st = (giStore *)index->previous(kp, l);
	// int start = giStore::gtKey(kp);
	// if (!st || l != sizeof (k) || start > i)
	    // throw gist::internalError("bogus index in giStr::piece");

	// i -= start;



#if 0
	if (!isStr())
		throw typeError("strpiece");
	const char * result = ((giStr *)ptr)->piece(index, len);
	if (result)
		unique = false;
	return result;




	// giStore * st;

	if (!index)
	{
		if (idx < 0 || idx >= size)
			return 0;
		else
		{
			ptr = &((const char *)data)[idx];
			return size - idx;
		}
	}
	else
	{
		/*
		 *	Make a key out of the index + 1.  We add one because
		 *	giIndex::previous() is a "less-than" operation and
		 *	we want "less-or-equal".
		 */
		// i += min;
		// char k[sizeof (int)];
		// giStore::mkKey(k, i+1);
		// const char * kp = &k[0];
		// unsigned l = sizeof k;

		// {
			// intKey * kp = index->previous(i + 1);
			// st = key.str;
			// st = 0;
		// }
		// st = (giStore *)index->previous(kp, l);
		// int start = giStore::gtKey(kp);
		// if (!st || l != sizeof (k) || start > i)
		    // throw gist::internalError("bogus index in giStr::piece");

		// i -= start;
	}

	if ((unsigned)i >= st->size)
	{
		// len = 0;
		return 0;
	}
	else
	{
		// len = st->size - i;
		// idx += len;
		return 0;
		//return &((const char *)st->data)[i];
	}

#endif

}


unsigned
gist::strpiece(int & ix, const char *& pt) const
{
	unsigned l = _strpiece(ix, pt);
	if (l > 0)
		((gist *)this)->unique = false;
	return l;
}

/************************************************************/
/*
 *	String concatenation.
 */

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

	giStr * ls = (giStr *)ptr;
	giStr * rs = (giStr *)rp->ptr;

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
			else if (!ls->data)
				break;
			else
				o = ls->len;

			unsigned l = rp->cnt;
			if (l > (ls->size - o))
				break;

			/*
			 *	We are allowed to write to the left directly,
			 *	and there is space available.  Go for it.
			 */
			strcopy(&ls->data[o], r);
			// giStr::copy(&ls->data[o], rp);
			cnt += l;
			ls->len += l;
			if (!ls->index)
				ls->hasNull = false;
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

			strcopy(&nls->data[0], *this);
			strcopy(&nls->data[cnt], r);
			// giStr::copy(&nls->data[0], this);
			// giStr::copy(&nls->data[cnt], rp);

			unique = true;
			ptr = nls;
			skip = 0;
			cnt += rp->cnt;

			return;
		}

		/*
		 *	Simple copies failed, so now allocate a new chunk
		 *	and add it to the end of the current chunk list,
		 *	making the left into a multi if needed.
		 */
		if (!unique || !ls->index)
		{
			// makeNewIndex();
		}

		giChunk * cp; // = gistInternal::alloc(sizeof (giChunk));
		cp->data = (char *)gistInternal::alloc(strChunk);
		cp->len = rp->cnt;
		ls->data = cp->data;
		ls->len = rp->cnt;
		ls->size = strChunk;
		// giStrCopy(cp->data, rp);
		return;

	}

	(void)rs->index->search(0);	// just to quiet the compiler.

	throw gist::notYetError("giStr::concat");

	/*
	 *	Append the right onto the left.  If the right is a single
	 *	string, make a new chunk header for it, otherwise, copy
	 *	the chunks from the right to the left.  Both strings will
	 *	no longer be unique.
	 */
	if (!ls->index || !unique)
	{
		// makeNewIndex();
	}


/*


	-  if b's size is less than the copy limit
			(must copy, some code depends on it)
		-  if a is unique
			-  get final storage pointer and length
				(if single, then the main pointer)
				(if multi, last record, or main pointer)
			-  if there is space available
				-  copy b directly
				-  update info
				-  done
		-  if the total size is less than the copy limit
			-  allocate a new chunk
			-  copy both strings there.
			-  done
		-  if a is not unique
			-  create a new index structure
			-  (makes a unique)
		-  add a new chunk to the end of the string.
		-  copy b into it.

	(append function from here on...)
	-  if a is not unique,
		-  make a new index structure

	-  append the b chunk onto the string (insert into index).


---

	*  could say that unique means that the index has not changed,
	   and that if the giStr has pointers, then that is also unique.
	   So if another string chunk is appended to the index, the local
	   pointers will be NIL.


*/




#if 0

	if (a.isStr())
		ap = &a;
	else
	{
		ax = a.toString();
		ap = &ax;
	}

	if (b.isStr())
		bp = &b;
	else
	{
		bx = b.toString();
		bp = &bx;
	}

	return ax;

	// return *giStr::concat(ap, bp);
#endif
}


void
gist::strcat(int c)
{
	char a[2] = { c, '\0' };
	gist cx(a);
	strcat(cx);
}


void
gist::strcat(const char * r)
{
	gist rx(r);
	strcat(rx);
}

/************************************************************/

unsigned
gist::strcopy(char * dest, const gist & src, unsigned start, unsigned count)
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
