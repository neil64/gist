/*
 *	The main external interface to Gist.
 */

#ifndef __GIST_H__
#define __GIST_H__

/**********************************************************************/

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
