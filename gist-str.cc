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
const int	maxCopy = 32;

/*
 *	String space is allocated in multiples of this value.
 */
const int	strChunk = 64;

/**********************************************************************/
/*
 *	Internal gist member functions.
 */

void
giStr::mkTmp(giStore & st, const char * s)
{
	st.data = (void *)s;
	st.size = strlen(s);
	index = 0;
	str = &st;
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
giStr::flatten()
{
	if (!index && str->hasNull && str->data)
		return;

	throw gist::notYetError("giStr::flatten");

	// always add a '\0'
	// set multiRef and hasNull
	// may need to move a non-index string if there is no '\0'.
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
		st = str;
	else
	{
		/*
		 *	Make a key out of the index + 1.  We add one because
		 *	giIndex::previous() is a "less-than" operation and
		 *	we want "less-or-equal".
		 */
		i += min;
		char k[sizeof (int)];
		giStore::mkKey(k, i+1);
		const char * kp = &k[0];
		unsigned l = sizeof k;

		st = (giStore *)index->previous(kp, l);
		int start = giStore::gtKey(kp);
		if (!st || l != sizeof (k) || start > i)
		    throw gist::internalError("bogus index in giStr::piece");

		i -= start;
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


int
giStr::cmp(giStr * r)
{
	if (this == r)
		return 1;

	int ll = 0, rl = 0;
	int li= 0, ri = 0;
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
gist::strCast(int multi) const
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

	sp->flatten();

	return (char *)sp->str->data;
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
	giStore * st = giStore::alloc(0);
	st->data = (char *)s;		// Drop const, but unique is false.
	if (l < 0)
	{
		st->hasNull = true;
		l = strlen(s);
	}
	st->size = l;

	giStr * sp = new giStr;
	sp->str = st;
	sp->index = 0;

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
	sp->str = st;
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
gist::cmp(const char * s) const
{
	gist x;
	gist * gp;

	/*
	 *	Grab the string pointer.  If the object is not a string
	 *	try to make it into one.
	 */
	if (isStr())
		gp = (gist *)this;
	else
	{
		x = toString();
		gp = &x;
	}

	giStr * sp = (giStr *)gp->ptr;

	/*
	 *	Set up a temporary internal string structure for the right
	 *	operand then do the compare.
	 */
	giStore st1;
	giStr s1;
	s1.mkTmp(st1, s);

	return sp->cmp(&s1);
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


void
gist::strcat(const gist & r)
{
	const gist * rp;

	if (!isStr())
		throw typeError("strcat");

	if (r.isStr())
		rp = &r;
	else
		rp = new gist(r.toString());

	giStr * sp = (giStr *)ptr;

	if (!sp->index && unique)
	{
		/*
		 *	There is a chance that we could just copy the
		 *	new string data onto the end of the current
		 *	string, assuming there is space.
		 */
	}

/*

	-  This will become "the" concatenation function, since access to
	   the gist is needed.

	-  if a is a single string
		if a's string is not a multi
			if b's size is less than copy limit,
			   and there is space available to copy it to a
				copy b into a
				done

		else if a's size + b's size is less than the copy limit
			allocate a new string and copy both to it.
			done

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
gist::strcat(const char * r)
{
	gist rx(r);
	strcat(rx);
}
