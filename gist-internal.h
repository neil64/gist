/*
 *	Gist -- Internal data structures.
 */

#ifndef __GIST_INTERNAL_H__
#define __GIST_INTERNAL_H__


/**********************************************************************/
/*
 *	Indexing.
 */

class giChunk;				// Forward reference


struct intKey
{
	int		key;
	giChunk *	data;
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
	bool		insert(int, giChunk *);

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

    public:
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

struct gistInternal
{
	void *		operator new(unsigned);
	void		operator delete(void *) {}
	static void *	alloc(unsigned);
	static void	free(void *);
};


/*
 *	A string can be in two major states, dubbed "single" and "multi".
 *	Single is a string who's data is contained in a single chunk of
 *	memory with all characters consecutive.	 Multi is a collection of
 *	memory chunks that together form the string.  If `index' is set,
 *	the string is mutli, and `index' points to an indexing structure
 *	containing the string pieces.  If `index' is NIL, the string is
 *	single.	 `data' and `size' refer to a string chunk;  if single,
 *	it is the string data, and if multi it is the last string chunk
 *	(caveat unique).  `hasNull' is set if there is known to be a '\0'
 *	character after the last character of the string; the '\0' is not
 *	included in the string length but it is included in the chunk size.
 *
 *	If `unique' is set in the `gist' that refers to this giStr, some
 *	special conditions exist.  If the string is single, the whole
 *	string is known not to be referenced by any other `gist';  this
 *	allows the string to be modified in place.  If the string is multi,
 *	then the index structure is known not to be referenced elsewhere,
 *	but the string chunks themselves may be referenced elsewhere.
 *
 *	If `unique' is set on a multi string, and if `data' is non-NIL, then
 *	`data', `size' and `len' refer to a chunk that can be modified --
 *	known not to be referenced elsewhere; `data' refers to the first
 *	byte of storage and the first character of the string chunk, "data +
 *	len - 1" point to the last valid character, and "data + size - 1"
 *	point to the last byte of storage.
 *
 *	For a single string, only `data' and `size' are valid, and `skip'
 *	and `cnt' are taken from the gist;  `data' points to the first
 *	byte of storage, "data + skip" points to the first real character,
 *	"data + skip + cnt - 1" points to the last real character, "data +
 *	size - 1" points to the last byte of storage.
 *
 *	If `hasNull' is set, the byte at "data + skip + cnt" equals '\0',
 *	and `size' is large enough to contain the '\0'.	 `hasNull' would
 *	be difficult to take advantage of for multi strings, so it is only
 *	available for single strings.
 *
 *	A multi string stores pieces of the string indexed by the offset in
 *	the string of the first character of the chunk.	 `index->min' is
 *	the offset of the first character of the string, and `index->max'
 *	is the offset of the character just past the last character of
 *	the string.  Initially, `min' is zero, but it may go negative if
 *	data is inserted at the front of the string.
 */
struct giStr : gistInternal
{
	giIndexInt *	index;
	char *		data;
	unsigned	size;
	union {
		unsigned	len;
		bool		hasNull;
	};

	/********/

	void		makeMulti(unsigned len);
	long		toInt(bool sign, unsigned base);
	double		toFloat();
};

/*
 *	A string chunk.  Used to store pieces of strings in the skip list.
 *	`data' is the string data, `len' is the string length;  `data0'
 *	points to the start of the string storage chunk.  It is not used
 *	other than to create a reference for the garbage collector, since
 *	we promise the GC that we would not have internal references.
 */
struct giChunk
{
	char *		data;
	unsigned	len;
	char *		data0;
};

/**********************************************************************/

#endif // __GIST_INTERNAL_H__
