/*
 *	Gist -- Operators part 3.
 *
 *	Contains:	==  !=  <  >  <=  >=
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

#define OP0(l, r, op)						\
	if (l.isFloat() || r.isFloat())				\
		return l.toFloat() op r.toFloat();		\
	else if (l.isInt() || r.isInt())			\
		return l.toInt() op r.toInt();			\
	else if (l.isStr() && r.isStr())			\
		return l.strcmp(r) op 0;			\
	else							\
		throw gist::typeError("operator " #op);


#define OP1(l, r, op)						\
	if (l.isFloat())					\
		return l.toFloat() op (double)r;		\
	else if (l.isInt())					\
		return l.val op r;				\
	else							\
		throw gist::typeError("operator " #op);


#define OP1u(l, r, op)						\
	if (l.isFloat())					\
		return l.toFloat() op (double)r;		\
	else if (l.isInt())					\
		return (unsigned long)l.val op r;		\
	else							\
		throw gist::typeError("operator " #op);


#define OP1f(l, r, op)						\
	if (l.isFloat())					\
		return l.toFloat() op r;			\
	else if (l.isInt())					\
		return l.val op (int)r;				\
	else							\
		throw gist::typeError("operator " #op);


#define OP2(l, r, op)						\
	try							\
	{							\
		return l.strcmp(r) op 0;			\
	}							\
	catch (gist::valueError)				\
	{							\
		return 0;					\
	}

/**********************************************************************/

bool operator ==(const gist & l, const gist & r)	{ OP0(l, r, ==) }
bool operator ==(const gist & l, int r)			{ OP1(l, r, ==) }
bool operator ==(const gist & l, unsigned r)		{ OP1u(l, r, ==) }
bool operator ==(const gist & l, long r)		{ OP1(l, r, ==) }
bool operator ==(const gist & l, unsigned long r)	{ OP1u(l, r, ==) }
bool operator ==(int l, const gist & r)			{ OP1(r, l, ==) }
bool operator ==(unsigned l, const gist & r)		{ OP1u(r, l, ==) }
bool operator ==(long l, const gist & r)		{ OP1(r, l, ==) }
bool operator ==(unsigned long l, const gist & r)	{ OP1u(r, l, ==) }
bool operator ==(const gist & l, float r)		{ OP1f(l, r, ==) }
bool operator ==(const gist & l, double r)		{ OP1f(l, r, ==) }
bool operator ==(float l, const gist & r)		{ OP1f(r, l, ==) }
bool operator ==(double l, const gist & r)		{ OP1f(r, l, ==) }
bool operator ==(const gist & l, const char * r)	{ OP2(l, r, ==) }
bool operator ==(const char * l, const gist & r)	{ OP2(r, l, ==) }

bool operator !=(const gist & l, const gist & r)	{ OP0(l, r, !=) }
bool operator !=(const gist & l, int r)			{ OP1(l, r, !=) }
bool operator !=(const gist & l, unsigned r)		{ OP1u(l, r, !=) }
bool operator !=(const gist & l, long r)		{ OP1(l, r, !=) }
bool operator !=(const gist & l, unsigned long r)	{ OP1u(l, r, !=) }
bool operator !=(int l, const gist & r)			{ OP1(r, l, !=) }
bool operator !=(unsigned l, const gist & r)		{ OP1u(r, l, !=) }
bool operator !=(long l, const gist & r)		{ OP1(r, l, !=) }
bool operator !=(unsigned long l, const gist & r)	{ OP1u(r, l, !=) }
bool operator !=(const gist & l, float r)		{ OP1f(l, r, !=) }
bool operator !=(const gist & l, double r)		{ OP1f(l, r, !=) }
bool operator !=(float l, const gist & r)		{ OP1f(r, l, !=) }
bool operator !=(double l, const gist & r)		{ OP1f(r, l, !=) }
bool operator !=(const gist & l, const char * r)	{ OP2(l, r, !=) }
bool operator !=(const char * l, const gist & r)	{ OP2(r, l, !=) }

bool operator <(const gist & l, const gist & r)		{ OP0(l, r, <) }
bool operator <(const gist & l, int r)			{ OP1(l, r, <) }
bool operator <(const gist & l, unsigned r)		{ OP1u(l, r, <) }
bool operator <(const gist & l, long r)			{ OP1(l, r, <) }
bool operator <(const gist & l, unsigned long r)	{ OP1u(l, r, <) }
bool operator <(int l, const gist & r)			{ OP1(r, l, >) }
bool operator <(unsigned l, const gist & r)		{ OP1u(r, l, >) }
bool operator <(long l, const gist & r)			{ OP1(r, l, >) }
bool operator <(unsigned long l, const gist & r)	{ OP1u(r, l, >) }
bool operator <(const gist & l, float r)		{ OP1f(l, r, <) }
bool operator <(const gist & l, double r)		{ OP1f(l, r, <) }
bool operator <(float l, const gist & r)		{ OP1f(r, l, >) }
bool operator <(double l, const gist & r)		{ OP1f(r, l, >) }
bool operator <(const gist & l, const char * r)		{ OP2(l, r, <) }
bool operator <(const char * l, const gist & r)		{ OP2(r, l, >) }

