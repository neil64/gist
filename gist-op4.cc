/*
 *	Gist -- Operators part 4.
 *
 *	Contains:	[]
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

#define OP1(i, m)							\
	if (typ == GT_ARRAY)						\
		return ((gist *)this)->_arrayindex((long)i);		\
	else if (typ == GT_TABLE)					\
		return ((gist *)this)->_tableindex((long)i, m);		\
	else								\
		throw typeError("subscript not allowed");


#define OP2(i, m)							\
	if (typ == GT_TABLE)						\
		return ((gist *)this)->_tableindex(i, m);		\
	else if (typ == GT_ARRAY)					\
		throw indexError("array index must be integer");	\
	else								\
		throw typeError("subscript not allowed");

/**********************************************************************/

gist & gist::operator [](int i)				{ OP1(i, true) }
gist & gist::operator [](unsigned i)			{ OP1(i, true) }
gist & gist::operator [](long i)			{ OP1(i, true) }
gist & gist::operator [](unsigned long i)		{ OP1(i, true) }
gist & gist::operator [](long long i)			{ OP1(i, true) }
gist & gist::operator [](unsigned long long i)		{ OP1(i, true) }

gist & gist::operator [](float i)			{ OP2(i, true) }
gist & gist::operator [](double i)			{ OP2(i, true) }
gist & gist::operator [](const char * i)		{ OP2(i, true) }

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
	{
		switch (i.typ)
		{
		case GT_NIL:
		case GT_STR:
		case GT_INT:
		case GT_FLOAT:
		case GT_PTR:
			break;

		default:
			throw indexError("table index must be nil, "
						"str, int, float or ptr");
		}

		return _tableindex(i, true);
	}

	throw typeError("subscript not allowed");
}

/********************/

const gist & gist::operator [](int i) const		{ OP1(i, false) }
const gist & gist::operator [](unsigned i) const	{ OP1(i, false) }
const gist & gist::operator [](long i) const		{ OP1(i, false) }
const gist & gist::operator [](unsigned long i) const	{ OP1(i, false) }
const gist & gist::operator [](long long i) const	{ OP1(i, false) }
const gist & gist::operator [](unsigned long long i) const { OP1(i, false) }

const gist & gist::operator [](float i) const		{ OP2(i, false) }
const gist & gist::operator [](double i) const		{ OP2(i, false) }
const gist & gist::operator [](const char * i) const	{ OP2(i, false) }

const gist &
gist::operator [](const gist & i) const
{
	if (typ == GT_ARRAY)
	{
		if (i.typ == GT_INT)
			return ((gist *)this)->_arrayindex(i.val);
		throw indexError("array index must be integer");
	}
	if (typ == GT_TABLE)
	{
		switch (i.typ)
		{
		case GT_NIL:
		case GT_STR:
		case GT_INT:
		case GT_FLOAT:
		case GT_PTR:
			break;

		default:
			throw indexError("table index must be nil, "
						"str, int, float or ptr");
		}

		return ((gist *)this)->_tableindex(i, false);
	}

	throw typeError("subscript not allowed");
}
