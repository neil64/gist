/*
 *	Test gist.
 */

#include	<stdio.h>
#include	<assert.h>
#include	<stdlib.h>

#include	"gist.h"


gist g1, g2;
gist ga[10] =
{
	1, 2, 3, "abc", 4.5
};

const char *	ascii = " !\"#$%&'()*+,-./0123456789:;<=>?"
			"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
			"`abcdefghijklmnopqrstuvwxyz{|}~";


void
printGist(gist & g)
{
	switch (g.type())
	{
	case gist::GT_NIL:
		printf("nil");
		break;

	case gist::GT_STR:
		printf("str = %s", (const char *)g);
		break;

	case gist::GT_ARRAY:
		printf("array");
		break;

	case gist::GT_TABLE:
		printf("table");
		break;

	case gist::GT_CODE:
		printf("code");
		break;

	case gist::GT_INT:
		printf("int = %ld", (long)g);
		break;

	case gist::GT_FLOAT:
		printf("float = %g", (double)g);
		break;

	case gist::GT_LONG:
		printf("long");
		break;

	case gist::GT_REAL:
		printf("real");
		break;

	default:
		printf("huh?");
		break;
	}
}


void
test_init()
{
	unsigned i;

	printf("init1:\n");

	for (i = 0; i < sizeof ga / sizeof ga[0]; i++)
	{
		printf("\t0: ");
		printGist(ga[i]);
		printf("\n");
	}
}


void
test_nil()
{
	gist a;
	gist b(1234);

	printf("nil1:\n");

	assert(a.isNil());
	assert(!b.isNil());
	b.clear();
	assert(b.isNil());
}


