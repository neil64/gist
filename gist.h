/*
 *	Gist -- Scripting language like data structures.
 *
 *	Gist provides C++ with a loosly typed data object similar to
 *	that found in languages like Python, JavaScript, etc.  A gist
 *	object takes care of its own storage management, using the Boehm
 *	conservative garbage collector.  It should be possible to use
 *	gist objects with careless abandon with fear of clobbering
 *	memory, so long as you don't use C++ to look inside.  With gist
 *	you should be able to code almost as easily as you can in Python,
 *	etc, but with code that is 10 - 100 times faster.
 *
 *	The data types that a gist object can have:
 *
 *		Int	32-bit integer
 *		Str	A character string
 *		Float	A 64-bit IEEE-748 floating point number
 *		Array	Simply array, single index 0 .. n
 *		Table	Associative array
 *		Long	Arbitrary length integer
 *		Real	Arbitrary length floating point number
 *		Code	Executable pseudo code (compiled script)
 *
 *	The last `Code' type is not implemented yet, and may never be.
 *	If it is, it will likely be compiled JavaScript code.  The two
 *	variable length types are incomplete.
 */

#ifndef __GIST_H__
#define __GIST_H__


/**********************************************************************/

class gist
{
    public:
	/********************************/
	/*
	 *	Constructors.
	 *
	 *	Note that there is no destructor.  Objects are just
	 *	abandoned and left to the garbage collector to find.
	 *	There is no cleaning up to do with a gist object.
	 */
	gist()			{ typ = GT_NIL; }
	gist(int v)		{ typ = GT_INT; val = v; }
	gist(unsigned v)	{ typ = GT_INT; val = v; }
	gist(long v)		{ typ = GT_INT; val = v; }
	gist(unsigned long v)	{ typ = GT_INT; val = v; }
	gist(float v)		{ typ = GT_FLOAT; dval = v; }
	gist(double v)		{ typ = GT_FLOAT; dval = v; }
	gist(const gist & g)	{ ((gist &)g).unique = 0; all = g.all; }
	gist(const gist * g)	{ ((gist *)g)->unique = 0; all = g->all; }
	gist(const char * s)	{ set(s); }
	gist(const char * s, int len)	{ set(s, len); }

	/*
	 *	These should make Long's or Real's, but for now ...
	 */
	gist(long long v)		{ typ = GT_INT; val = v; }
	gist(unsigned long long v)	{ typ = GT_INT; val = v; }
	gist(long double v)		{ typ = GT_FLOAT; dval = v; }

	/********************************/
	/*
	 *	Assignment.
	 */
	gist &	operator =(int v)		{ typ = GT_INT; val = v;
							return *this; }
	gist &	operator =(unsigned v)		{ typ = GT_INT; val = v;
							return *this; }
	gist &	operator =(long v)		{ typ = GT_INT; val = v;
							return *this; }
	gist &	operator =(unsigned long v)	{ typ = GT_INT; val = v;
							return *this; }
	gist &	operator =(float v)		{ typ = GT_FLOAT; dval = v;
							return *this; }
	gist &	operator =(double v)		{ typ = GT_FLOAT; dval = v;
							return *this; }
	gist &	operator =(const gist & g)	{ ((gist &)g).unique = 0;
						  all = g.all;
						  return *this; }
	gist &	operator =(const gist * g)	{ ((gist *)g)->unique = 0;
						  all = g->all;
						  return *this; }
	gist &	operator =(const char * s)	{ set(s);
						  return *this; }
	gist &	operator =(long long v)		{ typ = GT_INT; val = v;
						  return *this; }
	gist &	operator =(unsigned long long v)
						{ typ = GT_INT; val = v;
						  return *this; }
	gist &	operator =(long double v)
						{ typ = GT_FLOAT; dval = v;
						  return *this; }

	void		set()			{ typ = GT_NIL; }
	void		set(int v)		{ typ = GT_INT; val = v; }
	void		set(unsigned v)		{ typ = GT_INT; val = v; }
	void		set(long v)		{ typ = GT_INT; val = v; }
	void		set(unsigned long v)	{ typ = GT_INT; val = v; }
	void		set(float v)		{ typ = GT_FLOAT; dval = v; }
	void		set(double v)		{ typ = GT_FLOAT; dval = v; }
	void		set(const gist & g)	{ ((gist &)g).unique = 0;
						  all = g.all; }
	void		set(const gist * g)	{ ((gist *)g)->unique = 0;
						  all = g->all; }
	void		set(const char *);
	void		set(const char *, int);
	void		set(long long v)	{ typ = GT_INT; val = v; }
	void		set(unsigned long long v) { typ = GT_INT; val = v; }
	void		set(long double v)	{ typ = GT_FLOAT; dval = v; }

