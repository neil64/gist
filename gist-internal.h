/*
 *	Gist -- Internal data structures.
 */

#ifndef __GIST_INTERNAL_H__
#define __GIST_INTERNAL_H__

namespace gist
{

/**********************************************************************/

class giStr : public gist::gist_internal
{
	// cord like structures, right here.
};

class giStr32 : public giStr
{
	giStr *		str;
	unsigned long	skip;
	unsigned long	cnt;
};


/**********************************************************************/

} // namespace

#endif // __GIST_INTERNAL_H__