void
test_int()
{
	printf("int1:\n");

	gist a(123);
	gist b = 456;
	gist c;  c.set(789);
	gist d(b);

	printf("int2:\n");

	assert((int)a == 123);
	assert((int)b == 456);
	assert((int)c == 789);
	assert((int)(c - 790) == -1);
	assert(b == d);
	assert(d == 456);
	assert((unsigned)d == 456);
	b = 454;
	assert((int)(a + b) == 577);
	printf("\ta-b = %s (-331)\n", (const char *)((a - b).toString()));
	printf("\t-700 + c = %d (89)\n", (int)(-700 + c));

	printf("int3:\n");

	a = "0x1000";
	printf("\t%s = %ld\n", (const char *)a, a.toInt());
	a = "z00";
	printf("\t%s = %ld\n", (const char *)a, a.toInt(36));
	a = "11110100001001000000";
	printf("\t%s = %ld\n", (const char *)a, a.toInt(2));

	printf("int4:\n");

	a = 1;
	int p, q;

	p = a;
	q = a++;
	printf("\ta = %d, a++ = %d, a = %d\n", p, q, (int)a);
	assert(q == 1);
	assert(a == 2);

	p = a;
	q = ++a;
	printf("\ta = %d, ++a = %d, a = %d\n", p, q, (int)a);
	assert(q == 3);
	assert(a == 3);

	p = a;
	q = a--;
	printf("\ta = %d, a-- = %d, a = %d\n", p, q, (int)a);
	assert(q == 3);
	assert(a == 2);

	p = a;
	q = --a;
	printf("\ta = %d, --a = %d, a = %d\n", p, q, (int)a);
	assert(q == 1);
	assert(a == 1);

	b = -a;
	printf("\ta = %d, -a = %d\n", (int)a, (int)b);
	assert(b == -1);

	a = 0xf0e1a537;
	b = ~a;
	printf("\ta = %#x, ~a = %#x\n", (int)a, (int)b);
	assert(b == 0x0f1e5ac8);

	b = !a;
	printf("\ta = %#x, !a = %#x\n", (int)a, (int)b);
	assert(b == 0);

	printf("int5:\n");

	a = 5678;
	b = 1234;
	c = 4;
	int x = 5678;
	int y = 1234;
	int z = 4;
	int i;

	i = a+b;	printf("\ta+b = %d (6912)\n", i);    assert(i==6912);
	i = a+y;	printf("\ta+y = %d (6912)\n", i);    assert(i==6912);
	i = x+b;	printf("\tx+b = %d (6912)\n", i);    assert(i==6912);

	i = a-b;	printf("\ta-b = %d (4444)\n", i);   assert(i==4444);
	i = a-y;	printf("\ta-y = %d (4444)\n", i);   assert(i==4444);
	i = x-b;	printf("\tx-b = %d (4444)\n", i);   assert(i==4444);

	i = a*b;	printf("\ta*b = %d (7006652)\n", i); assert(i==7006652);
	i = a*y;	printf("\ta*y = %d (7006652)\n", i); assert(i==7006652);
	i = x*b;	printf("\tx*b = %d (7006652)\n", i); assert(i==7006652);

	i = a/b;	printf("\ta/b = %d (4)\n", i);    assert(i==4);
	i = a/y;	printf("\ta/y = %d (4)\n", i);    assert(i==4);
	i = x/b;	printf("\tx/b = %d (4)\n", i);    assert(i==4);

	i = a%b;	printf("\ta%%b = %d (742)\n", i);    assert(i==742);
	i = a%y;	printf("\ta%%y = %d (742)\n", i);    assert(i==742);
	i = x%b;	printf("\tx%%b = %d (742)\n", i);    assert(i==742);

	i = a^b;	printf("\ta^b = %d (4860)\n", i);    assert(i==4860);
	i = a^y;	printf("\ta^y = %d (4860)\n", i);    assert(i==4860);
	i = x^b;	printf("\tx^b = %d (4860)\n", i);    assert(i==4860);

	i = a&b;	printf("\ta&b = %d (1026)\n", i);    assert(i==1026);
	i = a&y;	printf("\ta&y = %d (1026)\n", i);    assert(i==1026);
	i = x&b;	printf("\tx&b = %d (1026)\n", i);    assert(i==1026);

	i = a|b;	printf("\ta|b = %d (5886)\n", i);    assert(i==5886);
	i = a|y;	printf("\ta|y = %d (5886)\n", i);    assert(i==5886);
	i = x|b;	printf("\tx|b = %d (5886)\n", i);    assert(i==5886);

	i = a<<c;	printf("\ta<<c = %d (90848)\n", i);  assert(i==90848);
	i = a<<z;	printf("\ta<<z = %d (90848)\n", i);  assert(i==90848);
	i = x<<c;	printf("\tx<<c = %d (90848)\n", i);  assert(i==90848);

	i = a>>c;	printf("\ta>>c = %d (354)\n", i);  assert(i==354);
	i = a>>z;	printf("\ta>>z = %d (354)\n", i);  assert(i==354);
	i = x>>c;	printf("\tx>>c = %d (354)\n", i);  assert(i==354);
}


static inline const char *
cb(bool x)
{
	return x ? "true" : "false";
}