	void		clear()			{ typ = GT_NIL; }
	void		strclear();

	/********************************/
	/*
	 *	Shallow copying.
	 */

	void		copy(const char *, int = -1);
	void		copy(const gist &);
	void		copy(const gist *);

	/********************************/
	/*
	 *	Memory allocation.
	 */
	void *		operator new(unsigned);
	void		operator delete(void * p) {}

	/********************************/
	/*
	 *	Subscripting.  It always returns a reference to a gist
	 *	object, which will usually be an element of a gist array.
	 *	The object can be modified in place.
	 */
	gist &		operator [](int);
	gist &		operator [](unsigned);
	gist &		operator [](long);
	gist &		operator [](unsigned long);
	gist &		operator [](float);
	gist &		operator [](double);
	gist &		operator [](long long);
	gist &		operator [](unsigned long long);
	gist &		operator [](const char *);
	gist &		operator [](char *);
	gist &		operator [](const gist &);

	const gist &	operator [](int) const;
	const gist &	operator [](unsigned) const;
	const gist &	operator [](long) const;
	const gist &	operator [](unsigned long) const;
	const gist &	operator [](float) const;
	const gist &	operator [](double) const;
	const gist &	operator [](long long) const;
	const gist &	operator [](unsigned long long) const;
	const gist &	operator [](const char *) const;
	const gist &	operator [](char *) const;
	const gist &	operator [](const gist &) const;

	/********************************/
	/*
	 *	Type conversion.
	 */

	/*
	 *	Try to convert a type to a string.  This is only supported
	 *	by some types, such as numbers.
	 */
	gist		toString() const;

	/*
	 *	Try to convert a type to an integer or double.	This is
	 *	only supported by the numeric types and the string type.
	 *	Overflow and value errors can result.
	 *
	 *	If converting a string to an integer with `toInt()', the
	 *	the conversion is like the C library function `strtol',
	 *	allowing conversions using base 2 to 36, and the default
	 *	"base 0" which accepts the prefix "0x" for hexadecimal and
	 *	"0" for octal.	When casting a string to an integer, only
	 *	base 10 is accepted.  If the conversion causes an overflow,
	 *	`toInt()' and `toFloat()' both throw and `overflowError'
	 *	exception.  Overflow from one of the casts returns an
	 *	undefined result.
	 */
	long		toInt(unsigned base = 0) const;
			operator int() const;
			operator unsigned() const;
			operator long() const;
			operator unsigned long() const;
			operator long long() const;
			operator unsigned long long() const;

	double		toFloat() const;
			operator float() const		{ return toFloat(); }
			operator double() const		{ return toFloat(); }

			operator bool() const;

	/*
	 *	Return a pointer to the string storage of the gist object.
	 *	If necessary, the string storage is manipulated to make
	 *	it a C++ string, by ensuring the string is in contiguous
	 *	storage and that it is null terminated.
	 *
	 *	With "const char *", a pointer to existing storage is
	 *	returned.  Repeated casts will return the same pointer.
	 *	The storage may be referenced by other objects.	 Modifying
	 *	the string data though this pointer can have unpredictable
	 *	and undesirable results.
	 *
	 *	With "char *", the string storage is made to be unique before
	 *	returning a pointer to it.  Repeated casts will return
	 *	a pointer to different storage, copying the string data
	 *	if necessary.  So long as the original gist string object is
	 *	not modified or used in any way (including another cast), the
	 *	string data referred to by the pointer can be modified and
	 *	the changes will be reflected in the original gist string.
	 *	As soon as the original object is used or changed, the
	 *	pointer should be considered to refer to read-only data;
	 *	changing the string data though the pointer after the object
	 *	has been used or modified can have unpredictable results.
	 */
			operator const char *() const
				{ return (const char *)_strcast(false); }
			operator char *() const
				{ return _strcast(true); }
	const char *	CCS() const
				{ return (const char *)_strcast(false); }

