/*
 *	Gist -- Operators part 4.
 *
 *	Contains:	
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

#define OP1(i)								\
	if (typ == GT_ARRAY)						\
		return _arrayindex((long)i);				\
	else if (typ == GT_TABLE)					\
		return _tableindex((long)i);				\
	else if (typ == GT_NIL)						\
		return _makearray((long)i);				\
	else								\
		throw typeError("subscript not allowed");


#define OP2(i)								\
	if (typ == GT_TABLE)						\
		return _tableindex((long)i);				\
	else if (typ == GT_ARRAY)					\
		throw indexError("array index must be integer");	\
	else								\
		throw typeError("subscript not allowed");

/**********************************************************************/


gist & gist::operator [](int i)				{ OP1(i) }
gist & gist::operator [](unsigned i)			{ OP1(i) }
gist & gist::operator [](long i)			{ OP1(i) }
gist & gist::operator [](unsigned long i)		{ OP1(i) }
gist & gist::operator [](long long i)			{ OP1(i) }
gist & gist::operator [](unsigned long long i)		{ OP1(i) }

gist & gist::operator [](float i)			{ OP2(i) }
gist & gist::operator [](double i)			{ OP2(i) }
gist & gist::operator [](const char * i)		{ OP2(i) }

gist &
gist::operator [](const gist & i)
{
	if (typ == GT_ARRAY)
	{
		if (i.typ == GT_INT)
			return _arrayindex(i.val);
		throw indexError("array index must be integer");
	}
	if (typ == GT_TABLE)
		return _tableindex(i);
	if (typ == GT_NIL)
	{
		if (i.typ == GT_INT)
			return _makearray(i.val);
		throw indexError("array index must be integer");
	}

	throw typeError("subscript not allowed");
}