void
test_bool()
{
	printf("bool1:\n");

	gist zero(false);
	gist one = true;
	bool r;

	r = one==zero;	printf("\t1!=0: %s (false)\n", cb(r));	assert(!r);

	printf("bool2:\n");

	gist a = 123;
	gist b(456);
	gist c;
	c.set(456);
	int x = 123;
	int y = 456;
	gist f(123.4);
	gist g(567.8);
	gist h(567.8);
	double l = 123.4;
	double m = 567.8;

	printf("\t(bool)a = %s\n", cb((bool)a));

	printf("bool3:\n");

	r = b==c;	printf("\tb==c: %s (true)\n", cb(r));	assert(r);
	r = b==y;	printf("\tb==y: %s (true)\n", cb(r));	assert(r);
	r = y==c;	printf("\ty==c: %s (true)\n", cb(r));	assert(r);

	r = g==h;	printf("\tg==h: %s (true)\n", cb(r));	assert(r);
	r = g==m;	printf("\tg==m: %s (true)\n", cb(r));	assert(r);
	r = m==h;	printf("\tm==h: %s (true)\n", cb(r));	assert(r);

	r = a!=c;	printf("\ta!=c: %s (true)\n", cb(r));	assert(r);
	r = a!=y;	printf("\ta!=y: %s (true)\n", cb(r));	assert(r);
	r = x!=c;	printf("\tx!=c: %s (true)\n", cb(r));	assert(r);

	r = f!=h;	printf("\tf!=h: %s (true)\n", cb(r));	assert(r);
	r = f!=m;	printf("\tf!=m: %s (true)\n", cb(r));	assert(r);
	r = l!=h;	printf("\tl!=h: %s (true)\n", cb(r));	assert(r);

	r = a<c;	printf("\ta<c: %s (true)\n", cb(r));	assert(r);
	r = a<y;	printf("\ta<y: %s (true)\n", cb(r));	assert(r);
	r = x<c;	printf("\tx<c: %s (true)\n", cb(r));	assert(r);

	r = f<h;	printf("\tf<h: %s (true)\n", cb(r));	assert(r);
	r = f<m;	printf("\tf<m: %s (true)\n", cb(r));	assert(r);
	r = l<h;	printf("\tl<h: %s (true)\n", cb(r));	assert(r);

	r = a>c;	printf("\ta>c: %s (false)\n", cb(r));	assert(!r);
	r = a>y;	printf("\ta>y: %s (false)\n", cb(r));	assert(!r);
	r = x>c;	printf("\tx>c: %s (false)\n", cb(r));	assert(!r);

	r = f>h;	printf("\tf>h: %s (false)\n", cb(r));	assert(!r);
	r = f>m;	printf("\tf>m: %s (false)\n", cb(r));	assert(!r);
	r = l>h;	printf("\tl>h: %s (false)\n", cb(r));	assert(!r);

	r = a<=c;	printf("\ta<=c: %s (true)\n", cb(r));	assert(r);
	r = a<=y;	printf("\ta<=y: %s (true)\n", cb(r));	assert(r);
	r = x<=c;	printf("\tx<=c: %s (true)\n", cb(r));	assert(r);

	r = f<=h;	printf("\tf<=h: %s (true)\n", cb(r));	assert(r);
	r = f<=m;	printf("\tf<=m: %s (true)\n", cb(r));	assert(r);
	r = l<=h;	printf("\tl<=h: %s (true)\n", cb(r));	assert(r);

	r = a>=c;	printf("\ta>=c: %s (false)\n", cb(r));	assert(!r);
	r = a>=y;	printf("\ta>=y: %s (false)\n", cb(r));	assert(!r);
	r = x>=c;	printf("\tx>=c: %s (false)\n", cb(r));	assert(!r);

	r = f>=h;	printf("\tf>=h: %s (false)\n", cb(r));	assert(!r);
	r = f>=m;	printf("\tf>=m: %s (false)\n", cb(r));	assert(!r);
	r = l>=h;	printf("\tl>=h: %s (false)\n", cb(r));	assert(!r);
}


void
test_assign()
{
	printf("assign1:\n");
}


