/*
 *	Gist -- Indexing.
 */

#ifndef __GIST_INDEX_H__
#define __GIST_INDEX_H__


/**********************************************************************/
/*
 *	Indexing.
 */

#include	<memory.h>

struct strNode
{
	int		key;
	giStr *		str;
	strNode	*	fwd[0];

	int
	cmp(strNode * r)
	{
		if (key < r->key)
			return -1;
		else if (key > r->key)
			return 1;
		else
			return 0;
	}
};


struct tblNode
{
	const char *	key;
	unsigned	klen;
	gist		val;
	tblNode	*	fwd[0];

	int
	cmp(tblNode * r)
	{
		unsigned n = klen;
		if (n > r->klen)
			n = r->klen;

		int x = memcmp(key, r->key, n);
		if (x != 0)
			return x;

		if (klen < r->klen)
			return -1;
		else if (klen > r->klen)
			return 1;
		else
			return 0;
	}
};

/**************************************************/

template<class T>
class giIndex
{
    public:
	giIndex();
	~giIndex();

    public:
	/*
	 *	Search will locate and return the entry that matches the
	 *	given `key'.
	 */
	T *		search(T *);

	/*
	 *	Return the first or the last tuple in the index.
	 */
	T *		first();
	T *		last();

	/*
	 *	Find the tuple either just before or just after the
	 *	record matching the given `key'.  The key of the matched
	 *	record replaces `key' and `len'.  If no records remain,
	 *	NIL is returned and `key' is set to NIL and `len' to zero.
	 *	If NIL is passed as the key, NIL will be returned.
	 */
	T *		next(T *);
	T *		previous(T *);

	/*
	 *	Insert the given `ref' at the given `key' and return true.
	 *	If an entry already exists, false is returned.
	 */
	bool		insert(T *);

	/*
	 *	Remove the entry matching the given `key', if it exists.
	 *	True is returned if it was deleted, or false if it was not
	 *	found.
	 */
	bool		remove(T *);

    private:
	enum
	{
		MaxLevel = 16,
		P = (((unsigned)-1) / 2)
	};

	T *		head[MaxLevel];
	unsigned	levels;

	T *		cache;
	int		min, max;
};

/**********************************************************************/

#include	<memory.h>


#if 0

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

#endif // 0

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
		while ((p = pp[i]) && (r = p->cmp(key)) < 0)
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
		while ((p = pp[i]) && (r = p->cmp(key)) < 0)
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

	pp = &head[0];
	px = 0;
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && p->cmp(key) < 0)
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
		while ((p = pp[i]) && (r = p->cmp(key)) <= 0)
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
		while ((p = pp[i]) && (r = p->cmp(key)) < 0)
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

/**********************************************************************/

#endif // __GIST_INDEX_H__
