/*
 *	Gist -- Arrays.
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist &
gist::_arrayindex(long i)
{
	if (i < 0)
		throw indexError("-ve array index not allowed (yet)");

	giArray * ap = (giArray *)ptr;
	unsigned pi = i / giAChunk::items;
	unsigned px = i % giAChunk::items;
	giAChunk * pp;

	if (ap->cache && ap->ci == pi)
		pp = ap->cache;
	else
	{
		intKey * kp = ap->index->search(pi);
		if (kp)
			pp = (giAChunk *)kp->chunk;
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

		if ((unsigned)i >= cnt)
			cnt = i + 1;
	}

	return pp->g[px];
}


gist &
gist::_makearray(long i)
{
	if (i < 0)
		throw indexError("-ve array index not allowed (yet)");

	unsigned pi = i / giAChunk::items;
	unsigned px = i % giAChunk::items;

	giArray * ap = new giArray;
	giAChunk * pp = (giAChunk *)gistInternal::alloc(sizeof (giAChunk));

	ap->index = new (giIndexInt::ArrayLevels) giIndexInt;
	ap->index->insert(pi, pp);
	ap->cache = pp;
	ap->ci = pi;

	typ = GT_ARRAY;
	ptr = ap;
	cnt = i + 1;
	skip = 0;

	return pp->g[px];
}


unsigned
gist::len() const
{
	if (typ == GT_ARRAY || typ == GT_STR)
		return cnt;
	else
		return 0;
}


unsigned
len(const gist & g)
{
	if (g.typ == gist::GT_ARRAY || g.typ == gist::GT_STR)
		return g.cnt;
	else
		return 0;
}
