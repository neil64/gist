/*
 *	Gist -- Operators part 5.
 *
 *	Contains:	u- ~ ! ++ --
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist
gist::operator -() const
{
	gist x = *this;

	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	case GT_STR:
	default:
		throw typeError("unary -");

	case GT_INT:
		x.val = -x.val;
		break;

	case GT_FLOAT:
		x.dval = -x.dval;
		break;
	}

	return x;
}


gist
gist::operator ~() const
{
	gist x = *this;

	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	case GT_STR:
	case GT_FLOAT:
	default:
		throw typeError("operator ~");

	case GT_INT:
		x.val = ~x.val;
		break;
	}

	return x;
}


gist
gist::operator !() const
{
	gist x = *this;

	switch (type())
	{
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		throw typeError("operator !");

	case GT_STR:
		if (cnt > 0)
			x.clear();
		else
	case GT_NIL:
			x = 1;
		break;

	case GT_INT:
		x.val = !x.val;
		break;

	case GT_FLOAT:
		x.dval = !x.dval;
		break;
	}

	return x;
}


gist &
gist::operator ++()
{
	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	case GT_STR:
	default:
		throw typeError("pre-increment");

	case GT_INT:
		val++;
		break;

	case GT_FLOAT:
		dval++;
		break;
	}

	return *this;
}


gist
gist::operator ++(int)
{
	gist x = *this;

	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	case GT_STR:
	default:
		throw typeError("post-increment");

	case GT_INT:
		val++;
		break;

	case GT_FLOAT:
		dval++;
		break;
	}

	return x;
}


gist &
gist::operator --()
{
	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	case GT_STR:
	default:
		throw typeError("pre-decrement");

	case GT_INT:
		val--;
		break;

	case GT_FLOAT:
		dval--;
		break;
	}

	return *this;
}


gist
gist::operator --(int)
{
	gist x = *this;

	switch (type())
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	case GT_STR:
	default:
		throw typeError("post-decrement");

	case GT_INT:
		val--;
		break;

	case GT_FLOAT:
		dval--;
		break;
	}

	return x;
}
