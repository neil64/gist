/*
 *	Test gist.
 */

#include	<stdio.h>
#include	<assert.h>
#include	<stdlib.h>

#include	"gist.h"


void
test_int()
{
	gist a(123);
	gist b = 456;

	assert((int)a == 123);
	assert((int)b == 456);
	b = 454;
	assert((int)(a + b) == 577);
}


void
test_float()
{
	gist a(123.0);
	gist pi(3.142);

	assert((double)(a + pi) == 126.142);
}


void
report(const char * ty, gist::error & e)
{
	printf("gist %s error", ty);
	if (e.msg)
		printf(": %s", e.msg);
	printf("\n");
	exit(1);
}


int
main(int argc, char ** argv)
{
	try
	{
		test_int();
		test_float();
	}
	catch (gist::valueError e)
	{
		report("value", e);
	}
	catch (gist::typeError e)
	{
		report("type", e);
	}
	catch (gist::indexError e)
	{
		report("index", e);
	}
	catch (gist::overflowError e)
	{
		report("overflow", e);
	}
	catch (gist::internalError e)
	{
		report("internal", e);
	}
	catch (...)
	{
		printf("unknown exception\n");
		exit(1);
	}

	return 0;
}