	/********************************/
	/*
	 *	Operators.
	 */
#define GIST_OPS1(r, o)							\
	friend r	operator o(const gist &, const gist &);		\
	friend r	operator o(const gist &, int);			\
	friend r	operator o(const gist &, unsigned);		\
	friend r	operator o(const gist &, long);			\
	friend r	operator o(const gist &, unsigned long);	\
	friend r	operator o(const gist &, long long);		\
	friend r	operator o(const gist &, unsigned long long);	\
	friend r	operator o(int, const gist &);			\
	friend r	operator o(unsigned, const gist &);		\
	friend r	operator o(long, const gist &);			\
	friend r	operator o(unsigned long, const gist &);	\
	friend r	operator o(long long, const gist &);		\
	friend r	operator o(unsigned long long, const gist &);

#define GIST_OPS2(r, o)							\
	GIST_OPS1(r, o)							\
	friend r	operator o(const gist &, float);		\
	friend r	operator o(const gist &, double);		\
	friend r	operator o(float, const gist &);		\
	friend r	operator o(double, const gist &);

#define GIST_OPS2a(r, o)						\
	GIST_OPS2(r, o)							\
	friend r	operator o(const gist &, const char *);		\
	friend r	operator o(const char *, const gist &);		\
	friend r	operator o(const gist &, char *);		\
	friend r	operator o(char *, const gist &);

#define GIST_OPS2b(r, o)						\
	GIST_OPS2a(r, o)						\
	friend r	operator o(const gist &, const void *);		\
	friend r	operator o(const void *, const gist &);

#define GIST_OPS3(o)							\
	gist &		operator o(const gist &);			\
	gist &		operator o(int);				\
	gist &		operator o(unsigned);				\
	gist &		operator o(long);				\
	gist &		operator o(unsigned long);			\
	gist &		operator o(long long);				\
	gist &		operator o(unsigned long long);

#define GIST_OPS4(o)							\
	GIST_OPS3(o)							\
	gist &		operator o(float);				\
	gist &		operator o(double);

	GIST_OPS2a(gist, +)
	GIST_OPS2(gist, -)
	GIST_OPS2(gist, *)
	GIST_OPS2(gist, /)
	GIST_OPS1(gist, %)
	GIST_OPS2b(bool, ==)
	GIST_OPS2b(bool, !=)
	GIST_OPS2a(bool, <)
	GIST_OPS2a(bool, >)
	GIST_OPS2a(bool, <=)
	GIST_OPS2a(bool, >=)

	GIST_OPS1(gist, ^)
	GIST_OPS1(gist, &)
	GIST_OPS1(gist, |)
	GIST_OPS1(gist, <<)
	GIST_OPS1(gist, >>)
	GIST_OPS1(gist, &&)
	GIST_OPS1(gist, ||)

	GIST_OPS4(+=)
	gist &		operator +=(const char *);
	GIST_OPS4(-=)
	GIST_OPS4(*=)
	GIST_OPS4(/=)
	GIST_OPS4(%=)
	GIST_OPS3(^=)
	GIST_OPS3(&=)
	GIST_OPS3(|=)
	GIST_OPS3(<<=)
	GIST_OPS3(>>=)

	gist		operator -() const;
	gist		operator ~() const;
	gist		operator !() const;

	gist &		operator ++();
	gist		operator ++(int);
	gist &		operator --();
	gist		operator --(int);

	// gist		operator &();
	// gist		operator ,(const gist &);
#undef GIST_OPS1
#undef GIST_OPS2
#undef GIST_OPS3
#undef GIST_OPS4

	/********************************/
	/*
	 *	Typing.
	 */
	enum type_e
	{
		GT_NIL = 0,
		GT_STR,
		GT_ARRAY,
		GT_TABLE,
		GT_PTR,
		GT_CODE,
		GT_FILE,
		GT_REGEX,
		GT_INT,
		GT_FLOAT,
		GT_LONG,
		GT_REAL
	};
	type_e		type() const		{ return typ; }

	bool		isNil() const		{ return typ == GT_NIL; }
	bool		isInt() const		{ return typ == GT_INT; }
	bool		isFloat() const		{ return typ == GT_FLOAT; }
	bool		isStr() const		{ return typ == GT_STR; }
	bool		isNumber() const	{ return typ >= GT_INT; }
	bool		isArray() const		{ return typ == GT_ARRAY; }
	bool		isTable() const		{ return typ == GT_TABLE; }
	bool		isPtr() const		{ return typ == GT_PTR; }
	bool		isFile() const		{ return typ == GT_FILE; }
	bool		isRegex() const		{ return typ == GT_REGEX; }

