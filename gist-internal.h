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
	void *		data;
	unsigned	size;

	union
	{
		struct {
			char		multiRef;
			char		hasNull;
		};
		int		flags;
	};

	static giStore * alloc(unsigned sz);
	void		promote();
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

	static char *	cast(gist *, int);
	static gist	concat(const gist &, const gist &);

	unsigned	skip;
	giIndex *	index;
	union {
		giStore *	str;
		struct {
			unsigned min, max;
		};
	};
};

/**********************************************************************/

#endif // __GIST_INTERNAL_H__