void
test_float()
{
	printf("float1:\n");

	gist a(123.0);
	gist b = 0.0;
	double pi0 = 3.1415926535897932384626433832795029L;
	gist pi1(3.142);
	gist pi(3.1415926535897932384626433832795029L);
	gist s = "0x1.921fb54442d18p+1";

	printf("float2:\n");

	assert((double)(a + pi1) == 126.142);
	printf("\t\"exact\" pi  = %la\n", pi0);
	printf("\tconstant pi = %.30g\n", pi0);
	printf("\tgist pi     = %.30g\n", (double)s);
	printf("\tpi.toString = %s\n", (const char *)pi.toString());
	assert((double)pi == (double)s);
	assert(pi == s);

	printf("float3:\n");

	a = 122.0;
	b = 30.5;
	double x = 122.0;
	double y = 30.5;
	double i;

	i = a+b;	printf("\ta+b = %g (152.5)\n", i);  assert(i==152.5);
	i = a+y;	printf("\ta+y = %g (152.5)\n", i);  assert(i==152.5);
	i = x+b;	printf("\tx+b = %g (152.5)\n", i);  assert(i==152.5);

	i = a-b;	printf("\ta-b = %g (91.5)\n", i);   assert(i==91.5);
	i = a-y;	printf("\ta-y = %g (91.5)\n", i);   assert(i==91.5);
	i = x-b;	printf("\tx-b = %g (91.5)\n", i);   assert(i==91.5);

	i = a*b;	printf("\ta*b = %g (3721)\n", i); assert(i==3721);
	i = a*y;	printf("\ta*y = %g (3721)\n", i); assert(i==3721);
	i = x*b;	printf("\tx*b = %g (3721)\n", i); assert(i==3721);

	i = a/b;	printf("\ta/b = %g (4)\n", i);   assert(i==4);
	i = a/y;	printf("\ta/y = %g (4)\n", i);   assert(i==4);
	i = x/b;	printf("\tx/b = %g (4)\n", i);   assert(i==4);

	printf("float4:\n");

	a = 1.5;
	double p, q;

	p = a;
	q = a++;
	printf("\ta = %g, a++ = %g, a = %g\n", p, q, (double)a);
	assert(q == 1.5);
	assert(a == 2.5);

	p = a;
	q = ++a;
	printf("\ta = %g, ++a = %g, a = %g\n", p, q, (double)a);
	assert(q == 3.5);
	assert(a == 3.5);

	p = a;
	q = a--;
	printf("\ta = %g, a-- = %g, a = %g\n", p, q, (double)a);
	assert(q == 3.5);
	assert(a == 2.5);

	p = a;
	q = --a;
	printf("\ta = %g, --a = %g, a = %g\n", p, q, (double)a);
	assert(q == 1.5);
	assert(a == 1.5);

	a = 1.5;
	b = -a;
	printf("\ta = %g, -a = %g\n", (double)a, (double)b);
	assert(b == -1.5);

	b = !a;
	printf("\ta = %g, !a = %g\n", (double)a, (double)b);
	assert(b == 0);
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
	printf("\t(s+t).toString = %s\n", s.toString().CCS());

	printf("string3:\n");

	int i;
	s = "";
	for (i = 32; i < 126; i++)
		s.strcat(i);
	const char * cp = s.CCS();
	printf("\tstrlen(s):  %d, %d, %d\n", s.strlen(), strlen(s), strlen(cp));
	printf("\ts: %.64s\n\t   %.64s\n", cp, cp+64);
	// printf("\ts: %.64s\n", s.CCS());
	// printf("\ts: %.64s\n", s.CCS() + 64);

	s = "";
	for (i = 32; i < 126; i += 2)
	{
		char a[3] = { i, i+1, 0 };
		strcat(s, a);
	}
	cp = s.CCS();
	printf("\tstrlen(s):  %d, %d, %d\n", s.strlen(), strlen(s), strlen(cp));
	printf("\ts: %.64s\n\t   %.64s\n", cp, cp+64);
}


void
test_array()
{
	printf("array1:\n");

	gist a;
	a.array(10);

	a[9] = 9;
	a[5] = "5";
	a[0] = 0;
	a[1] = "one";
	a[2].array(5)[3] = "2/3";

	printf("\tarray length = %d (%d)\n", a.len(), len(a));

	for (unsigned i = 0; i < a.len(); i++)
	{
		printf("\t%2d: ", i);
		printGist(a[i]);
		printf("\n");
	}

	a.array(16);
	a[15] = "fifteen";
	gist s = a.pop();
	printf("\ta[15] = %s\n", s.CCS());
	printf("\ta.len = %d\n", a.len());

	a.array(16);
	printf("\tnew element 16 = ");
	printGist(a[15]);
	printf("\n");
}


void
test_table()
{
	printf("table1:\n");
}


#if 0
void
test_pickle()
{
	printf("pickle1:\n");

	gist::pickle pik;
	gist hw = "Hello world!";
	gist str;

	pik.set(hw);
	pik.out(str);

	printf("\tpickled hello world = '%s'\n", str.CCS());
}
#endif


void
test_misc()
{
	printf("misc1:\n");

	printf("\tnot testing copy(gist)\n");
	// .... ?.copy(const gist &);
	// .... ?.copy(const gist *);
}


void
report(const char * ty, gist::error & e)
{
	printf("\n-------->> gist %s error", ty);
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
		test_init();
		test_nil();
		test_int();
		test_bool();
		test_assign();
		test_float();
		test_string();
		test_array();
		test_table();
		// test_pickle();
		test_misc();
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
