/*
 *	Gist -- skip list implementation.
 *
 *	There are two skip implementations here, one that is keyed by an
 *	`int' and the other by a character point with length.  The speed
 *	of the `int' keyed one is fairly critical, and I could not figure a
 *	way to combine them without sacrificing some performance;  I tried
 *	converting the `int's to a string, a derived class, and templates.
 *	I didn't try macros, as I think that it would make the code less
 *	readable and would not save any time.
 */

#include	<memory.h>

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/
/*
 *	Skip list using an `int' key.
 */


/*
 *	Initialize an index.
 */
giIndexInt::giIndexInt()
{
	int		i;

	for (i = 0; i < MaxLevel; i++)
		head[i] = 0;
	levels = 1;
}


/*
 *	Free all resources in a skip list.
 */
giIndexInt::~giIndexInt()
{
	intKey *	a;
	intKey *	b;

	for (a = head[0]; a; a = b)
	{
		b = a->fwd[0];
		gistInternal::free(a);
	}
}


/*
 *	Search for a record matching `key'.
 */
intKey *
giIndexInt::search(int key)
{
	intKey * p;
	intKey ** pp = &head[0];
	int i = levels - 1;			// levels always >= 1

	do
	{
		while ((p = pp[i]) && p->key < key)
			pp = &p->fwd[0];
		if (p && p->key == key)
			return p;
	} while (--i >= 0);

	return 0;
}


/*
 *	Return the first entry.
 */
intKey *
giIndexInt::first()
{
	return head[0];
}


/*
 *	Return the last entry.
 */
intKey *
giIndexInt::last()
{
	intKey * p;
	intKey ** pp = &head[0];
	int i = levels - 1;

	do
		while ((p = pp[i]))
			pp = &p->fwd[0];
	while (--i >= 0);

	return p;
}


/*
 *	Return the record immediately following the given key.
 */
intKey *
giIndexInt::next(int key)
{
	intKey * p;
	intKey ** pp = &head[0];
	int i = levels - 1;

	do
	{
		while ((p = pp[i]) && p->key < key)
			pp = &p->fwd[0];
		if (p && p->key == key)
			return p->fwd[0];
	} while (--i >= 0);

	if (p)
		p = p->fwd[0];
	return p;
}


/*
 *	Return the record immediately preceding the given key/tag combo.
 */
intKey *
giIndexInt::previous(int key)
{
	intKey * p;
	intKey ** pp = &head[0];
	intKey * px = 0;
	int i = levels - 1;

	do
	{
		while ((p = pp[i]) && p->key < key)
		{
			px = p;
			pp = &p->fwd[0];
		}
	} while (--i >= 0);

	return px;
}


/*
 *	Insert a new entry.  If an entry already exists with the same key,
 *	The entry is added after any all entries with matching keys.
 */
bool
giIndexInt::insert(int key, giChunk * data)
{
	intKey ** update[MaxLevel];
	intKey * p;
	intKey ** pp = &head[0];
	int i = levels - 1;

	do
	{
		while ((p = pp[i]) && p->key < key)
			pp = &p->fwd[0];
		update[i] = pp;
	} while (--i >= 0);

	if (p && p->key == key)
		return false;

	/*
	 *	Decide on the height of the new entry.
	 */
	unsigned level = 1;
	while (level < MaxLevel && level <= levels)
	{
		unsigned	rand;

		/*
		 *	Simple fast random number generator.
		 */
		{
			static unsigned	z = 362436069,
					w = 521288629,
					jsr = 123456789,
					jcong = 380116160;
			unsigned	mwc;

			z = 36969 * (z & 65535) + (z >> 16);
			w = 18000 * (w & 65535) + (w >> 16); 

			mwc = (z << 16) + (w & 65535);

			jsr ^= jsr << 17;
			jsr ^= jsr >> 13;
			jsr ^= jsr << 15;

			jcong = 69069 * jcong + 1234567;

			rand = (mwc ^ jcong) + jsr;
		} 

		if (rand & 0x80000000)
			break;

		level++;
	}

	i = sizeof (intKey) + sizeof (intKey *) * level;
	p = (intKey *)gistInternal::alloc(i);
	p->key = key;
	p->data = data;

	if (level > levels)
	{
		update[level - 1] = &head[0];
		levels = level;
	}

	do
	{
		--level;
		p->fwd[level] = update[level][level];
		update[level][level] = p;
	} while (level > 0);

	return true;
}


/*
 *	Remove an entry (if it exists).
 */
bool
giIndexInt::remove(int key)
{
	intKey ** update[MaxLevel];
	intKey * p;
	intKey ** pp = &head[0];
	int i = levels - 1;

	do
	{
		while ((p = pp[i]) && p->key < key)
			pp = &p->fwd[0];
		update[i] = pp;
	} while (--i >= 0);

	if (!p || p->key != key)
		return false;

	for (i = 0; (unsigned)i < levels; i++)
	{
		if (update[i][i] != p)
			break;
		update[i][i] = p->fwd[i];
	}

	gistInternal::free(p);

	while (levels > 1 && head[levels - 1] == 0)
		levels--;

	return true;
}

#if 0

/**********************************************************************/