	/********************************/
	/*
	 *	Exceptions.
	 */
	struct error
	{
		const char * typ;
		const char * msg;
		error() : typ(0), msg(0) {}
		error(const char * t, const char * m) : typ(t), msg(m) {}
	};
#define GIST_ERROR(t)					\
	struct t : error				\
	{						\
		t() {}					\
		t(const char * m) : error(#t, m) {}	\
	};

	/*
	 *	A value error is thrown typically when a value cannot be
	 *	coerced into a useful form, such as trying to convert a
	 *	string to a number where the string does not contain digits.
	 */
	GIST_ERROR(valueError)

	/*
	 *	A type error is thrown when the operands of an operation
	 *	are not suitable for the operation.  (ie.  trying to add
	 *	an array to an integer.)
	 */
	GIST_ERROR(typeError)

	/*
	 *	When using a gist array, and index out of bounds or an
	 *	illegal index type gives an `indexError'.
	 */
	GIST_ERROR(indexError)

	/*
	 *	If a conversion causes an overflow, an `overflowError'
	 *	is thrown.
	 */
	GIST_ERROR(overflowError)

	/*
	 *	If an attempt is made to extract a pointer with `ptrget()'
	 *	on a non-pointer object, a `ptrError' is thrown.
	 */
	GIST_ERROR(ptrError)

	/*
	 *	If an attempt is made to open a file that does not exist,
	 *	a `noSuchFileError' is thrown.
	 */
	GIST_ERROR(noSuchFileError)

	/*
	 *	An attempt to access a file that is not open, does not
	 *	permit access, or some other illegal state, a `fileError'
	 *	is thrown.
	 */
	GIST_ERROR(fileError);

	/*
	 *	If an attempt is made to read a file that has reached
	 *	end-of-file, an `eofError' is thrown.  This is not
	 *	necessarily an error, depending on your point of view.
	 */
	GIST_ERROR(eofError);

	/*
	 *	If an attempt is made to open or perform I/O on a file
	 *	that causes an error, a `ioError' is thrown, and the
	 *	system `errno' will be set appropriately.
	 */
	GIST_ERROR(ioError)

	/*
	 *	A part of Gist that is not implemented yet.
	 */
	GIST_ERROR(notYetError)

	/*
	 *	Some sort of internal error (please file a bug report).
	 */
	GIST_ERROR(internalError)

#undef GIST_ERROR

	/**************************************************************/
	/**************************************************************/
	/*
	 *	Gist per object storage.
	 *
	 *	On most architectures, a gist object will fit within
	 *	16 bytes.
	 *
	 *	[ Originally, this structure was 8 bytes.  For various
	 *	  implementation reasons, it was not possible to keep it
	 *	  that small.  It survived at 12 bytes for a short while
	 *	  until more problems arose and it was realized that most
	 *	  allocators rounded up to 16 in any case.  In the case
	 *	  of strings (which is expected to be the most common gist
	 *	  data type), it is a win, since the data here used to be
	 *	  elsewhere; with the current arrangement, some of the code
	 *	  is simplified, and faster. ]
	 *
	 *	[ Rant:  In my opinion, C++ is broken when it comes to
	 *	  nested classes and unions.  What I want is to use anonymous
	 *	  structs and unions to effect the layout of a gist object,
	 *	  but I want those members to be private to gist.  The C++
	 *	  spec says no private members of an anonymous union,
	 *	  and nested class obey the same access rules as regular
	 *	  classes.  So, a nested class called, say, `x_t' cannot be
	 *	  accessed by the world, but its members can.  Who's bright
	 *	  idea was that?  I'm sure I could make it right by naming
	 *	  everything, but then I'd have to change several thousand
	 *	  lines of internal gist code, or use macros.  So, the data
	 *	  members of the gist class are not private.  User's, please
	 *	  pretend that they are private. ]
	 */
    private:
	union
	{
	    struct
	    {
		union
		{
		    /*
		     *	The value of this object if the object type is
		     *	GT_INT.
		     */
		    long	val;		// Integer type

		    /*
		     *	The value of this object if the object type is
		     *	GT_FLOAT.
		     */
		    double	dval;		// Float type

