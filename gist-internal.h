/*
 *	Gist -- Internal data structures.
 */

#ifndef __GIST_INTERNAL_H__
#define __GIST_INTERNAL_H__


/**********************************************************************/
/*
 *	Indexing.
 */


struct intKey
{
	int		key;
	giStr *		str;
	intKey	*	fwd[0];
};


class giIndexInt
{
    public:
	giIndexInt();
	~giIndexInt();

    public:
	/*
	 *	Search will locate and return the entry that matches the
	 *	given `key'.
	 */
	intKey *	search(int);

	/*
	 *	Return the first or the last tuple in the index.
	 */
	intKey *	first();
	intKey *	last();

	/*
	 *	Find the tuple either just before or just after the
	 *	record matching the given `key'.  The key of the matched
	 *	record replaces `key' and `len'.  If no records remain,
	 *	NIL is returned and `key' is set to NIL and `len' to zero.
	 *	If NIL is passed as the key, NIL will be returned.
	 */
	intKey *	next(int);
	intKey *	previous(int);

	/*
	 *	Insert the given `ref' at the given `key' and return true.
	 *	If an entry already exists, false is returned.
	 */
	bool		insert(int, giStr *);

	/*
	 *	Remove the entry matching the given `key', if it exists.
	 *	True is returned if it was deleted, or false if it was not
	 *	found.
	 */
	bool		remove(int);

    private:
	enum
	{
		MaxLevel = 16,
		P = (((unsigned)-1) / 2)
	};

	intKey *	head[MaxLevel];
	unsigned	levels;

	intKey *	cache;
	int		min, max;
};

/******************************/

struct strKey
{
	const char *	key;
	unsigned	klen;
	gist		val;
	strKey	*	fwd[0];
};


class giIndexStr
{
    public:
	giIndexStr();
	~giIndexStr();

    public:
	/*
	 *	Search will locate and return the entry that matches the
	 *	given `key'.
	 */
	strKey *	search(int);

	/*
	 *	Return the first or the last tuple in the index.
	 */
	strKey *	first();
	strKey *	last();

	/*
	 *	Find the tuple either just before or just after the
	 *	record matching the given `key'.  The key of the matched
	 *	record replaces `key' and `len'.  If no records remain,
	 *	NIL is returned and `key' is set to NIL and `len' to zero.
	 *	If NIL is passed as the key, NIL will be returned.
	 */
	strKey *	next(int);
	strKey *	previous(int);

	/*
	 *	Insert the given `ref' at the given `key' and return true.
	 *	If an entry already exists, false is returned.
	 */
	bool		insert(strKey *);

	/*
	 *	Remove the entry matching the given `key', if it exists.
	 *	True is returned if it was deleted, or false if it was not
	 *	found.
	 */
	bool		remove(int);

    private:
	enum
	{
		MaxLevel = 16,
		P = (((unsigned)-1) / 2)
	};

	strKey *	head[MaxLevel];
	unsigned	levels;

	strKey *	cache;
	int		min, max;
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
	giIndexInt *	index;
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

struct giStr0 : gistInternal
{
	char *		data;
	unsigned	size;
	bool		big;
	bool		hasNull;
};

struct giStr1 : giStr
{
	giIndexInt *	index;
	// int		min, max;
};

/**********************************************************************/

#endif // __GIST_INTERNAL_H__
