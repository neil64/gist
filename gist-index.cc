/*
 *	Gist -- skip list implementation.
 */

#include	<memory.h>

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

int
giIndex::sk_t::cmp(const char * ke, unsigned le)
{
	unsigned l = le;
	if (l > len)
		l = len;

	int r = memcmp(key, ke, l);
	if (r != 0)
		return r;

	if (len < le)
		return -1;
	else if (len > le)
		return 1;
	else
		return 0;

}

/****************************************/

/*
 *	Initialize an index.
 */
giIndex::giIndex()
{
	int		i;

	for (i = 0; i < MaxLevel; i++)
		head[i] = 0;
	levels = 1;
}


/*
 *	Free all resources in a skip list.
 */
giIndex::~giIndex()
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
void *
giIndex::search(const char * key, unsigned len)
{
	int		i;
	sk_t **		pp;
	sk_t *		p;
	int		r = 1;

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = p->cmp(key, len)) < 0)
			pp = &p->fwd[0];

		if (r == 0)
			return p->ref;
		if (r > 0)
			break;
	}

	return 0;
}


/*
 *	Return the first entry.
 */
void *
giIndex::first(const char *& key, unsigned & len)
{
	sk_t * p = head[0];

	if (p)
	{
		key = p->key;
		len = p->len;
		return p->ref;
	}
	else
		return 0;
}


/*
 *	Return the last entry.
 */
void *
giIndex::last(const char *& key, unsigned & len)
{
	int		i;
	sk_t **		pp = &head[0];
	sk_t *		p = 0;

	for (i = levels - 1; i >= 0; i--)
		while (pp[i])
		{
			p = pp[i];
			pp = &p->fwd[0];
		}

	if (p)
	{
		key = p->key;
		len = p->len;
		return p->ref;
	}
	else
		return 0;
}


/*
 *	Return the record immediately following the given key.
 */
void *
giIndex::next(const char *& key, unsigned & len)
{
	const char *	k1 = key;
	unsigned	l1 = len;
	int		i;
	sk_t **		pp;
	sk_t *		p = 0;
	int		r = 1;

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = p->cmp(k1, l1)) < 0)
			pp = &p->fwd[0];

		if (r == 0)
		{
			p = p->fwd[0];
			goto gotIt;
		}
		if (r > 0)
			break;
	}

  gotIt:
	if (p)
	{
		key = p->key;
		len = p->len;
		return p->ref;
	}
	else
		return 0;
}


/*
 *	Return the record immediately preceding the given key/tag combo.
 */
void *
giIndex::previous(const char *& key, unsigned & len)
{
	const char *	k1 = key;
	unsigned	l1 = len;
	int		i;
	sk_t **		pp;
	sk_t *		px;
	sk_t *		p;
	int		r;

	pp = &head[0];
	px = 0;
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = p->cmp(k1, l1)) < 0)
		{
			px = p;
			pp = &p->fwd[0];
		}
	}

	if (px)
	{
		key = px->key;
		len = px->len;
		return px->ref;
	}
	else
		return 0;
}



/*
 *	Insert a new entry.  If an entry already exists with the same key,
 *	The entry is added after any all entries with matching keys.
 */
int
giIndex::insert(const char * key, unsigned len, void * ref)
{
	int		i;
	sk_t **		pp;
	sk_t *		p;
	int		r;
	sk_t **		update[MaxLevel];
	unsigned	level;

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = p->cmp(key, len)) <= 0)
			pp = &p->fwd[0];
		update[i] = pp;
	}

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

	i = sizeof (sk_t) + sizeof (sk_t *) * level;
	p = (sk_t *)gistInternal::alloc(i + len);
	p->key = (char *)p + i;
	memcpy(p->key, key, len);
	p->len = len;
	p->ref = ref;

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

	return 1;
}


/*
 *	Remove an entry (if it exists).
 */
int
giIndex::remove(const char * key, unsigned len)
{
	int		i;
	sk_t **		pp;
	sk_t *		p = 0;
	int		r = 1;
	sk_t **		update[MaxLevel];

	pp = &head[0];
	for (i = levels - 1; i >= 0; i--)
	{
		while ((p = pp[i]) && (r = p->cmp(key, len)) < 0)
			pp = &p->fwd[0];
		update[i] = pp;
	}

	if (r != 0)
		return 0;		// Doesn't exist

	for (i = 0; (unsigned)i < levels; i++)
	{
		if (update[i][i] != p)
			break;
		update[i][i] = p->fwd[i];
	}

	gistInternal::free(p);

	while (levels > 1 && head[levels - 1] == 0)
		levels--;

	return 1;
}
