/*
 *	Gist -- integers.
 */

#include	"gist.h"


/**********************************************************************/

class gist
{
    public:
	/********************************/
	/*
	 *	Constructors.
	 *
	 *	Note that there is no destructor, since we are using
	 *	garbage collection to reclaim space.
	 */
	gist()			{ ptr = &nil; }
	gist(int v)		{ ptr = 0; val = v; }
	gist(unsigned v)	{ ptr = 0; val = v; }
	gist(long v)		{ ptr = 0; val = v; }
	gist(unsigned long v)	{ ptr = 0; val = v; }
	gist(const gist & g)	{ ptr = g.ptr; val = g.val; }
	gist(const gist * g)	{ ptr = g->ptr; val = g->val; }

	gist(long long);
	gist(unsigned long long);
	gist(float);
	gist(double);

	gist(const char *, int len = -1);

	/********************************/
	/*
	 *	Assignment.
	 */
	gist &	operator =(int v)		{ ptr = 0; val = v; }
	gist &	operator =(unsigned v)		{ ptr = 0; val = v; }
	gist &	operator =(long v)		{ ptr = 0; val = v; }
	gist &	operator =(unsigned long v)	{ ptr = 0; val = v; }
	gist &	operator =(const gist & g)	{ ptr = g.ptr; val = g.val; }
	gist &	operator =(const gist * g)	{ ptr = g->ptr; val = g->val; }

	gist &		operator =(long long);
	gist &		operator =(unsigned long long);
	gist &		operator =(float);
	gist &		operator =(double);

	gist &		operator =(const char *);

	/********************************/
	/*
	 *	Memory allocation.
	 */
	void *		operator new(unsigned);
	void		operator delete(void *) {}

	/********************************/
	/*
	 *	Subscripting.  It always returns a reference to a gist object,
	 *	which will usually be an element of a gist array.  The object
	 *	can be modified in place.
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
	double		toNumber() const;
			operator float() const;
			operator double() const;

	/*
	 *	Obtain a string representation of the gist object (by using
	 *	toString() if necessary), then
	 *	copy the string, terminated with a '\0', into a new chunk
	 *	of memory and return it.  Multiple calls may or may not return
	 *	the same chunk of memory.
	 */
			operator char *() const;

	/********************************/
	/*
	 *	Operators.
	 */
#define GIST_OPS1(o)							\
	friend gist	operator o(const gist &, const gist &);		\
	friend gist	operator o(const gist &, int);			\
	friend gist	operator o(const gist &, unsigned);		\
	friend gist	operator o(const gist &, long);			\
	friend gist	operator o(const gist &, unsigned long);	\
	friend gist	operator o(const gist &, long long);		\
	friend gist	operator o(const gist &, unsigned long long);	\
	friend gist	operator o(int, const gist &);			\
	friend gist	operator o(unsigned, const gist &);		\
	friend gist	operator o(long, const gist &);			\
	friend gist	operator o(unsigned long, const gist &);	\
	friend gist	operator o(long long, const gist &);		\
	friend gist	operator o(unsigned long long, const gist &);

#define GIST_OPS2(o)							\
	GIST_OPS1(o)							\
	friend gist	operator o(const gist &, float);		\
	friend gist	operator o(const gist &, double);		\
	friend gist	operator o(float, const gist &);		\
	friend gist	operator o(double, const gist &);

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

	GIST_OPS2(+)
	friend gist	operator +(const gist &, const char *);
	friend gist	operator +(const char *, const gist &);
	GIST_OPS2(-)
	GIST_OPS2(*)
	GIST_OPS2(/)
	GIST_OPS2(%)
	GIST_OPS2(==)
	GIST_OPS2(!=)
	GIST_OPS2(<)
	GIST_OPS2(>)
	GIST_OPS2(<=)
	GIST_OPS2(>=)

	GIST_OPS1(^)
	GIST_OPS1(&)
	GIST_OPS1(|)
	GIST_OPS1(<<)
	GIST_OPS1(>>)
	GIST_OPS1(&&)
	GIST_OPS1(||)

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

	gist		operator -();
	gist		operator ~();
	gist		operator !();

	gist		operator ++();
	gist		operator ++(int);
	gist		operator --();
	gist		operator --(int);

	// gist		operator &();
	// gist		operator ,(const gist &);
#undef GIST_OPS1
#undef GIST_OPS2
#undef GIST_OPS3
#undef GIST_OPS4

	/********************************/
	/*
	 *	Exceptions.
	 */

	/*
	 *	A value error is thrown typically when a value cannot be
	 *	coerced into a useful form, such as trying to convert a
	 *	string to a number where the string does not contain digits.
	 */
	struct valueError {};

	/*
	 *	When using a gist array, and index out of bounds gives
	 *	an `indexError'.
	 */
	struct indexError {};

	/*
	 *	If a conversion causes an overflow, an `overflowError'
	 *	is thrown.
	 */
	struct overlowError {};

	/**************************************************************/
	/**************************************************************/
	/*
	 *	Gist per object storage.  We try very hard to make this
	 *	no more than 8 bytes.  The `ptr' is usually a pointer to
	 *	internal data, or it can be 0.  If 0, the value is an
	 *	integer with the integer value stored in `value.  If the
	 *	pointer is non-zero, the type is determined in other ways.
	 *	For non-integer values, the use of `value' is varied.
	 */
    private:
	void *		ptr;
	long		val;

	/*
	 *	The NIL object.  Any gist that points here is a NIL object.
	 *	The object itself does not contain anything useful.
	 */
	static gist	nil;
};


/**********************************************************************/

#endif // __GIST_H__
