/*
 *	Gist -- Internal data structures.
 */

#ifndef __GIST_INTERNAL_H__
#define __GIST_INTERNAL_H__


/**********************************************************************/

struct giBase : public gist::gistInternal
{
		giBase(gist::type_e t) { type = t; }
	void *	operator new(unsigned);
	void	operator delete(void *) {}
};


struct giStr : giBase
{
		giStr() : giBase(gist::GT_STR) {}
		giStr(gist::type_e t) : giBase(t) {}

	static gist	concat(const gist &, const gist &);

	// cord like structures, right here.
};

struct giStr32 : giStr
{
		giStr32() : giStr(gist::GT_STR32) {}

	giStr *		str;
	unsigned long	skip;
	unsigned long	cnt;
};

struct giFloat : giBase
{
		giFloat() : giBase(gist::GT_FLOAT) {}

	double		val;
};


/**********************************************************************/

#endif // __GIST_INTERNAL_H__