bool operator >(const gist & l, const gist & r)		{ OP0(l, r, >) }
bool operator >(const gist & l, int r)			{ OP1(l, r, >) }
bool operator >(const gist & l, unsigned r)		{ OP1u(l, r, >) }
bool operator >(const gist & l, long r)			{ OP1(l, r, >) }
bool operator >(const gist & l, unsigned long r)	{ OP1u(l, r, >) }
bool operator >(int l, const gist & r)			{ OP1(r, l, <) }
bool operator >(unsigned l, const gist & r)		{ OP1u(r, l, <) }
bool operator >(long l, const gist & r)			{ OP1(r, l, <) }
bool operator >(unsigned long l, const gist & r)	{ OP1u(r, l, <) }
bool operator >(const gist & l, float r)		{ OP1f(l, r, >) }
bool operator >(const gist & l, double r)		{ OP1f(l, r, >) }
bool operator >(float l, const gist & r)		{ OP1f(r, l, <) }
bool operator >(double l, const gist & r)		{ OP1f(r, l, <) }
bool operator >(const gist & l, const char * r)		{ OP2(l, r, >) }
bool operator >(const char * l, const gist & r)		{ OP2(r, l, <) }

bool operator <=(const gist & l, const gist & r)	{ OP0(l, r, <=) }
bool operator <=(const gist & l, int r)			{ OP1(l, r, <=) }
bool operator <=(const gist & l, unsigned r)		{ OP1u(l, r, <=) }
bool operator <=(const gist & l, long r)		{ OP1(l, r, <=) }
bool operator <=(const gist & l, unsigned long r)	{ OP1u(l, r, <=) }
bool operator <=(int l, const gist & r)			{ OP1(r, l, >=) }
bool operator <=(unsigned l, const gist & r)		{ OP1u(r, l, >=) }
bool operator <=(long l, const gist & r)		{ OP1(r, l, >=) }
bool operator <=(unsigned long l, const gist & r)	{ OP1u(r, l, >=) }
bool operator <=(const gist & l, float r)		{ OP1f(l, r, <=) }
bool operator <=(const gist & l, double r)		{ OP1f(l, r, <=) }
bool operator <=(float l, const gist & r)		{ OP1f(r, l, >=) }
bool operator <=(double l, const gist & r)		{ OP1f(r, l, >=) }
bool operator <=(const gist & l, const char * r)	{ OP2(l, r, <=) }
bool operator <=(const char * l, const gist & r)	{ OP2(r, l, >=) }

bool operator >=(const gist & l, const gist & r)	{ OP0(l, r, >=) }
bool operator >=(const gist & l, int r)			{ OP1(l, r, >=) }
bool operator >=(const gist & l, unsigned r)		{ OP1u(l, r, >=) }
bool operator >=(const gist & l, long r)		{ OP1(l, r, >=) }
bool operator >=(const gist & l, unsigned long r)	{ OP1u(l, r, >=) }
bool operator >=(int l, const gist & r)			{ OP1(r, l, <=) }
bool operator >=(unsigned l, const gist & r)		{ OP1u(r, l, <=) }
bool operator >=(long l, const gist & r)		{ OP1(r, l, <=) }
bool operator >=(unsigned long l, const gist & r)	{ OP1u(r, l, <=) }
bool operator >=(const gist & l, float r)		{ OP1f(l, r, >=) }
bool operator >=(const gist & l, double r)		{ OP1f(l, r, >=) }
bool operator >=(float l, const gist & r)		{ OP1f(r, l, <=) }
bool operator >=(double l, const gist & r)		{ OP1f(r, l, <=) }
bool operator >=(const gist & l, const char * r)	{ OP2(l, r, >=) }
bool operator >=(const char * l, const gist & r)	{ OP2(r, l, <=) }



#if 0

bool
operator ==(const gist & l, const gist & r)
{
	if (l.isFloat() || r.isFloat())
		return l.toFloat() == r.toFloat();
	else if (l.isInt() || r.isInt())
		return l.toInt() == r.toInt();
	else if (l.isStr() && r.isStr())
		return l.strcmp(r) == 0;
	else
		throw gist::typeError("operator ==");
}


bool
operator ==(const gist & l, int r)
{
	if (l.isFloat())
		return l.toFloat() == (double)r;
	else if (l.isInt())
		return l.val == r;
	else
		throw gist::typeError("operator -");
}


bool
operator ==(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) == 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator ==(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) == 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator !=(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) != 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator !=(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) != 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) < 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) > 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) > 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) < 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <=(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) <= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator <=(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) >= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >=(const gist & l, const char * r)
{
	try
	{
		return l.strcmp(r) >= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}


bool
operator >=(const char * l, const gist & r)
{
	try
	{
		return r.strcmp(l) <= 0;
	}
	catch (gist::valueError)
	{
		return 0;
	}
}

#endif // 0
