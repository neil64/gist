/*
 *	Gist -- Internal data structures.
 */

#ifndef __GIST_INTERNAL_H__
#define __GIST_INTERNAL_H__


/**********************************************************************/
/*
 *	Skip lists.
 */

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
	void *		search(const char * key, unsigned len);

	/*
	 *	Return the first or the last tuple in the index.
	 */
	void *		first(const char *& key, unsigned & len);
	void *		last(const char *& key, unsigned & len);

	/*
	 *	Find the tuple either just before or just after the
	 *	record matching the given `key'.  The key of the matched
	 *	record replaces `key' and `len'.  If no records remain,
	 *	NIL is returned and `key' is set to NIL and `len' to zero.
	 *	If NIL is passed as the key, NIL will be returned.
	 */
	void *		next(const char *& key, unsigned & len);
	void *		previous(const char *& key, unsigned & len);

	/*
	 *	Insert the given `ref' at the given `key' and return true.
	 *	If an entry already exists, false is returned.
	 */
	int		insert(const char * key, unsigned len, void * ref);

	/*
	 *	Remove the entry matching the given `key', if it exists.
	 *	True is returned if it was deleted, or false if it was not
	 *	found.
	 */
	int		remove(const char * key, unsigned len);

    private:
	struct sk_t
	{
		char *		key;
		unsigned	len;
		void *		ref;
		sk_t *		fwd[0];

		int		cmp(const char *, unsigned);
	};

	enum
	{
		MaxLevel = 16,
		P = (((unsigned)-1) / 2)
	};

	sk_t *		head[MaxLevel];
	unsigned	levels;
};

/******************************/

struct giStore
{
	/*
	 *	The string buffer and its size.  If the size is zero, then
	 *	the pointer must be ignored (could be zero, or just bogus).
	 */
	void *		data;
	unsigned	size;

	/*
	 *	`multiRef' is set if we are not sure that there is only a
	 *	single reference.  `hasNull' is set if there is a '\0' after
	 *	the last byte of the string.  `flag' covers all of the
	 *	other flags, just for convenience.
	 */
	union
	{
		struct {
			char		multiRef;
			char		hasNull;
		};
		int		flags;
	};

	static giStore * alloc(unsigned sz);
};

/******************************/

struct giBase : public gist::gistInternal
{
			giBase(gist::type_e t) { type = t; }

	void *		operator new(unsigned);
	void		operator delete(void *) {}
	static void *	alloc(unsigned);
	static void	free(void *);
};


struct giStr : giBase
{
		giStr() : giBase(gist::GT_STR) {}
		giStr(gist::type_e t) : giBase(t) {}

	unsigned	skip;
	giIndex *	index;
	union {
		giStore *	str;
		struct {
			unsigned min, max;
		};
	};

	static gist	concat(const gist &, const gist &);
	void		flatten();
	long		toInt(int sign);
	double		toFloat();
};

/**********************************************************************/

#endif // __GIST_INTERNAL_H__