int
cmp(strNode * l, strNode * r)
{
	if (l->key < r->key)
		return -1;
	else if (l->key > r->key)
		return 1;
	else
		return 0;
}


int
cmp(tblNode * l, tblNode * r)
{
	unsigned n = l->klen;
	if (n > r->klen)
		n = r->klen;

	int x = memcmp(l->key, r->key, n);
	if (x != 0)
		return x;

	if (l->klen < r->klen)
		return -1;
	else if (l->klen > r->klen)
		return 1;
	else
		return 0;
}

/****************************************/

/*
 *	Initialize an index.
 */
template<class T>
giIndex<T>::giIndex<T>()
{
	int		i;

	for (i = 0; i < MaxLevel; i++)
		head[i] = 0;
	levels = 1;
}


/*
 *	Free all resources in a skip list.
 */
template<class T>
giIndex<T>::~giIndex<T>()
{
	sk_t *		a;
	sk_t *		b;

	for (a = head[0]; a; a = b)
	{
		b = a->fwd[0];
		delete a;
	}
}


/*
 *	Search for a record matching `key'.
 */
template<class T>
T *
giIndex<T>::search(T * key)
{
	int		i;
	T **		pp;
	T *		p;
	int		r = 1;

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = cmp(p, key)) < 0)
			pp = &p->fwd[0];

		if (r == 0)
			return *p;
		if (r > 0)
			break;
	}

	return 0;
}


/*
 *	Return the first entry.
 */
template<class T>
T *
giIndex<T>::first()
{
	return head[0];
}


/*
 *	Return the last entry.
 */
template<class T>
T *
giIndex<T>::last()
{
	int	i;
	T **	pp = &head[0];
	T *	p = 0;

	for (i = levels - 1; i >= 0; i--)
		while (pp[i])
		{
			p = pp[i];
			pp = &p->fwd[0];
		}

	return p;
}


/*
 *	Return the record immediately following the given key.
 */
template<class T>
T *
giIndex<T>::next(T * key)
{
	int		i;
	T **		pp;
	T *		p = 0;
	int		r = 1;

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = cmp(p, key)) < 0)
			pp = &p->fwd[0];

		if (r < 0)
			continue;

		if (r == 0)
			p = p->fwd[0];
		break;
	}

	return p;
}


/*
 *	Return the record immediately preceding the given key/tag combo.
 */
template<class T>
T *
giIndex<T>::previous(T * key)
{
	int		i;
	T **		pp;
	T *		px;
	T *		p;
	int		r;

	pp = &head[0];
	px = 0;
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = cmp(p, key)) < 0)
		{
			px = p;
			pp = &p->fwd[0];
		}
	}

	return px;
}



/*
 *	Insert a new entry.  If an entry already exists with the same key,
 *	The entry is added after any all entries with matching keys.
 */
template<class T>
bool
giIndex<T>::insert(T * key)
{
	int		i;
	T **		pp;
	T *		p;
	int		r;
	T **		update[MaxLevel];
	unsigned	level;

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
#warning "<= looks wrong"
		while ((p = pp[i]) && (r = cmp(p, key)) <= 0)
			pp = &p->fwd[0];
		update[i] = pp;
	}
#warning "need to return false if found"

	/*
	 *	Decide on the height of the new entry.
	 */
	level = 1;
	while (level < MaxLevel && level <= levels)
	{
		unsigned	rand;

		/*
		 *	Simple fast random number generator.
		 */
		{
			static unsigned	z = 362436069,
					w = 521288629,
					jsr = 123456789,
					jcong = 380116160;
			unsigned	mwc;

			z = 36969 * (z & 65535) + (z >> 16);
			w = 18000 * (w & 65535) + (w >> 16); 

			mwc = (z << 16) + (w & 65535);

			jsr ^= jsr << 17;
			jsr ^= jsr >> 13;
			jsr ^= jsr << 15;

			jcong = 69069 * jcong + 1234567;

			rand = (mwc ^ jcong) + jsr;
		} 

		if (rand & 0x80000000)
			break;

		level++;
	}

#warning "insert stuff ..."
	// i = sizeof (sk_t) + sizeof (sk_t *) * level;
	// p = (sk_t *)gistInternal::alloc(i + len);
#warning "no internal pointers"
	// p->key = (char *)p + i;
	// memcpy(p->key, key, len);
	// p->len = len;
	// p->ref = ref;

	if (level > levels)
	{
		update[level - 1] = &head[0];
		levels = level;
	}

	do
	{
		--level;
		p->fwd[level] = update[level][level];
		update[level][level] = p;
	} while (level > 0);

	return true;
}


/*
 *	Remove an entry (if it exists).
 */
template<class T>
bool
giIndex<T>::remove(T * key)
{
	int		i;
	T **		pp;
	T *		p = 0;
	int		r = 1;
	T **		update[MaxLevel];

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = cmp(p, key)) < 0)
			pp = &p->fwd[0];
		update[i] = pp;
	}

	if (r != 0)
		return false;		// Doesn't exist

	for (i = 0; (unsigned)i < levels; i++)
	{
		if (update[i][i] != p)
			break;
		update[i][i] = p->fwd[i];
	}

#warning "free index"
	gistInternal::free(p);

	while (levels > 1 && head[levels - 1] == 0)
		levels--;

	return true;
}

#endif // 0