		    /*
		     *	String and array bounds.  Usually `skip' is zero and
		     *	`cnt' equal to the object size.	 These values are
		     *	adjusted to provide a subset of the underlying value,
		     *	such as that returned by the `substr' function.
		     */
		    struct
		    {
			unsigned    cnt;
			unsigned    skip;
		    };

		    /*
		     *	The value of this object if the object type is
		     *	GT_PTR.
		     */
		    void *	ptr;		// Pointer type
		};

		/*
		 *  A pointer to internal data, if required.  For string
		 *  and array types, this is the string / array storage.
		 */
		struct gistInternal * intern;	// Internal data

		/*
		 *  Object type.  Set to one of the values from the
		 *  enum above.  The bit field syntax is used to ensure that
		 *  the type only takes a byte of storage.  The size will
		 *  be rounded up on architectures that have other than
		 *  8-bits per byte.
		 */
		type_e	typ : 8;		// Object type

		/*
		 *  True if the value of this object is known not to be
		 *  referenced by another object;  this is of most use
		 *  for string types, to allow the string to be modified
		 *  directly rather than having to copy it first; it is
		 *  not initialized on objects that don't use it, such as
		 *  integer and floating types.
		 */
		bool	unique;			// Set if no other refs to this

		/*
		 *	2 bytes reserved for future use.  However, see
		 *	gist::opertor new() in gist.cc before using this.
		 */
		// short	res0;
	    };

	    /*
	     *	A structure to cover the whole object, to make it easy to
	     *	copy.  Copying a gist object using something like "a.all =
	     *	b.all" gives compilers a chance to generate more efficient
	     *	code for the copy.  We include a copy of most of the above
	     *	declarations so that we can get gdb to behave.
	     */
	    struct {
		union {
		    long val;
		    double dval;
		    struct { unsigned cnt; unsigned skip; } s;
		    void * ptr;
		};
		struct gistInternal * intern;
		type_e typ:8;
		bool unique;
	    } all;
	};

	/*
	 *	Private methods.
	 */
	char *		_strcast(bool rw) const;
	unsigned	_strpiece(int & index, const char *& ptr) const;
	int		_stridx(long) const;
	void		_strflatten() const;
	// void		_strzero();
	void		_strsplit(const gist &);
	void		_strsplit(const gist &, const char *, int);
	long		_toInt(bool sign, unsigned base) const;

	gist &		_arrayindex(long);
	gist &		_tableindex(long, bool);
	gist &		_tableindex(double, bool);
	gist &		_tableindex(const char *, bool);
	gist &		_tableindex(const gist &, bool);

	/*
	 *	Some internal structures that we call friends.
	 */
	friend class giStr;
	friend class giRegex;

	/**************************************************************/
	/**************************************************************/
	/*
	 *	Public gist member functions.
	 */
    public:

	/*
	 *	Strings.
	 */
	unsigned	strlen() const
					{ return typ == GT_STR ? cnt : 0; }
	friend unsigned	strlen(const gist & g)
					{ return g.typ == GT_STR ? g.cnt : 0; }

	int		strcmp(const char *) const;
	int		strcmp(const gist &) const;
	friend int	strcmp(const gist &, const char *);
	friend int	strcmp(const char *, const gist &);
	friend int	strcmp(const gist &, const gist &);

	int		strncmp(const char *, int = -1) const;
	int		strncmp(const gist &, int = -1) const;
	friend int	strncmp(const gist & l, const char * r, int z = -1)
				{ return l.strncmp(r, z); }
	friend int	strncmp(const char *, const gist &, int = -1);
	friend int	strncmp(const gist & l, const gist & r, int z = -1)
				{ return l.strncmp(r, z); }

	int		strcasecmp(const char *) const;
	int		strcasecmp(const gist &) const;
	friend int	strcasecmp(const gist &, const char *);
	friend int	strcasecmp(const char *, const gist &);
	friend int	strcasecmp(const gist &, const gist &);

	int		strncasecmp(const char *, int = -1) const;
	int		strncasecmp(const gist &, int = -1) const;
	friend int	strncasecmp(const gist &, const char *, int = -1);
	friend int	strncasecmp(const char *, const gist &, int = -1);
	friend int	strncasecmp(const gist &, const gist &, int = -1);

	unsigned	strpiece(int & index, const char *& ptr) const;
	friend unsigned	strpiece(const gist &, int & index, const char *& ptr);

