/*
 *	The main external interface to Gist.
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
	friend gist	operator +(gist &, gist &);
	friend gist	operator +(gist &, int);
	friend gist	operator +(gist &, unsigned);
	friend gist	operator +(gist &, long);
	friend gist	operator +(gist &, unsigned long);
	friend gist	operator +(int, gist &);
	friend gist	operator +(unsigned, gist &);
	friend gist	operator +(long, gist &);
	friend gist	operator +(unsigned long, gist &);
	friend gist	operator +(gist &, float);
	friend gist	operator +(gist &, double);
	friend gist	operator +(float, gist &);
	friend gist	operator +(double, gist &);
	friend gist	operator +(gist &, const char *);
	friend gist	operator +(const char *, gist &);

#warning "repeat all this!"

	friend gist	operator -(gist &, gist &);
	friend gist	operator -(gist &, long);
	friend gist	operator -(long, gist &);
	friend gist	operator -(gist &, double);
	friend gist	operator -(double, gist &);

	friend gist	operator *(gist &, gist &);
	friend gist	operator *(gist &, long);
	friend gist	operator *(long, gist &);
	friend gist	operator *(gist &, double);
	friend gist	operator *(double, gist &);

	friend gist	operator /(gist &, gist &);
	friend gist	operator /(gist &, long);
	friend gist	operator /(long, gist &);
	friend gist	operator /(gist &, double);
	friend gist	operator /(double, gist &);

	friend gist	operator %(gist &, gist &);
	friend gist	operator %(gist &, long);
	friend gist	operator %(long, gist &);
	friend gist	operator %(gist &, double);
	friend gist	operator %(double, gist &);

	friend gist	operator ^(gist &, gist &);
	friend gist	operator ^(gist &, long);
	friend gist	operator ^(long, gist &);

	friend gist	operator &(gist &, gist &);
	friend gist	operator &(gist &, long);
	friend gist	operator &(long, gist &);

	friend gist	operator |(gist &, gist &);
	friend gist	operator |(gist &, long);
	friend gist	operator |(long, gist &);

	friend gist	operator <<(gist &, gist &);
	friend gist	operator <<(gist &, long);
	friend gist	operator <<(long, gist &);

	friend gist	operator >>(gist &, gist &);
	friend gist	operator >>(gist &, long);
	friend gist	operator >>(long, gist &);

	friend gist	operator &&(gist &, gist &);
	friend gist	operator &&(gist &, long);
	friend gist	operator &&(long, gist &);

	friend gist	operator ||(gist &, gist &);
	friend gist	operator ||(gist &, long);
	friend gist	operator ||(long, gist &);

	friend gist	operator ==(gist &, gist &);
	friend gist	operator ==(gist &, long);
	friend gist	operator ==(long, gist &);
	friend gist	operator ==(gist &, double);
	friend gist	operator ==(double, gist &);
	friend gist	operator ==(gist &, const char *);
	friend gist	operator ==(const char *, gist &);

	friend gist	operator !=(gist &, gist &);
	friend gist	operator !=(gist &, long);
	friend gist	operator !=(long, gist &);
	friend gist	operator !=(gist &, double);
	friend gist	operator !=(double, gist &);
	friend gist	operator !=(gist &, const char *);
	friend gist	operator !=(const char *, gist &);

	friend gist	operator <(gist &, gist &);
	friend gist	operator <(gist &, long);
	friend gist	operator <(long, gist &);
	friend gist	operator <(gist &, double);
	friend gist	operator <(double, gist &);
	friend gist	operator <(gist &, const char *);
	friend gist	operator <(const char *, gist &);

	friend gist	operator >(gist &, gist &);
	friend gist	operator >(gist &, long);
	friend gist	operator >(long, gist &);
	friend gist	operator >(gist &, double);
	friend gist	operator >(double, gist &);
	friend gist	operator >(gist &, const char *);
	friend gist	operator >(const char *, gist &);

	friend gist	operator <=(gist &, gist &);
	friend gist	operator <=(gist &, long);
	friend gist	operator <=(long, gist &);
	friend gist	operator <=(gist &, double);
	friend gist	operator <=(double, gist &);
	friend gist	operator <=(gist &, const char *);
	friend gist	operator <=(const char *, gist &);

	friend gist	operator >=(gist &, gist &);
	friend gist	operator >=(gist &, long);
	friend gist	operator >=(long, gist &);
	friend gist	operator >=(gist &, double);
	friend gist	operator >=(double, gist &);
	friend gist	operator >=(gist &, const char *);
	friend gist	operator >=(const char *, gist &);

	gist		operator -();
	gist		operator !();
	gist		operator ~();

	gist &		operator +=(gist &);
	gist &		operator +=(long);
	gist &		operator +=(double);
	gist &		operator +=(const char *);

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



#if 0

class GistObject
{
	friend class Str;
	friend class Long;

    private:
	void incref() { ref++; }
	void decref() { ref--; }

	int	ref;
};

namespace Gist
{
	// class GistObject;

	class Gist
	{
	    public:
		Gist()		{}
		~Gist()		{}

	    protected:
		GistObject * obj;

		void	incref();
		void	decref();
	};
}


// class Int : Gist
// {
// };


class Long : Gist::Gist
{
};


class Real : Gist::Gist
{
};


class Str // : Gist::Gist
{
    public:
	Str()			{ obj = 0; }
	Str(const char *);
	Str(const Str & s)	{ obj = s.obj; obj->incref(); }
	~Str()			{}

	Str &	operator =(const char *);
	Str &	operator =(const Str &);

	Str &	set(const char *, unsigned len = (~0U>>1));
	Str &	set(const Str &);

    private:
	GistObject *	obj;
};


// class Float : Gist
// {
// };


class Array : Gist::Gist
{
};


class Table : Gist::Gist
{
    public:
	Gist &	operator [](int);
	Gist &	operator [](const char *);
	Gist &	operator [](const Gist &);
};


class Code : Gist::Gist
{
};




// using namespace Gist::Gist;
// using namespace Gist::Int;
// using namespace Gist::Long;
// using namespace Gist::Real;
// using namespace Gist::Str;
// using namespace Gist::Float;
// using namespace Gist::Array;
// using namespace Gist::Table;
// using namespace Gist::Code;

#endif // 0

#if 0

/**********************************************************************/

