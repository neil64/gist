/*
 *	Test gist.
 */

#include	<stdio.h>
#include	<assert.h>
#include	<stdlib.h>

#include	"gist.h"


gist g1, g2;


void
test_int()
{
	printf("int1:\n");

	gist a(123);
	gist b = 456;

	printf("int1:\n");

	assert((int)a == 123);
	assert((int)b == 456);
	b = 454;
	assert((int)(a + b) == 577);
}


void
test_float()
{
	printf("float1:\n");

	gist a(123.0);
	double pi0 = 3.1415926535897932384626433832795029L;
	gist pi1(3.142);
	gist pi(3.1415926535897932384626433832795029L);
	gist s = "0x1.921fb54442d18p+1";

	printf("float2:\n");

	assert((double)(a + pi1) == 126.142);
	printf("\t\"exact\" pi  = %la\n", pi0);
	printf("\tconstant pi = %.30g\n", pi0);
	printf("\tgist pi     = %.30g\n", (double)s);
	assert((double)pi == (double)s);
	assert(pi == s);
}


void
test_string()
{
	printf("string1:\n");

	gist s = "abc";
	s += "def";
	assert(s == "abcdef");
	printf("\tabcdef = %s\n", (const char *)s);

	printf("string2:\n");

	s = "01234567890123456789";
	gist t("abcdefghijklmnopqrstuvwxyz");
	s += t;
	printf("\ts+t = %s\n", (const char *)s);
}


void
test_subscript()
{
	gist a = 123;
	gist b = a[1.0f];

	printf("b = %d\n", (int)b);
}


void
report(const char * ty, gist::error & e)
{
	printf("\n\tgist %s error", ty);
	if (e.msg)
		printf(": %s", e.msg);
	printf("\n");
	exit(1);
}


int
main(int argc, char ** argv)
{
	printf("Test gist\n\n");
	printf("\tSize of gist = %d\n", sizeof (gist));

	g1 = g2;

	try
	{
		test_int();
		test_float();
		test_string();
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
	catch (gist::notYetError e)
	{
		report("not yet", e);
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
}
