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
		intern = ap;
	}
	else
	{
		giArray * ap = (giArray *)intern;

		if (l == 0 && ap->len > 0)
		{
			ap->skip = 0;
			ap->index = new (giIndexInt::ArrayLevels) giIndexInt;
		}
		else if (l < ap->len)
		{
			unsigned i;
			unsigned pi = l / giAChunk::items;
			unsigned px = l % giAChunk::items;

			intKey * kp = ap->index->search(pi);

			if (kp)
			{
				giAChunk * pp = kp->achunk;
				for (i = px; i < giAChunk::items; i++)
					pp->g[i].clear();
			}

			i = pi;
			pi = (ap->len + 1) / giAChunk::items;
			for (; i < pi; i++)
				ap->index->remove(i);
		}

		ap->len = l;
		ap->cache = 0;
	}

	return *this;
}


gist &
gist::arrayensure(unsigned l)
{
	if (typ == GT_ARRAY)
	{
		giArray * ap = (giArray *)intern;
		if (l + 1 < ap->len)
			return *this;
	}

	return array(l + 1);
}


gist &
gist::_arrayindex(long i)
{
	giArray * ap = (giArray *)intern;

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


void
gist::push(const gist & v)
{
	if (typ != GT_ARRAY)
		throw typeError("push expects an array");
	array(((giArray *)intern)->len + 1);
	(*this)[-1] = v;
}


gist &
gist::pop()
{
	if (typ != GT_ARRAY)
		throw typeError("pop expects an array");
	gist * r = new gist((*this)[-1]);
	array(((giArray *)intern)->len - 1);
	return *r;
}
