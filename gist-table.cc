/*
 *	Gist -- Tables.
 */

#include	<stdio.h>
#include	<string.h>
#include	<memory.h>
#include	<stdlib.h>
#include	<errno.h>

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

/*
 *	Create a new table, or clear the current table.
 */
gist &
gist::table(bool clear)
{
	if (typ != GT_TABLE || clear)
	{
		giTable * ap = new giTable;

		typ = GT_TABLE;
		intern = ap;
	}

	return *this;
}

/**********************************************************************/

gist &
gist::_tableindex(const gist & i, bool make)
{

#if 1

	/*
	 *	Given that C++ will not necessarily use the `const' version
	 *	of the subscript operator when an r-value is required, we
	 *	cannot rely on getting an exception when a table element
	 *	doesn't exist and only an r-value is required.	It is
	 *	possible someone could write code that works on one compiler
	 *	and not on another, which is not a situation that I like.
	 *	So, `make' is ignored, and we always build a new element when
	 *	`i' doesn't match, even when the subscript is an r-value.
	 */
	giTable * tp = (giTable *)intern;
	gistKey * k = tp->index.search(i, true);
	if (!k)
		throw internalError("table entry was not built as "
						"it should have been");
	return k->val;

#else // 1

	/*
	 *	This is how it should be;  see above.
	 */
	giTable * tp = (giTable *)intern;
	gistKey * k = tp->index.search(i, make);
	if (!k)
		throw indexError("table index does not exist");
	return k->val;

#endif // 1

}

/********************/

gist &
gist::_tableindex(long i, bool make)
{
	gist ix = i;
	return _tableindex(ix, make);
}


gist &
gist::_tableindex(double i, bool make)
{
	gist ix = i;
	return _tableindex(ix, make);
}


gist &
gist::_tableindex(const char * i, bool make)
{
	gist ix = i;
	return _tableindex(ix, make);
}
