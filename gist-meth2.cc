/*
 *	Gist -- Methods part 2.
 *
 *	Contains:	substr, strtrim
 *		(all non-core string methods)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/


gist
gist::substr(int s, unsigned c) const
{
	if (!isStr())
		throw typeError("substr() expects a string");

	gist r;

	if (s < -(int)skip)
		s = -(int)skip;
	if (s < (int)cnt && c > 0)
	{
		r.typ = GT_STR;
		r.unique = false;
		((gist *)this)->unique = false;
		r.ptr = ptr;

		r.skip = skip + s;
		r.cnt = cnt - s;
		if (r.cnt > c)
			r.cnt = c;
	}

	return r;
}


void
gist::strtrim(int s, unsigned c)
{
	if (!isStr())
		throw typeError("substr() expects a string");

	if (s < -(int)skip)
		s = -(int)skip;
	if (s < (int)cnt && c > 0)
	{
		skip += s;
		cnt -= s;
		if (cnt > c)
			cnt = c;
	}
	else
		_strzero();
}
