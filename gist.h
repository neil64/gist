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
	gist()			{ ptr = &Nil; }
	gist(int v)		{ ptr = &Int; val = v; }
	gist(unsigned v)	{ ptr = &Int; val = v; }
	gist(long v)		{ ptr = &Int; val = v; }
	gist(unsigned long v)	{ ptr = &Int; val = v; }
	gist(float v)		{ ptr = &Float; dval = v; }
	gist(double v)		{ ptr = &Float; dval = v; }
	gist(const gist & g)	{ ptr = g.ptr; val = g.val; }
	gist(const gist * g)	{ ptr = g->ptr; val = g->val; }

	gist(long long);
	gist(unsigned long long);

	gist(const char *, int len = -1);

	/********************************/
	/*
	 *	Assignment.
	 */
	gist &	operator =(int v)		{ ptr = &Int; val = v;
							return *this; }
	gist &	operator =(unsigned v)		{ ptr = &Int; val = v;
							return *this; }
	gist &	operator =(long v)		{ ptr = &Int; val = v;
							return *this; }
	gist &	operator =(unsigned long v)	{ ptr = &Int; val = v;
							return *this; }
	gist &	operator =(float v)		{ ptr = &Float; dval = v;
							return *this; }
	gist &	operator =(double v)		{ ptr = &Float; dval = v;
							return *this; }
	gist &	operator =(const gist & g)	{ ptr = g.ptr;
						  all[0] = g.all[0];
						  all[1] = g.all[1];
						  return *this; }
	gist &	operator =(const gist * g)	{ ptr = g->ptr;
						  all[0] = g->all[0];
						  all[1] = g->all[1];
						  return *this; }
	gist &		operator =(long long);
	gist &		operator =(unsigned long long);
	gist &		operator =(const char *);


	gist &		set()			{ ptr = &Nil;
							return *this; }
	gist &		set(int v)		{ ptr = &Int; val = v;
							return *this; }
	gist &		set(unsigned v)		{ ptr = &Int; val = v;
							return *this; }
	gist &		set(long v)		{ ptr = &Int; val = v;
							return *this; }
	gist &		set(unsigned long v)	{ ptr = &Int; val = v;
							return *this; }
	gist &		set(float v)		{ ptr = &Float; dval = v;
							return *this; }
	gist &		set(double v)		{ ptr = &Float; dval = v;
							return *this; }
	gist &		set(const gist & g)	{ ptr = g.ptr;
						  all[0] = g.all[0];
						  all[1] = g.all[1];
						  return *this; }
	gist &		set(const gist * g)	{ ptr = g->ptr;
						  all[0] = g->all[0];
						  all[1] = g->all[1];
						  return *this; }
	gist &		set(long long);
	gist &		set(unsigned long long);
	gist &		set(const char *, int = -1);

	void		clear()			{ ptr = &Nil; }

	/********************************/
	/*
	 *	Shallow copying.
	 */

	gist &		copy(const char *, int = -1);
	gist &		copy(const gist &);
	gist &		copy(const gist *);

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
	 */
	long		toInt() const;
			operator int() const;
			operator unsigned() const;
			operator long() const;
			operator unsigned long() const;
			operator long long() const;
			operator unsigned long long() const;
	double		toFloat() const;
			operator float() const		{ return toFloat(); }
			operator double() const		{ return toFloat(); }

	/*
	 *	Obtain a string representation of the gist object (by
	 *	using toString() if necessary), then copy the string,
	 *	terminated with a '\0', into a new chunk of memory and
	 *	return it.  Multiple calls may or may not return the same
	 *	(cached) chunk of memory.  The alternate `const char *'
	 *	can return the memory used for the actual string storage,
	 *	rather than a cache;  modifying this memory might change
	 *	the original string.
	 */
	operator const char *() const
	{
		return (const char *)strCast(0);
	}

	operator char *() const
	{
		return strCast(1);
	}

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
	GIST_OPS2(gist, %)
	GIST_OPS2a(int, ==)
	GIST_OPS2a(int, !=)
	GIST_OPS2a(int, <)
	GIST_OPS2a(int, >)
	GIST_OPS2a(int, <=)
	GIST_OPS2a(int, >=)

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
	gist &		operator ++(int);
	gist &		operator --();
	gist &		operator --(int);

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
	type_e		type() const		{ return ptr->type; }

	int		isNil() const		{ return ptr == &Nil; }
	int		isInt() const		{ return ptr == &Int; }
	int		isFloat() const		{ return ptr == &Float; }
	int		isStr() const		{ return type() == GT_STR; }
	int		isNumber() const	{ return type() >= GT_INT; }
	int		isArray() const		{ return type() == GT_ARRAY; }
	int		isTable() const		{ return type() == GT_TABLE; }

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
	 */

	/*
	 *	The base type of the internal gist data structure.
	 */
	struct gistInternal
	{
		type_e	type;
	};

	/*
	 *	Gist per object storage.  We try very hard to make this
	 *	no more than 8 bytes.  The `ptr' is usually a pointer to
	 *	internal data, or it can be 0.  If 0, the value is an
	 *	integer with the integer value stored in `value.  If the
	 *	pointer is non-zero, the type is determined in other ways.
	 *	For non-integer values, the use of `value' is varied.
	 *
	 *	Well ok, it's now 12 bytes.  This should not make a whole
	 *	lot of difference with the use of the Boehm GC -- the GC
	 *	defaults to placing a single byte after any allocation,
	 *	to allow pointer references to the the of an object + 1
	 *	to still hit the object (reasonable, I guess), then after
	 *	adding the byte, it rounds up to the next allocation size.
	 *	So for 8 bytes, to really get 16.  For 12 bytes we still
	 *	get 16.  Voila.  (Tellement maintenant nous devons essayer
	 *	tres dur de ne pas lui faire plus de 15 bytes.)
	 */
    private:
	gistInternal *		ptr;
	union
	{
		long		val;		// Integer type
		double		dval;		// Float type
		struct
		{
			unsigned	cnt;	// String/Array type
			unsigned	skip;
		};
		long		all[2];		// One entry to cover it all
	};

	/*
	 *	The NIL object and integer typing object.  All NIL objects,
	 *	and integer objects point here.
	 */
	static gistInternal	Nil;
	static gistInternal	Int;
	static gistInternal	Float;

	/*
	 *	Private methods.
	 */
	char *		strCast(int) const;

	/**************************************************************/
	/**************************************************************/
	/*
	 *	Public gist member functions.
	 */
    public:

	/*
	 *	Strings.
	 */
	int		cmp(const char *) const;

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
