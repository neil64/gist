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
	// unsigned		i;

	//for (unsigned i = 0; i < maxLevel; i++)
	//	head[i] = 0;
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


void *
giIndexInt::operator new(unsigned sz, int lvl)
{
	sz += lvl * sizeof (intKey *);
	giIndexInt * ip = (giIndexInt *)gistInternal::alloc(sz);
	ip->maxLevel = lvl;

	return (void *)ip;
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
	while (level < maxLevel && level <= levels)
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

		if (rand & 0x80000000)		// P = 1/2
			break;

		level++;
	}

	i = sizeof (intKey) + sizeof (intKey *) * level;
	p = (intKey *)gistInternal::alloc(i);
	p->key = key;
	p->chunk = data;

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

/**********************************************************************/
/**********************************************************************/
/*
 *	Skip list using a `gist' key.
 */


/*
 *	Initialize an index.
 */
giIndexGist::giIndexGist()
{
	// unsigned		i;

	//for (unsigned i = 0; i < MaxLevel; i++)
	//	head[i] = 0;
	levels = 1;
}


/*
 *	Free all resources in a skip list.
 */
giIndexGist::~giIndexGist()
{
	gistKey *	a;
	gistKey *	b;

	for (a = head[0]; a; a = b)
	{
		b = a->fwd[0];
		gistInternal::free(a);
	}
}


void *
giIndexGist::operator new(unsigned sz)
{
	giIndexGist * ip = (giIndexGist *)gistInternal::alloc(sz);
	return (void *)ip;
}


/*
 *	Compare two gist values, for the skip list.  This causes ranges of
 *	object types, so keys with unlike types work in gist tables.  Like
 *	gist objects compare as you would expect, and unlike objects compare
 *	so that NIL < STR < INT < FLOAT < LONG < REAL, which just happens to
 *	be the order of gist types in gist.h.  Some types cannot be keys,
 *	such as arrays, tables and code.  It is assumed that the caller has
 *	taken care of those types.
 */
static int
cmpGist(const gist & l, const gist & r)
{
	if (l.typ == r.typ)
	{
		switch (l.typ)
		{
		case gist::GT_NIL:
		default:
			return 0;

		case gist::GT_STR:
			return strcmp(l, r);

		case gist::GT_INT:
			if (l.val == r.val)
				return 0;
			else if (l.val < r.val)
				return -1;
			else
				return 1;

		case gist::GT_FLOAT:
			if (l.dval == r.dval)
				return 0;
			else if (l.dval < r.dval)
				return -1;
			else
				return 1;

		case gist::GT_PTR:
			if (l.ptr == r.ptr)
				return 0;
			else if (l.ptr < r.ptr)
				return -1;
			else
				return 1;
		}
	}
	else if (l.typ < r.typ)
		return -1;
	else
		return 1;
}


/*
 *	Search for a record matching `key'.  If the entry is not found and
 *	`make' is true, insert a new entry.
 */
gistKey *
giIndexGist::search(const gist & key, bool make)
{
	gistKey ** update[MaxLevel];
	gistKey * p;
	gistKey ** pp = &head[0];
	int i = levels - 1;
	int x = 1;

	do
	{
		while ((p = pp[i]) && (x = cmpGist(p->key, key)) < 0)
			pp = &p->fwd[0];
		update[i] = pp;
		if (x == 0)
			return p;
	} while (--i >= 0);

	if (!make)
		return 0;

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

		if (rand & 0x80000000)		// P = 1/2
			break;

		level++;
	}

	i = sizeof (gistKey) + sizeof (gistKey *) * level;
	p = (gistKey *)gistInternal::alloc(i);
	p->key = key;
	p->val.clear();

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

	return p;
}


/*
 *	Return the first entry.
 */
gistKey *
giIndexGist::first()
{
	return head[0];
}


/*
 *	Return the last entry.
 */
gistKey *
giIndexGist::last()
{
	gistKey * p;
	gistKey ** pp = &head[0];
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
gistKey *
giIndexGist::next(const gist & key)
{
	gistKey * p;
	gistKey ** pp = &head[0];
	int i = levels - 1;
	int x = 1;

	do
	{
		while ((p = pp[i]) && (x = cmpGist(p->key, key)) < 0)
			pp = &p->fwd[0];
		if (x == 0)
			return p->fwd[0];
	} while (--i >= 0);

	if (p)
		p = p->fwd[0];
	return p;
}


/*
 *	Return the record immediately preceding the given key/tag combo.
 */
gistKey *
giIndexGist::previous(const gist & key)
{
	gistKey * p;
	gistKey ** pp = &head[0];
	gistKey * px = 0;
	int i = levels - 1;

	do
	{
		while ((p = pp[i]) && cmpGist(p->key, key))
		{
			px = p;
			pp = &p->fwd[0];
		}
	} while (--i >= 0);

	return px;
}


#if 0

/*
 *	Insert a new entry.  If an entry already exists with the same key,
 *	The entry is added after any all entries with matching keys.
 */
bool
giIndexGist::insert(const gist & key, const gist & data)
{
	gistKey ** update[MaxLevel];
	gistKey * p;
	gistKey ** pp = &head[0];
	int i = levels - 1;
	int x = 1;

	do
	{
		while ((p = pp[i]) && (x = cmpGist(p->key, key)) < 0)
			pp = &p->fwd[0];
		update[i] = pp;
	} while (--i >= 0);

	if (x == 0)
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

		if (rand & 0x80000000)		// P = 1/2
			break;

		level++;
	}

	i = sizeof (gistKey) + sizeof (gistKey *) * level;
	p = (gistKey *)gistInternal::alloc(i);
	p->key = key;
	p->val = data;

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

#endif // 0


/*
 *	Remove an entry (if it exists).
 */
bool
giIndexGist::remove(const gist & key)
{
	gistKey ** update[MaxLevel];
	gistKey * p;
	gistKey ** pp = &head[0];
	int i = levels - 1;
	int x = 1;

	do
	{
		while ((p = pp[i]) && (x = cmpGist(p->key, key)) < 0)
			pp = &p->fwd[0];
		update[i] = pp;
	} while (--i >= 0);

	if (x != 0)
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
