/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	set(char *), cons(char *), (char *)
 */

#include	<string.h>
#include	<memory.h>
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

void
giStr::mkTmp(const char * s)
{
	index = 0;
	data = (char *)s;
	size = strlen(s);
	hasNull = true;
}


//gist *
// giStr::concat(const gist * a, const gist * b)
void
giStr::concat(gist & a, const gist * b)
{
#if 0
	if (b->cnt == 0)
		return (gist *)a;
	if (a->cnt == 0)
		return (gist *)b;
#endif

/*
	if a is a single string
		if a's string is not a multi
			if b's size is less than copy limit,
			   and there is space available to copy it to a
				copy b into a
				done

		else if a's size + b's size is less than the copy limit
			allocate a new string and copy both to it.
			done

*/








	throw gist::notYetError("giStr::concat");
}


void
giStr::flatten(bool null)
{
	// if (!index && str->hasNull && str->data)
		// return;

	throw gist::notYetError("giStr::flatten");

	// always add a '\0'
	// set multiRef and hasNull
	// may need to move a non-index string if there is no '\0'.
	// don't copy if already single and there is space for the '\0'

	// some calls change this gist ((char *)),
	//	others need a new copy (copy())
}


long
giStr::toInt(int sign)
{
	throw gist::notYetError("giStr::toInt");
}


double
giStr::toFloat()
{
	throw gist::notYetError("giStr::toFloat");
}


const char *
giStr::piece(int & idx, int & len)
{
	giStore * st;
	int i = idx;

	if (!index)
	{
#warning "this needs to be a gist::piece"  // so it can get access to skip,cnt
		; // st = str;
	}
	else
	{
		/*
		 *	Make a key out of the index + 1.  We add one because
		 *	giIndex::previous() is a "less-than" operation and
		 *	we want "less-or-equal".
		 */
#warning "wrong here also"
		// i += min;
		char k[sizeof (int)];
		giStore::mkKey(k, i+1);
		const char * kp = &k[0];
		unsigned l = sizeof k;

#warning "check this"
		{
			intKey * kp = index->previous(i + 1);
			// st = key.str;
			st = 0;
		}
		// st = (giStore *)index->previous(kp, l);
		// int start = giStore::gtKey(kp);
		// if (!st || l != sizeof (k) || start > i)
		    // throw gist::internalError("bogus index in giStr::piece");

		// i -= start;
	}

	if ((unsigned)i >= st->size)
	{
		len = 0;
		return 0;
	}
	else
	{
		len = st->size - i;
		idx += len;
		return &((const char *)st->data)[i];
	}
}



void
giStr::copy(char * to, const gist * from)
{
	giStr * sp = (giStr *)from->ptr;

	if (!sp->index)
		memcpy(to, &sp->data[from->skip], from->cnt);
	else
	{
		int idx = 0;
		int len;
		const char * pp;

		while ((pp = sp->piece(idx, len)))
		{
			memcpy(to, pp, len);
			to += len;
		}
	}
}

#if 0

int
giStr::cmp(giStr * r)
{
	if (this == r)
		return 1;

	int ll = 0, rl = 0;
	int li = 0, ri = 0;
	const char * lp = 0, * rp = 0;

	for (;;)
	{
		if (ll == 0)
			lp = piece(li, ll);
		if (rl == 0)
			rp = r->piece(ri, rl);

		if (!lp || !rp)
			break;

		int l = ll;
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

#endif

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
		sp->flatten(true);

	return sp->data;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

#if 0

struct str
{
	-  multi-ref flag, set and never reset.
	-  buffer
	-  buffer size
};


set string:

	-  allocate a str
	-  set the multi-ref flag
	-  set the reference to the passed string.




string buffer management
------------------------

allocate a string:

	-  round up the amount requested
	-  allocate space for the buffer
	-  allocate a str
	-  set values

concatenate (append) two strings (a += operation):

	-  if the left is not multi-ref
		-  if there is space to append the right onto the left,
			copy the data
	-  if the left is not already a multi-str,
		-  allocate a multi-str
		-  insert the left onto the string with zero offset
	-  insert the right onto the multi-str, with correct offset.


prepend (right is prepended onto the left):

	-  similar to append


index a value in the string:

	-  find the str block
	-  calculate the pointer
	-  return the pointer and the number of bytes remaining from there.



#endif // 0

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
	if (l < 0)
		l = strlen(s);
	giStore * st = giStore::alloc(l+1);
	memcpy(st->data, s, l);
	((char *)st->data)[l] = '\0';

	giStr * sp = new giStr;
#warning "wrong here"
	// sp->str = st;
	sp->index = 0;

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
gist::_strcmp(const gist & r) const
{
	if (this == &r)
		return 0;

	giStr * ls = (giStr *)ptr;
	giStr * rs = (giStr *)r.ptr;
	unsigned lc = cnt;
	unsigned rc = r.cnt;

	if (ls == rs && skip == r.skip)
	{
		if (lc == rc)
			return 0;
		else if (lc < rc)
			return -1;
		else
			return 1;
	}

	int ll = 0, rl = 0;
	int li = skip, ri = r.skip;
	const char * lp = 0, * rp = 0;

	for (;;)
	{
		if (ll == 0)
			lp = ls->piece(li, ll);
		if (rl == 0)
			rp = rs->piece(ri, rl);

		if (!lp || !rp)
			break;

		int l = ll;
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

	if (lc == rc)
		return 0;
	else if (lc < rc)
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

	return lp->_strcmp(r);
}


const char *
gist::strpiece(int & index, int & len)
{
	if (!isStr())
		throw typeError("strpiece");
	const char * result = ((giStr *)ptr)->piece(index, len);
	if (result)
		unique = false;
	return result;
}

/**********************************************************************/
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
			giStr::copy(&ls->data[o], rp);
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

			giStr::copy(&nls->data[0], this);
			giStr::copy(&nls->data[cnt], rp);

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
