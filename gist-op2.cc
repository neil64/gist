/*
 *	Gist -- Operators part 2.
 *
 *	Contains:	+= -= *= /= %= ^= &= |= <<= >>=
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

#define OP1(r, op)						\
	if (isInt())						\
	{							\
		val op r.toInt();				\
		return *this;					\
	}							\
	throw gist::typeError("operator " #op);


#define OP2(r, op)						\
	if (isInt())						\
	{							\
		val op r;					\
		return *this;					\
	}							\
	throw gist::typeError("operator " #op);


#define OP1f(r, op)						\
	if (isInt())						\
	{							\
		val op r.toInt();				\
		return *this;					\
	}							\
	if (isFloat())						\
	{							\
		dval op r.toFloat();				\
		return *this;					\
	}							\
	throw gist::typeError("operator " #op);


#define OP2f(r, op)						\
	if (isInt())						\
	{							\
		val op r;					\
		return *this;					\
	}							\
	if (isFloat())						\
	{							\
		dval op (double)r;				\
		return *this;					\
	}							\
	throw gist::typeError("operator " #op);


#define OP2g(r, op)						\
	if (!isFloat())						\
	{							\
		dval = toFloat();				\
		typ = GT_FLOAT;					\
	}							\
	dval op r;						\
	return *this;


#define OP2s(r, op)						\
	if (isInt())						\
	{							\
		val op r;					\
		return *this;					\
	}							\
	if (isFloat())						\
	{							\
		dval op (double)r;				\
		return *this;					\
	}							\
	if (isStr())						\
	{							\
		strcat(r);					\
		return *this;					\
	}							\
	throw gist::typeError("operator " #op);


/********************/

gist &
gist::operator +=(const gist & r)
{
	if (isInt())
	{
		val += r.toInt();
		return *this;
	}
	if (isFloat())
	{
		dval += r.toFloat();
		return *this;
	}
	if (isStr())
	{
		strcat(r);
		return *this;
	}

	throw gist::typeError("operator +=");
}


gist &
gist::operator +=(const char * s)
{
	gist x(s);

	if (isFloat())
		dval += x.toFloat();
	else if (isInt())
		val += x.toInt();
	else
		strcat(x);

	return *this;
}


gist & gist::operator +=(int r)			{ OP2s(r, +=) }
gist & gist::operator +=(unsigned r)		{ OP2s(r, +=) }
gist & gist::operator +=(long r)		{ OP2s(r, +=) }
gist & gist::operator +=(unsigned long r)	{ OP2s(r, +=) }
gist & gist::operator +=(float r)		{ OP2g(r, +=) }
gist & gist::operator +=(double r)		{ OP2g(r, +=) }

/********************/

gist & gist::operator -=(const gist & r)	{ OP1f(r, -=) }
gist & gist::operator -=(int r)			{ OP2f(r, -=) }
gist & gist::operator -=(unsigned r)		{ OP2f(r, -=) }
gist & gist::operator -=(long r)		{ OP2f(r, -=) }
gist & gist::operator -=(unsigned long r)	{ OP2f(r, -=) }
gist & gist::operator -=(float r)		{ OP2g(r, -=) }
gist & gist::operator -=(double r)		{ OP2g(r, -=) }

/********************/

gist & gist::operator *=(const gist & r)	{ OP1f(r, *=) }
gist & gist::operator *=(int r)			{ OP2f(r, *=) }
gist & gist::operator *=(unsigned r)		{ OP2f(r, *=) }
gist & gist::operator *=(long r)		{ OP2f(r, *=) }
gist & gist::operator *=(unsigned long r)	{ OP2f(r, *=) }
gist & gist::operator *=(float r)		{ OP2g(r, *=) }
gist & gist::operator *=(double r)		{ OP2g(r, *=) }

/********************/

gist & gist::operator /=(const gist & r)	{ OP1f(r, /=) }
gist & gist::operator /=(int r)			{ OP2f(r, /=) }
gist & gist::operator /=(unsigned r)		{ OP2f(r, /=) }
gist & gist::operator /=(long r)		{ OP2f(r, /=) }
gist & gist::operator /=(unsigned long r)	{ OP2f(r, /=) }
gist & gist::operator /=(float r)		{ OP2g(r, /=) }
gist & gist::operator /=(double r)		{ OP2g(r, /=) }

/********************/

gist & gist::operator %=(const gist & r)	{ OP1(r, %=) }
gist & gist::operator %=(int r)			{ OP2(r, %=) }
gist & gist::operator %=(unsigned r)		{ OP2(r, %=) }
gist & gist::operator %=(long r)		{ OP2(r, %=) }
gist & gist::operator %=(unsigned long r)	{ OP2(r, %=) }

/********************/

gist & gist::operator ^=(const gist & r)	{ OP1(r, ^=) }
gist & gist::operator ^=(int r)			{ OP2(r, ^=) }
gist & gist::operator ^=(unsigned r)		{ OP2(r, ^=) }
gist & gist::operator ^=(long r)		{ OP2(r, ^=) }
gist & gist::operator ^=(unsigned long r)	{ OP2(r, ^=) }

/********************/

gist & gist::operator &=(const gist & r)	{ OP1(r, &=) }
gist & gist::operator &=(int r)			{ OP2(r, &=) }
gist & gist::operator &=(unsigned r)		{ OP2(r, &=) }
gist & gist::operator &=(long r)		{ OP2(r, &=) }
gist & gist::operator &=(unsigned long r)	{ OP2(r, &=) }

/********************/

gist & gist::operator |=(const gist & r)	{ OP1(r, |=) }
gist & gist::operator |=(int r)			{ OP2(r, |=) }
gist & gist::operator |=(unsigned r)		{ OP2(r, |=) }
gist & gist::operator |=(long r)		{ OP2(r, |=) }
gist & gist::operator |=(unsigned long r)	{ OP2(r, |=) }

/********************/

gist & gist::operator <<=(const gist & r)	{ OP1(r, <<=) }
gist & gist::operator <<=(int r)		{ OP2(r, <<=) }
gist & gist::operator <<=(unsigned r)		{ OP2(r, <<=) }
gist & gist::operator <<=(long r)		{ OP2(r, <<=) }
gist & gist::operator <<=(unsigned long r)	{ OP2(r, <<=) }

/********************/

gist & gist::operator >>=(const gist & r)	{ OP1(r, >>=) }
gist & gist::operator >>=(int r)		{ OP2(r, >>=) }
gist & gist::operator >>=(unsigned r)		{ OP2(r, >>=) }
gist & gist::operator >>=(long r)		{ OP2(r, >>=) }
gist & gist::operator >>=(unsigned long r)	{ OP2(r, >>=) }