	int		stridx(long) const;
	friend int	stridx(const gist &, long);

	char *		strbuf(unsigned size);
	friend char *	strbuf(gist & g, unsigned size)
				{ return g.strbuf(size); }

	void		strcat(int);
	void		strcat(const char *, int count = -1);
	void		strcopycat(const char *, int count = -1);
	void		strcat(const gist &);
	friend void	strcat(gist &, int);
	friend void	strcat(gist &, const char *, int count = -1);
	friend void	strcopycat(gist &, const char *, int count = -1);
	friend void	strcat(gist &, const gist &);
	friend void	strcat(char *, const gist &);
	friend void	strncat(char *, const gist &, unsigned count);

	void		strins(int);
	void		strins(const char *, int count = -1);
	void		strcopyins(const char *, int count = -1);
	void		strins(const gist &);

	friend unsigned	strcpy(char * dest, const gist & src,
				unsigned start = 0, unsigned count = (~0U>>1));
	friend unsigned	strncpy(char * dest, const gist & src, unsigned count)
				{ return strcpy(dest, src, 0, count); }

	gist		substr(int start, unsigned count = (~0U>>1)) const;
	friend gist	substr(const gist & src, int start,
						unsigned count = (~0U>>1))
				{ return src.substr(start, count); }
	void		strtrim(int start, unsigned count = (~0U>>1));
	friend void	strtrim(gist & str, int start,
						unsigned count = (~0U>>1))
				{ return str.strtrim(start, count); }

	bool		isalnum() const;
	bool		isalpha() const;
	bool		isdigit() const;
	bool		islower() const;
	bool		isupper() const;
	bool		isspace() const;
	friend bool	isalnum(const gist &);
	friend bool	isalpha(const gist &);
	friend bool	isdigit(const gist &);
	friend bool	islower(const gist &);
	friend bool	isupper(const gist &);
	friend bool	isspace(const gist &);

	friend int	atoi(const gist &, int base = 0);
	friend gist	strlower(const gist &);
	friend gist	strupper(const gist &);

	friend gist	strsplit(const gist & str, const char * sep = 0);
	friend gist	strsplit(const gist & str, const gist & sep);

	friend bool	strtrue(const gist &);

	friend void	strfill(gist &, unsigned size, const char * pattern);
	friend void	strfill(gist &, unsigned size, const gist & pattern);

	friend gist	strstrip(const gist &);
	friend gist	strcapwords(const gist &);

	/*
	 *	Arrays.
	 */
	gist &		array(unsigned len);
	gist &		arrayensure(unsigned len);
	void		push(const gist &);
	gist &		pop();
	void		unshift(const gist &);
	gist &		shift();

	/*
	 *	Tables.
	 */
	gist &		table(bool clear = false);
	void		tblset(const gist & key, const gist & val);
	void		tbladd(const gist & key, const gist & val);
	void		tbldel(const gist & key);
	bool		tblisset(const gist & key);
	bool		tblnext(gist & key, gist & val);
	bool		tblprev(gist & key, gist & val);

	/*
	 *	Pointers.
	 */
	void		ptrset(void * p)	{ typ = GT_PTR; ptr = p; }
	void *		ptrget() const;

	/*
	 *	Files.
	 */
	void		file(const char * fn = 0, const char * mode = 0);
	void		file(const gist & fn, const char * mode = 0);
	void		file(const char * fn, const gist & mode);
	void		file(const gist & fn, const gist & mode);
	void		file(int fd);
	gist		read(unsigned amount);
	gist		readline();
	void		write(gist & data);
	void		flush();
	void		close();

	/*
	 *	Regular expressions.
	 */
	bool		match(gist & str);
	bool		match(const char * str);
	bool		match(gist & str, gist & result);
	bool		match(const char * str, gist & result);

	/*
	 *	Generic methods.
	 */
	unsigned	len() const;
	friend unsigned	len(const gist &);

	/*
	 *	Formatting.
	 */
	char *		fmt(const char *, ...);

	/*
	 *	Debugging.
	 */
	friend void	GistPrint(gist *);

	/*
	 *	A NIL gist, for convenience, such as when wanting to
	 *	return NIL when a gist reference is required.
	 */
	static gist	nil;

	/*
	 *	Gist version information.
	 */
	static gist	version;
};

/**********************************************************************/
/*
 *	Non-member gist functions.
 */

/**********************************************************************/

#endif // __GIST_H__
