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
	gist(const char * s, int len = -1)	{ set(s, len); }

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
	void		set(const char *, int = -1);
	void		set(long long v)	{ typ = GT_INT; val = v; }
	void		set(unsigned long long v) { typ = GT_INT; val = v; }
	void		set(long double v)	{ typ = GT_FLOAT; dval = v; }

	void		clear()			{ typ = GT_NIL; }

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
	gist &		operator [](const gist &);

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
	 *	If converting a string to an integer with `toInt()', the C
	 *	library `strtol' function is used, allowing conversions using
	 *	base 2 to 36, and the default "base 0" which accepts the
	 *	prefix "0x" for hexadecimal and "0" for octal.	When casting
	 *	a string to an integer, only base 10 is accepted.  If the
	 *	conversion causes an overflow, `toInt()' and `toFloat()'
	 *	both throw and `overflowError' exception.  Overflow from one
	 *	of the casts returns an undefined result.
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
	friend r	operator o(const char *, const gist &);

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
	GIST_OPS2a(bool, ==)
	GIST_OPS2a(bool, !=)
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
		GT_CODE,
		GT_INT,
		GT_FLOAT,
		GT_LONG,
		GT_REAL
	};
	type_e		type() const		{ return typ; }

	int		isNil() const		{ return typ == GT_NIL; }
	int		isInt() const		{ return typ == GT_INT; }
	int		isFloat() const		{ return typ == GT_FLOAT; }
	int		isStr() const		{ return typ == GT_STR; }
	int		isNumber() const	{ return typ >= GT_INT; }
	int		isArray() const		{ return typ == GT_ARRAY; }
	int		isTable() const		{ return typ == GT_TABLE; }

	/********************************/
	/*
	 *	Exceptions.
	 */
	struct error
	{
		const char * msg;
		error() : msg(0) {}
		error(const char * m) : msg(m) {}
	};
#define GIST_ERROR(t)					\
	struct t : error				\
	{						\
		t() {}					\
		t(const char * m) : error(m) {}		\
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
	 *	When using a gist array, and index out of bounds gives
	 *	an `indexError'.
	 */
	GIST_ERROR(indexError)

	/*
	 *	If a conversion causes an overflow, an `overflowError'
	 *	is thrown.
	 */
	GIST_ERROR(overflowError)

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
		 *	2 bytes reserved for future use.
		 */
		// short	res0;

		/*
		 *  A pointer to internal data, if required.  For string
		 *  and array types, this is the string / array storage.
		 */
		struct gistInternal * ptr;	// Internal data

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
		};
	    };

	    /*
	     *	A structure to cover the whole object, to make it easy to
	     *	copy.  Copying a gist object using something like "a.all =
	     *	b.all" gives compilers a chance to generate more efficient
	     *	code for the copy.  We include a copy of most of the above
	     *	declarations so that we can get gdb to behave.
	     */
	    struct {
		type_e typ:8;
		bool unique;
		struct gistInternal * ptr;
		union {
		    long val;
		    double dval;
		    struct { unsigned cnt; unsigned skip; } s;
		};
	    } all;
	};

	/*
	 *	Private methods.
	 */
	char *		_strcast(bool rw) const;
	unsigned	_strpiece(int & index, const char *& ptr) const;
	void		_strflatten() const;

	gist &		_arrayindex(long);
	gist &		_tableindex(long);
	gist &		_tableindex(double);
	gist &		_tableindex(const char *);
	gist &		_tableindex(const gist &);

//	static void	_coerce1(const gist & l, const gist *& lp,
//				const gist & r, const gist *& rp,
//				gist & x, const char * op);

	/*
	 *	Some internal structures that we call friends.
	 */
	friend class giStr;

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

	unsigned	strpiece(int & index, const char *& ptr) const;
	friend unsigned	strpiece(const gist &, int & index, const char *& ptr);

	void		strcat(int);
	void		strcat(const char *);
	void		strcopycat(const char *);
	void		strcat(const gist &);
	friend void	strcat(gist &, int);
	friend void	strcat(gist &, const char *);
	friend void	strcopycat(gist &, const char *);
	friend void	strcat(gist &, const gist &);
	friend void	strcat(char *, const gist &);
	friend void	strncat(char *, const gist &, unsigned);

	void		strins(int);
	void		strins(const char *);
	void		strcopyins(const char *);
	void		strins(const gist &);

	static unsigned	strcpy(char * dest, const gist & src,
				unsigned start = 0, unsigned count = (~0U>>1));

	/*
	 *	Array's.
	 */
	void		append(const gist &);
	void		insert(const gist &);
	gist &		pop();
	gist &		shift();

	/*
	 *	Formatting.
	 */
	char *		fmt(const char *, ...);
};

/**********************************************************************/
/*
 *	Non-member gist functions.
 */

/**********************************************************************/

#endif // __GIST_H__
