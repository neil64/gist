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
	 *	`hasNull' is set if we know that there is a '\0' after
	 *	the last byte of the string.  `flag' covers all of the
	 *	other flags, just for convenience.
	 */
	union
	{
		struct {
			bool	hasNull;
		};
		int	flags;
	};

	static giStore * alloc(unsigned sz);
	static void	mkKey(char *, int idx);
	static int	gtKey(const char *);
};

/******************************/

struct gistInternal
{
	void *		operator new(unsigned);
	void		operator delete(void *) {}
	static void *	alloc(unsigned);
	static void	free(void *);
};


/*
 *	`Index' is a skip list that contains nodes of the string.  If set,
 *	the string is stored as many pieces, indexed by the offset of
 *	the first character in each piece.  Offsets start at zero, but
 *	can go negative if data is prepended.  `min' is the offset of the
 *	first character (usually zero, but can be negative);  `max' is the
 *	offset of the character position just after the last character.
 *	"max - min" is the length of the string.
 *
 *	If `index' is NIL, the string contains a single piece referenced by
 *	`str'.
 *
 *	Strings use the `unique' value in the gist object that refers to
 *	this giStr.  If `unique' is true then we know that this giStr is not
 *	referenced by any other gist object.  If such a string is modified,
 *	the internal implementation may choose to modify the string in place.
 *	One benefit of this is that appending single characters to a string
 *	is not an exponential operation as it is in some languages with
 *	string types.
 */
struct giStr : gistInternal
{
	giIndex *	index;
	union {
		giStore *	str;
		struct {
			int min, max;
		};
	};

	void		mkTmp(giStore &, const char *);
	static void	concat(gist &, const gist *);
	void		flatten();
	long		toInt(int sign);
	double		toFloat();
	const char *	piece(int & idx, int & len);
	int		cmp(giStr *);
};

/**********************************************************************/

#endif // __GIST_INTERNAL_H__
