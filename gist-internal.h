/*
 *	Gist -- Internal data structures.
 */

#ifndef __GIST_INTERNAL_H__
#define __GIST_INTERNAL_H__


/**********************************************************************/
/*
 *	Indexing.
 */

class giChunk;				// Forward references
class giSChunk;
class giAChunk;


/*
 *	Key, data and skip list pointers for a node of the integer indexed
 *	skip list.
 */
struct intKey
{
	int		key;
	union {
		giChunk * chunk;
		giSChunk * schunk;
		giAChunk * achunk;
	};
	intKey	*	fwd[0];
};


/*
 *	An integer indexed skip list.
 */
class giIndexInt
{
    public:
	giIndexInt();
	~giIndexInt();

	void *		operator new(unsigned sz, int levels);
	void		operator delete(void *)		{}

    public:
	/*
	 *	Search will locate and return the entry that matches the
	 *	given `key'.
	 */
	intKey *	search(int key);

	/*
	 *	Return the first or the last tuple in the index.
	 */
	intKey *	first();
	intKey *	last();

	/*
	 *	Find the node either just before or just after the record
	 *	matching the given `key'.  If no records remain, NIL is
	 *	returned.
	 */
	intKey *	next(int key);
	intKey *	previous(int key);

	/*
	 *	Insert the given `data' at the given `key' and return true.
	 *	If an entry already exists, false is returned.
	 */
	bool		insert(int key, giChunk * data);

	/*
	 *	Remove the entry matching the given `key', if it exists.
	 *	True is returned if it was deleted, or false if it was not
	 *	found.
	 */
	bool		remove(int key);

    private:
	unsigned char	levels;
	unsigned char	maxLevel;

	/*
	 *	The last entry returned.  The skip list code may use this
	 *	to optimise searches.
	 */
	intKey *	cache;

    public:
	/*
	 *	Minimum and maximum keys used.  Used in both the string code
	 *	and the array code to help with inserts and appends.
	 */
	int		min, max;

	/*
	 *	This skip list implementation sets the maximum number of
	 *	levels at the creation of the skip list, rather than having
	 *	a fixed maximum.  This allows strings to have a maximum of
	 *	eight levels and arrays to have a maximum of sixteen.  With
	 *	a P of 1/2 (hard coded in insert()), a depth of n is ideal
	 *	for a node count of 2^n.  For strings, a maximum of 8 performs
	 *	well for up to 256 chunks (for 64 byte chunks, that's a length
	 *	of 16k).  For arrays, a maximum depth of 16 allows for 64k
	 *	chunks (for 16 entry chunks, that's 1024k entries).  We want
	 *	to reduce the maximum depth to reduce the size of the skip
	 *	list header, since in the case of strings there could be a
	 *	lot of them.
	 */
	enum
	{
		StrLevels = 8,
		ArrayLevels = 16,
		MaxLevel = 16
	};

    private:
	/*
	 *	Skip list head pointers.
	 */
	intKey *	head[0];
};

/******************************/

/*
 *	Key, data and skip list pointers for a node of the gist indexed
 *	skip list.
 */
struct gistKey
{
	gist		key;
	gist		val;
	gistKey	*	fwd[0];
};


class giIndexGist
{
    public:
	giIndexGist();
	~giIndexGist();

	void *		operator new(unsigned sz);
	void		operator delete(void *)		{}

    public:
	/*
	 *	Search will locate and return the entry that matches the
	 *	given `key'.  If no matching entry is found and `make' is
	 *	true, make a new entry with NIL as the value, and return it.
	 */
	gistKey *	search(const gist & key, bool make = false);

	/*
	 *	Return the first or the last tuple in the index.
	 */
	gistKey *	first();
	gistKey *	last();

	/*
	 *	Find the tuple either just before or just after the record
	 *	matching the given `key'.  If no records remain, NIL is
	 *	returned.
	 */
	gistKey *	next(const gist & key);
	gistKey *	previous(const gist & key);

	/*
	 *	Insert the given `data' at the given `key' and return true.
	 *	If an entry already exists, false is returned.
	 */
	bool		insert(const gist & key, const gist & data);

	/*
	 *	Remove the entry matching the given `key', if it exists.
	 *	True is returned if it was deleted, or false if it was not
	 *	found.
	 */
	bool		remove(const gist & key);

    private:
	enum
	{
		MaxLevel = 16,
	};

	/*
	 *	The skip list head pointers.
	 */
	gistKey *	head[MaxLevel];
	unsigned	levels;

	/*
	 *	The last entry returned.  The skip list code may use this
	 *	to optimise searches.
	 */
	gistKey *	cache;
};

/******************************/

/*
 *	`gistInternal' is a class that is mentioned in the main `gist'
 *	class as a generic name for internal data.  We don't store anything
 *	there;	we just use it as a handy place to put a few methods.
 *	Note that some callers of alloc expect the memory to be zeroed
 *	(the Boehm GC allocator does this).
 */
struct gistInternal
{
	/*
	 *	GC allocator stuff.
	 */
	void *		operator new(unsigned);
	void		operator delete(void *) {}
	static void *	alloc(unsigned);
	static void	free(void *);

	/*
	 *	OS dependent stuff.
	 */
	static bool	isReadOnlyData(const void * ptr);
	static void	readOnlyDataTestInit();
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
		giSChunk *	chunk;
		bool		hasNull;
	};

	/********/

	void		makeMulti(unsigned len);
	long		toInt(bool sign, unsigned base);
	double		toFloat();
};

/******************************/

struct giArray : gistInternal
{
	unsigned	skip;
	unsigned	len;

	giIndexInt *	index;
	giAChunk *	cache;
	unsigned	ci;
};

/******************************/

struct giTable : gistInternal
{
	giIndexGist	index;
};

/******************************/

/*
 *	A base class for chunks, to help the skip list code.
 */
struct giChunk
{
};

/*
 *	A string chunk.  Used to store pieces of strings in the skip list.
 *	`data' is the string data, `len' is the string length;  `data0'
 *	points to the start of the string storage chunk.  It is not used
 *	other than to create a reference for the garbage collector, since
 *	we promise the GC that we would not have internal references.
 */
struct giSChunk : giChunk
{
	char *		data;
	unsigned	len;
	char *		data0;
};

/*
 *	An array chunk.  Storage for a small number of array elements.
 */
struct giAChunk : giChunk
{
	enum {
		items = 16
	};
	gist		g[items];
};


/**********************************************************************/

#endif // __GIST_INTERNAL_H__