/*
 *	This is *the* external interface for Gist.  Instances of this class
 *	are a single Gist value/object.  Member functions provide access to
 *	data, compile and execute Gist code, etc.
 */
class Gist_t
{
    public:
	Gist_t(void);			// NIL
	Gist_t(int);			// Integer
	Gist_t(double);			// Float
	Gist_t(const char *);		// String
	~Gist_t();

	/****************************************/
    public:
	/*
	 *	Value extraction
	 */

	/*
	 *	The Gist value is converted as if by `convert_to_integer()'
	 *	or `convert_to_float()' and returned.  The Gist value
	 *	remains unchanged.
	 */
	operator int();			// Converts value to integer
	operator double();		// Converts value to float

	/*
	 *	Copy the Gist value to the given string, converting as
	 *	necessary.  The first version copies the data into the
	 *	buffer described by `ptr' and `len';  a '\0' character will
	 *	be appended if there is room.  The second version allocates
	 *	memory from the system memory pool (using new or malloc())
	 *	and copies the string there;  a '\0' is always appended.
	 *	If the value is not representable as a string, an empty
	 *	string is returned.
	 */
	int		string_get(char * ptr, int len);
	char *		string_get(int & len);

	/*
	 *	Return a reference to the data of an atom.  `ptr' will point
	 *	to the first byte of data and `len' will be the length in
	 *	bytes of the data.  A reference to a list or NIL cannot be
	 *	returned;  if the Gist value is a list or NIL, the pointer
	 *	and length will be set to zero.
	 *
	 *	WARNING:  Usually the pointer returned will point to data in
	 *		  the compaction arena.  Any operation that can
	 *		  cause garbage collection (almost any member function
	 *		  here) will cause the data to move from under the
	 *		  pointer.  
	 */
	void		get_raw(const char *& ptr, int & len);

	/****************************************/

	/*
	 *	Convert this Gist value to the required type.  When
	 *	converting a string to a numeric value, it is converted
	 *	in a similar way to `strtol()' in the C library, with base
	 *	set to 0.  
	 */
	void		convert_to_integer(void);
	void		convert_to_long(void);
	void		convert_to_float(void);
	void		convert_to_string(void);

	/*
	 *	CAR and CDR operations (from LISP).  A car or cdr of an
	 *	atom yields the atom itself.
	 */
	void		car(void);
	void		cdr(void);
	void		cadr(void);
	void		caar(void);
	void		cddr(void);
	void		caddr(void);
	void		cdddr(void);
	void		cadddr(void);
	void		cddddr(void);
	void		caddddr(void);

	/****************************************/
    private:
	/*
	 *	The pointer to the data.
	 */
	class Pair_t *	data;
};

#endif // 0

/**********************************************************************/

#endif // __GIST_H__
