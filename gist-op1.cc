/*
 *	Gist -- Operators part 1.
 *
 *	Contains:	+ - * / % ^ & | << >>
 */

#include	"gist.h"
#include	"gist-internal.h"

/**********************************************************************/

#define OP0(l, r, op)						\
	if (l.isInt() || r.isInt())				\
		return gist(l.toInt() op r.toInt());		\
	else							\
		throw gist::typeError("operator " #op);


#define OP1(l, r, op)						\
	if (l.isInt())						\
		return gist(l.val op r);			\
	else							\
		throw gist::typeError("operator " #op);


#define OP2(l, r, op)						\
	if (r.isInt())						\
		return gist(l op r.val);			\
	else							\
		throw gist::typeError("operator " #op);


#define OP0f(l, r, op)						\
	if (l.isFloat() || r.isFloat())				\
		return gist(l.toFloat() op r.toFloat());	\
	else if (l.isInt() || r.isInt())			\
		return gist(l.toInt() op r.toInt());		\
	else							\
		throw gist::typeError("operator " #op);


#define OP1f(l, r, op)						\
	if (l.isFloat())					\
		return gist(l.toFloat() op (double)r);		\
	else if (l.isInt())					\
		return gist(l.val op r);			\
	else							\
		throw gist::typeError("operator " #op);


#define OP2f(l, r, op)						\
	if (r.isFloat())					\
		return gist((double)l op r.toFloat());		\
	else if (r.isInt())					\
		return gist(l op r.val);			\
	else							\
		throw gist::typeError("operator " #op);


/**********************************************************************/

gist
operator +(const gist & l, const gist & r)
{
	if (l.isFloat() || r.isFloat())
		return gist(l.toFloat() + r.toFloat());
	else if (l.isInt() || r.isInt())
		return gist(l.toInt() + r.toInt());
	else if (l.isStr() && r.isStr())
	{
		gist x(l);
		x.strcat(r);
		return x;
	}
	else
		throw gist::typeError("operator +");
}


gist operator +(const gist & l, int r)			{ OP1f(l, r, +) }
gist operator +(const gist & l, unsigned r)		{ OP1f(l, r, +) }
gist operator +(const gist & l, long r)			{ OP1f(l, r, +) }
gist operator +(const gist & l, unsigned long r)	{ OP1f(l, r, +) }
gist operator +(const gist & l, float r)		{ OP1f(l, r, +) }
gist operator +(const gist & l, double r)		{ OP1f(l, r, +) }
gist operator +(int l, const gist & r)			{ OP2f(l, r, +) }
gist operator +(unsigned l, const gist & r)		{ OP2f(l, r, +) }
gist operator +(long l, const gist & r)			{ OP2f(l, r, +) }
gist operator +(unsigned long l, const gist & r)	{ OP2f(l, r, +) }
gist operator +(float l, const gist & r)		{ OP2f(l, r, +) }
gist operator +(double l, const gist & r)		{ OP2f(l, r, +) }

/********************/

gist operator -(const gist & l, const gist & r)		{ OP0f(l, r, -) }
gist operator -(const gist & l, int r)			{ OP1f(l, r, -) }
gist operator -(const gist & l, unsigned r)		{ OP1f(l, r, -) }
gist operator -(const gist & l, long r)			{ OP1f(l, r, -) }
gist operator -(const gist & l, unsigned long r)	{ OP1f(l, r, -) }
gist operator -(const gist & l, float r)		{ OP1f(l, r, -) }
gist operator -(const gist & l, double r)		{ OP1f(l, r, -) }
gist operator -(int l, const gist & r)			{ OP2f(l, r, -) }
gist operator -(unsigned l, const gist & r)		{ OP2f(l, r, -) }
gist operator -(long l, const gist & r)			{ OP2f(l, r, -) }
gist operator -(unsigned long l, const gist & r)	{ OP2f(l, r, -) }
gist operator -(float l, const gist & r)		{ OP2f(l, r, -) }
gist operator -(double l, const gist & r)		{ OP2f(l, r, -) }

/********************/

gist operator *(const gist & l, const gist & r)		{ OP0f(l, r, *) }
gist operator *(const gist & l, int r)			{ OP1f(l, r, *) }
gist operator *(const gist & l, unsigned r)		{ OP1f(l, r, *) }
gist operator *(const gist & l, long r)			{ OP1f(l, r, *) }
gist operator *(const gist & l, unsigned long r)	{ OP1f(l, r, *) }
gist operator *(const gist & l, float r)		{ OP1f(l, r, *) }
gist operator *(const gist & l, double r)		{ OP1f(l, r, *) }
gist operator *(int l, const gist & r)			{ OP2f(l, r, *) }
gist operator *(unsigned l, const gist & r)		{ OP2f(l, r, *) }
gist operator *(long l, const gist & r)			{ OP2f(l, r, *) }
gist operator *(unsigned long l, const gist & r)	{ OP2f(l, r, *) }
gist operator *(float l, const gist & r)		{ OP2f(l, r, *) }
gist operator *(double l, const gist & r)		{ OP2f(l, r, *) }

/********************/

gist operator /(const gist & l, const gist & r)		{ OP0f(l, r, /) }
gist operator /(const gist & l, int r)			{ OP1f(l, r, /) }
gist operator /(const gist & l, unsigned r)		{ OP1f(l, r, /) }
gist operator /(const gist & l, long r)			{ OP1f(l, r, /) }
gist operator /(const gist & l, unsigned long r)	{ OP1f(l, r, /) }
gist operator /(const gist & l, float r)		{ OP1f(l, r, /) }
gist operator /(const gist & l, double r)		{ OP1f(l, r, /) }
gist operator /(int l, const gist & r)			{ OP2f(l, r, /) }
gist operator /(unsigned l, const gist & r)		{ OP2f(l, r, /) }
gist operator /(long l, const gist & r)			{ OP2f(l, r, /) }
gist operator /(unsigned long l, const gist & r)	{ OP2f(l, r, /) }
gist operator /(float l, const gist & r)		{ OP2f(l, r, /) }
gist operator /(double l, const gist & r)		{ OP2f(l, r, /) }

/********************/

gist operator %(const gist & l, const gist & r)		{ OP0(l, r, %) }
gist operator %(const gist & l, int r)			{ OP1(l, r, %) }
gist operator %(const gist & l, unsigned r)		{ OP1(l, r, %) }
gist operator %(const gist & l, long r)			{ OP1(l, r, %) }
gist operator %(const gist & l, unsigned long r)	{ OP1(l, r, %) }
gist operator %(int l, const gist & r)			{ OP2(l, r, %) }
gist operator %(unsigned l, const gist & r)		{ OP2(l, r, %) }
gist operator %(long l, const gist & r)			{ OP2(l, r, %) }
gist operator %(unsigned long l, const gist & r)	{ OP2(l, r, %) }

/********************/

gist operator ^(const gist & l, const gist & r)		{ OP0(l, r, ^) }
gist operator ^(const gist & l, int r)			{ OP1(l, r, ^) }
gist operator ^(const gist & l, unsigned r)		{ OP1(l, r, ^) }
gist operator ^(const gist & l, long r)			{ OP1(l, r, ^) }
gist operator ^(const gist & l, unsigned long r)	{ OP1(l, r, ^) }
gist operator ^(int l, const gist & r)			{ OP2(l, r, ^) }
gist operator ^(unsigned l, const gist & r)		{ OP2(l, r, ^) }
gist operator ^(long l, const gist & r)			{ OP2(l, r, ^) }
gist operator ^(unsigned long l, const gist & r)	{ OP2(l, r, ^) }

/********************/

gist operator &(const gist & l, const gist & r)		{ OP0(l, r, &) }
gist operator &(const gist & l, int r)			{ OP1(l, r, &) }
gist operator &(const gist & l, unsigned r)		{ OP1(l, r, &) }
gist operator &(const gist & l, long r)			{ OP1(l, r, &) }
gist operator &(const gist & l, unsigned long r)	{ OP1(l, r, &) }
gist operator &(int l, const gist & r)			{ OP2(l, r, &) }
gist operator &(unsigned l, const gist & r)		{ OP2(l, r, &) }
gist operator &(long l, const gist & r)			{ OP2(l, r, &) }
gist operator &(unsigned long l, const gist & r)	{ OP2(l, r, &) }

/********************/

gist operator |(const gist & l, const gist & r)		{ OP0(l, r, |) }
gist operator |(const gist & l, int r)			{ OP1(l, r, |) }
gist operator |(const gist & l, unsigned r)		{ OP1(l, r, |) }
gist operator |(const gist & l, long r)			{ OP1(l, r, |) }
gist operator |(const gist & l, unsigned long r)	{ OP1(l, r, |) }
gist operator |(int l, const gist & r)			{ OP2(l, r, |) }
gist operator |(unsigned l, const gist & r)		{ OP2(l, r, |) }
gist operator |(long l, const gist & r)			{ OP2(l, r, |) }
gist operator |(unsigned long l, const gist & r)	{ OP2(l, r, |) }

/********************/

gist operator <<(const gist & l, const gist & r)	{ OP0(l, r, <<) }
gist operator <<(const gist & l, int r)			{ OP1(l, r, <<) }
gist operator <<(const gist & l, unsigned r)		{ OP1(l, r, <<) }
gist operator <<(const gist & l, long r)		{ OP1(l, r, <<) }
gist operator <<(const gist & l, unsigned long r)	{ OP1(l, r, <<) }
gist operator <<(int l, const gist & r)			{ OP2(l, r, <<) }
gist operator <<(unsigned l, const gist & r)		{ OP2(l, r, <<) }
gist operator <<(long l, const gist & r)		{ OP2(l, r, <<) }
gist operator <<(unsigned long l, const gist & r)	{ OP2(l, r, <<) }

/********************/

gist operator >>(const gist & l, const gist & r)	{ OP0(l, r, >>) }
gist operator >>(const gist & l, int r)			{ OP1(l, r, >>) }
gist operator >>(const gist & l, unsigned r)		{ OP1(l, r, >>) }
gist operator >>(const gist & l, long r)		{ OP1(l, r, >>) }
gist operator >>(const gist & l, unsigned long r)	{ OP1(l, r, >>) }
gist operator >>(int l, const gist & r)			{ OP2(l, r, >>) }
gist operator >>(unsigned l, const gist & r)		{ OP2(l, r, >>) }
gist operator >>(long l, const gist & r)		{ OP2(l, r, >>) }
gist operator >>(unsigned long l, const gist & r)	{ OP2(l, r, >>) }
