/*
 *	Gist -- Arrays.
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

/*
 *	Create a new array of `length', or change the size of an existing
 *	array to `length'.
 */
gist &
gist::array(unsigned l)
{
	if (typ != GT_ARRAY)
	{
		giArray * ap = new giArray;

		ap->skip = 0;
		ap->len = l;
		ap->index = new (giIndexInt::ArrayLevels) giIndexInt;
		ap->cache = 0;

		typ = GT_ARRAY;
		ptr = ap;
	}
	else
	{
		giArray * ap = (giArray *)ptr;

		if (l < ap->len)
		{
			unsigned i;
			unsigned pi = l / giAChunk::items;
			unsigned px = i % giAChunk::items;

			intKey * kp = ap->index->search(pi);

			if (kp)
			{
				giAChunk * pp = kp->achunk;
				for (i = px; px < giAChunk::items; i++)
					pp->g[i].clear();
			}

			i = pi;
			pi = (ap->len + 1) / giAChunk::items;
			for (; i < pi; i++)
				ap->index->remove(i);
		}
	}

	return *this;
}


gist &
gist::_arrayindex(long i)
{
	giArray * ap = (giArray *)ptr;

	if (i < 0)
		i = ap->len + i;
	if (i < 0 || (unsigned)i >= ap->len)
		throw indexError("array index out of range");

	unsigned pi = i / giAChunk::items;
	unsigned px = i % giAChunk::items;
	giAChunk * pp;

	if (ap->cache && ap->ci == pi)
		pp = ap->cache;
	else
	{
		intKey * kp = ap->index->search(pi);
		if (kp)
			pp = kp->achunk;
		else
		{
			/*
			 *	No such piece, so make one.
			 */
			pp = (giAChunk *)gistInternal::alloc(sizeof (giAChunk));
			ap->index->insert(pi, pp);
		}

		ap->cache = pp;
		ap->ci = pi;
	}

	return pp->g[px];
}


unsigned
gist::len() const
{
	if (typ == GT_ARRAY)
		return ((giArray *)ptr)->len;
	else if (typ == GT_STR)
		return strlen();
	else
		return 0;
}


unsigned
len(const gist & g)
{
	if (g.typ == gist::GT_ARRAY)
		return ((giArray *)g.ptr)->len;
	else if (g.typ == gist::GT_STR)
		return g.strlen();
	else
		return 0;
}
