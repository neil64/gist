/*
 *	Gist -- Pickler.
 */


#include	"gist.h"
#include	"gist-internal.h"

#if 0			// was in gist.h

	/****************************************/
	/*
	 *	Pickler.
	 */
	struct pickle
	{
		pickle();
		pickle(const gist &);

		void		set(const gist &);
		gist		get();

		gist &		in(const char *);
		gist &		in(const gist &);

		const char *	out();
		unsigned	out(const char ** vec, unsigned vsz = 1);
		unsigned	out(char * buf, unsigned bsz);
		unsigned	out(gist & str);
		unsigned	outlen();

		void		reset(bool newLines);

		void		pretty(bool);

	    private:
		struct pickleInternal * ptr;
	};
#endif


/*
 *	Maximum column for output formatting (must be < 128).  This is
 *	typically less than 80, as 80 is historically the width of most
 *	dumb terminals.	 It doesn't mean too much these days, but it helps
 *	make the output more readible, should a human need to read it.
 */
const int MaxCol = 78;


/*
 *	The internal picking control structure.
 */
struct pickleInternal : gistInternal
{
	/*
	 *	The value we are pickling or unpickling.
	 */
	gist		value;

	/*
	 *	Stuff we need to nagivage through the value.
	 */
	gist		list;
	unsigned	li;
	gist		stack;

	/*
	 *	The input parser details.
	 */
	gist		pData;		// Collected data as it's parsed.
	enum {
	    P_TOP,
	    P_IDLE,
	    P_STR,
	    P_STR1,
	    P_STR2,
	    P_STR3,
	    P_STR5,
	    P_STR6,
	    P_BASE85,
	    P_NAME }	pState;		// Parser state.
	signed char	pChr;		// Temporary character storage.
	unsigned char	pB85;		// Base-85 character count.
	char		pB85c[5];	// Base-85 collected characters.
	char		pQuote;		// The kind of quote used for a string.
	unsigned	pLevel;		// Parser list depth.
	gist		pExcess;	// What's left after parsing.

	/*
	 *	Output details.
	 */
	gist		oData;		// String chunk currently being output.
	int		oLevel;		// List depth currently being output.
	short		oDone;		// Output formatting complete flag.
	int		ofLevel;	// Formatting indent level.
	int		ofColumn;	// Current output column.
	gist		ofIndent;	// List of indents for each level.

	/*
	 *	Support functions.
	 */
	int		in1(int c);
	int		in2(gist &);
	int		in3();
	// void		add1(node_t *);		// Node insert worker
	// void		rem1(node_t *&, list_t *&, node_t *);
	// void		clr1(node_t *);
	void		reset();

	void		add(gist &);
	void		addList();
	void		up();
};

/**********************************************************************/

void
pickleInternal::add(gist & v)
{
	list.push(v);
	li = list.len();
}


void
pickleInternal::addList()
{
	gist a;
	list.push(a.array(0));
	stack.push(list);
	list = a;
	li = 0;
}


void
pickleInternal::up()
{
	if (stack.len() > 0)
	{
		list = stack.pop();
		li = list.len();
	}
}

/**********************************************************************/

/*
 *	The following base-85 encoding works the same as that found in
 *	PostScript, except the character set used is altered so as to
 *	use only legal name characters (characters for which `isname()'
 *	returns true).  The '~' character is used instead of 'z' to
 *	represent zero, and '`' is used as the terminator character
 *	instead of the sequence '~>'.  '`' is also used as the start
 *	character;  that is, base-85 strings are enclosed in grave
 *	quotes (`....base-85....`).  On input, illegal base-85 sequences
 *	are simply ignored.
 */


/*
 *	Used for both `isname()' and `b85_to_bin'.  -1 is legal in a
 *	name but illegal in base-85;  -2 is illegal anywhere.
 */
static signed char	map0[] =
{
	 -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,		// 0x00
	 -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,		// 0x08
	 -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,		// 0x10
	 -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,		// 0x18
	 -2,   0,  -2,   1,   2,   3,   4,  -2,		// 0x20	  !"#$%&'
	 -2,  -2,   5,   6,   7,   8,   9,  10,		// 0x28  ()*+,-./
	 11,  12,  13,  14,  15,  16,  17,  18,		// 0x30  01234567
	 19,  20,  21,  22,  23,  24,  25,  26,		// 0x38  89:;<=>?
	 27,  28,  29,  30,  31,  32,  33,  34,		// 0x40  @ABCDEFG
	 35,  36,  37,  38,  39,  40,  41,  42,		// 0x48  HIJKLMNO
	 43,  44,  45,  46,  47,  48,  49,  50,		// 0x50  PQRSTUVW
	 51,  52,  53,  54,  -2,  55,  56,  57,		// 0x58  XYZ[\]^_
	 -2,  58,  59,  60,  61,  62,  63,  64,		// 0x60  `abcdefg
	 65,  66,  67,  68,  69,  70,  71,  72,		// 0x68  hijklmno
	 73,  74,  75,  76,  77,  78,  79,  80,		// 0x70  pqrstuvw
	 81,  82,  83,  84,  -1,  -1,  -1,  -2,		// 0x78  xyz{|}~
};

/*
 *	Used for `bin_to_b85()'.
 */
static char map1[] = "!#$%&*+,-./012345678"
		     "9:;<=>?@ABCDEFGHIJKL"
		     "MNOPQRSTUVWXYZ[]^_ab"
		     "cdefghijklmnopqrstuv"
		     "wxyz{";


/*
 *	`cnt' bytes from `f' are converted to base-85 and stored in the
 *	string pointed to by `t'.  `t' must be at least 5 characters large.
 *	The number of characters actually converted is returned.  `f' is
 *	a big-endian" representation of the data to be output;  if there
 *	are less than 4 bytes to convert, the valid data is assumed to be
 *	in the most significant bytes of `f'.
 */
static int
bin_to_b85(int cnt, unsigned long f, char * t)
{
	if (cnt == 4 && f == 0)
	{
		t[0] = '~';
		return 1;
	}

	t[4] = map1[f % 85];		f /= 85;
	t[3] = map1[f % 85];		f /= 85;
	t[2] = map1[f % 85];		f /= 85;
	t[1] = map1[f % 85];		f /= 85;
	t[0] = map1[f % 85];

	return cnt + 1;
}


/*
 *	Convert a base-85 string to binary.  `cnt' is the number of
 *	characters to parse from `f'.  The result is stored in `t'.
 *	If there are less than 5 characters `t' will contain the bytes
 *	available shifted to the most significant bytes.  The number
 *	of bytes converted is returned.  Zero is returned on error.
 *	The caller should parse '~';  it is not parsed here.
 */
static int
b85_to_bin(int cnt, char * f, unsigned long * t)
{
	unsigned long	n = 0;
	int		i;

	if (cnt <= 1)
		return 0;

	i = 0;
	do
	{
		unsigned long x = n * 85;
		if (x < n)	// result is greater than 2^32
			return 0;

		int c = *f++;
		if (c >= 0 && c <= 0x7f)
		{
			c = map0[c];
			if (c < 0)
				return 0;
			n = x + c;
		}
		else
			return 0;
		i++;
	} while (i < cnt);

	if (cnt < 5)
	{
		if (cnt == 2)
		{
			n = n * (85*85*85) + 0xffffff;
			n &= 0xff000000;
		}
		else if (cnt == 3)
		{
			n = n * (85*85) + 0xffff;
			n &= 0xffff0000;
		}
		else
		{
			n = n * 85 + 0xff;
			n &= 0xffffff00;
		}
	}

	*t = n;
	return cnt - 1;
}


#if 0

#define N 4

int
main(int ac, char ** av)
{
	int i;

	for (i = 0; i < (1 << (8 * (N-1))); i++)
	{
		unsigned long l = i << 24;
		unsigned long r;
		char b[5];
		int c = bin_to_b85(N-1, l, b);
		if (c != N)
		{
			printf("wrong chars\n");
			return 1;
		}
		b85_to_bin(N, b, &r);
		// printf("%6d  %5.*s  %08lx, %08lx\n", i, 5, b, l, r);
		if (l != r)
		{
			printf("not equal at %d (%lx, %lx)\n", i, l, r);
			return 1;
		}
	}

	return 0;
}

#endif


static int
isname(const int c)
{
	return (c >= 0  &&  c <= 0x7f  &&  map0[c] >= -1);
}

/**********************************************************************/

static int
hex(int c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return (c - 'a') + 10;
	if (c >= 'A' && c <= 'F')
		return (c - 'F') + 10;
	return -1;
}


/*
 *	Process a single character through the input state machine.
 *	Returns 0 if the result is not a complete token, or is not at the
 *	top level;  return 1 if we completed a token at the top level --
 *	that is, we completed a full list.
 */
int
pickleInternal::in1(int c)
{
	switch (pState)
	{
	case P_TOP:
	default:			// (default to keep the compiler quiet.)
		/*
		 *	All input must be inside a list, so at this point,
		 *	the only legal character is a '('.
		 */
		if (c == '(')
		{
			pLevel = 0;
			pState = P_IDLE;
		}

		return 0;

	case P_IDLE:
	idle:
		if (c == '(')
		{
			/*
			 *	Start a new list.  Return false from here
			 *	since we know we are not at the top level.
			 */
			addList();
			pLevel++;
			return 0;
		}
		else if (c == ')')
		{
			/*
			 *	Finish a list.	If we are at the top level,
			 *	return true, since this is the terminator
			 *	for the complete list.	Return to the P_TOP
			 *	state, since the only thing we can get
			 *	after this is a new list, starting with a
			 *	'(' character.
			 */
			if (pLevel == 0)
			{
				pState = P_TOP;
				return 1;
			}

			/*
			 *	We go up and onward one, so new items are
			 *	inserted after this list.
			 */
			pLevel--;
			up();
			// next();

			return 0;
		}
		else if (c == '\'' || c == '"')
		{
			/*
			 *	A string.  Remember the quote character
			 *	used and set the state machine to handle
			 *	the string.
			 */
			pQuote = c;
			pState = P_STR;
			return 0;
		}
		else if (c == '`')
		{
			pState = P_BASE85;
			pB85 = 0;
			return 0;
		}
		else if (isname(c))
		{
			/*
			 *	A name.  Store the first character and set
			 *	the state machine to handle the name.
			 */
			pData.strcat(c);
			pState = P_NAME;
			return 0;
		}
		else
		{
			/*
			 *	An illegal character, or a space character.
			 *	Ignore it.
			 */
			return 0;
		}

	case P_STR:
    str_again:
		if (c == pQuote)
		{
    done_data:
			add(pData);
			pData.clear();
			pState = P_IDLE;
		}
		else if (c == '\\')
			pState = P_STR1;
		else if (c == '\n')
			goto done_data;
		else if (c >= 0x20 && c <= 0x7e)
			pData.strcat(c);
		return 0;

	case P_STR1:
		switch (c)
		{
		case 'a':	c = '\a';	goto got_qchr;
		case 'b':	c = '\b';	goto got_qchr;
		case 'f':	c = '\f';	goto got_qchr;
		case 'n':	c = '\n';	goto got_qchr;
		case 'r':	c = '\r';	goto got_qchr;
		case 't':	c = '\t';	goto got_qchr;
		case 'v':	c = '\v';
    got_qchr:
			pData.strcat(c);
			pState = P_STR;
			break;

		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
			pState = P_STR2;
			pChr = c - '0';
			break;

		case 'x':
			pState = P_STR5;
			break;

		case '\n':
			pData.strcat('\\');
			goto done_data;

		default:
			if (c < 0x20 || c > 0x7e)
				c = '\\';
			goto got_qchr;
		}
		return 0;

	case P_STR2:
		if (c < '0' || c > '7')
		{
			pData.strcat(pChr);
			pState = P_STR;
			goto str_again;
		}

		pChr = (pChr << 3) + (c - '0');
		pState = P_STR3;
		return 0;

	case P_STR3:
		if (c < '0' || c > '7')
		{
			pData.strcat(pChr);
			pState = P_STR;
			goto str_again;
		}

		pChr = (pChr << 3) + (c - '0');
		pData.strcat(pChr);
		pState = P_STR;
		return 0;

	case P_STR5:
		{
			int n = hex(c);

			if (n < 0)
			{
				pData.strcat('x');
				pState = P_STR;
				goto str_again;
			}

			pChr = n;
			pState = P_STR6;
			return 0;
		}

	case P_STR6:
		{
			int n = hex(c);

			if (n < 0)
			{
				pData.strcat(pChr);
				pState = P_STR;
				goto str_again;
			}

			pChr = (pChr << 4) + n;
			pData.strcat(pChr);
			pState = P_STR;
			return 0;
		}

	case P_BASE85:
		{
			unsigned long n;
			unsigned x = pB85;

			if (c == '`')
			{
				if (x >= 2)
				{
					x = b85_to_bin(x, &pB85c[0], &n);
					while (x-- > 0)
					{
						pData.strcat((n >> 24) & 0xff);
						n <<= 8;
					}
				}

				goto done_data;
			}

			if (x == 0 && c == '~')
				pData.strcat("\0\0\0\0", 4);
			else
			{
				pB85c[x++] = c;
				if (x >= 5)
				{
					if (b85_to_bin(5, &pB85c[0], &n) > 0)
					{
						pData.strcat((n >> 24) & 0xff);
						pData.strcat((n >> 16) & 0xff);
						pData.strcat((n >> 8) & 0xff);
						pData.strcat((n >> 0) & 0xff);
					}

					x = 0;
				}
			}

			pB85 = x;
			return 0;
		}

	case P_NAME:
		if (isname(c))
		{
			/*
			 *	We got another legal name character.
			 *	Add it to the current name.  We return
			 *	false since we have not seen the end of
			 *	the name yet.
			 */
			pData.strcat(c);
			return 0;
		}
		else
		{
			/*
			 *	We have seen a character that is not
			 *	legal as a name character.  Add the name
			 *	we have to the LF_t and move to the idle
			 *	state.  We drop through to the `case P_IDLE'
			 *	since the character we just received still
			 *	needs to be processed.
			 */
			add(pData);
			pData.clear();

			/*
			 *	The current character terminated the name
			 *	but is not part of the name.  Return to
			 *	the idle state and process the character
			 *	immediately.
			 */
			pState = P_IDLE;
			goto idle;
		}
	}
}


int
pickleInternal::in2(gist & s)
{
	while (s)
	{
		int		x;

		x = s[0];
		if (in1(x))
			return 1;
		s.strtrim(1);
	}

	return 0;
}


int
pickleInternal::in3()
{
	/*
	 *	No more input data.  If we are already in the P_TOP state,
	 *	we have not collected any data at all, so return false.
	 */
	if (pState == P_TOP)
		return 0;

	/*
	 *	Otherwise we need to poke the state machine until it gets
	 *	to a P_TOP state, then return true.
	 */
	for (;;)
	{
		switch (pState)
		{
		case P_TOP:
			return 1;

		case P_IDLE:			// Pops up the stack
		case P_NAME:			// End the name, go to P_IDLE
			(void)in1(')');
			break;

		case P_STR:			// End of string, go to P_IDLE
		case P_STR1:			// Illegal; go to P_STR
		case P_STR2:
		case P_STR3:
		case P_STR5:
		case P_STR6:
			(void)in1(pQuote);
			break;

		case P_BASE85:			// End of base-85, go to P_IDLE
			(void)in1('`');
			break;
		}
	}
}


gist &
gist::pickle::in(const gist & s)
{
	// gist s(data);

	if (!s && !pExcess)
		return in3();

	if (in2(pExcess))
	{
		pExcess += s;
		return 1;
	}
	if (in2(s))
	{
		pExcess = s;
		return 1;
	}

	pExcess = 0;
	return 0;
}


int
gist::pickle::in(const char * data, int len)
{
	int		c;

	if (!data)
	{
		if (!pExcess)
			return in3();
		len = 0;
	}
	else if (len < 0)
		len = strlen(data);

	if (in2(pExcess))
	{
		pExcess.concat(data, len);
		return 1;
	}

	while (len-- > 0 && (c = *data++))
	{
		if (in1(c))
		{
			pExcess.set(data, len);
			return 1;
		}
	}

	pExcess = 0;
	return 0;
}


#if 0

void
pickleInternal::inputReset()
{
	pExcess = 0;
	pState = P_TOP;
	pLevel = 0;
}

#endif

/**********************************************************************/
/*
 *	Memory allocation.
 */


gist::pickle::pickle()
{
	pickleInternal * px = new pickleInternal;
	px->value.clear();
}


gist::pickle::pickle(const gist & v)
{
	pickleInternal * px = new pickleInternal;
	px->value = v;
	px->list = value;
	px->stack.array(0);
	px->li = 0;

	ptr = px;
}


void
pickleInternal::reset()
{
}


void
gist::pickle::set(const gist & v)
{
}


gist
gist::pickle::get()
{
	return gist();
}


gist &
gist::pickle::in(const char * s)
{
	return *new gist;
}


gist &
gist::pickle::in(const gist & s)
{
	return *new gist;
}


const char *
gist::pickle::out()
{
	return 0;
}


unsigned
gist::pickle::out(const char ** vec, unsigned vecs)
{
	return 0;
}


unsigned
gist::pickle::out(gist & str)
{
}






/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/*
 *	Library :: List Interchange Format
 */


#if 0

#include	<stdio.h>
#include	<string.h>

#include	"config.h"
#include	"lf.h"

/**********************************************************************/

LF_t::LF_t()
{
	root.start = 0;
	root.end = 0;
	root.parent = 0;
	root.parentList = 0;
	stack = &stackBase;
	stackBase.up = 0;
	stackBase.node = 0;
	stackBase.list = &root;
	inputReset();
	outputReset();
}


LF_t::~LF_t()
{
	clear();
}

/**********************************************************************/

/*
 *	Render a list into its character representation.  To help the string
 *	version of `output()' and `output_length()', passing a zero pointer
 *	in `buffer' will function correctly without storing any characters.
 *	Also, `o_data' will not be cleared on return.
 */
unsigned
LF_t::output(char * buffer, unsigned count, int newLines)
{
	int		delta = 0;
	Str_t		word = 0;
	unsigned	tally = 0;
	unsigned	total = 0;

	/*
	 *	Store the new buffer details.  Leave room to put a '\0'
	 *	character, and bomb out now if we were given a bogus buffer.
	 */
	if (buffer)
	{
		if (count <= 1)
		{
			if (count == 1 && buffer)
				*buffer = '\0';
			return 0;
		}
		count--;
	}
	else
		count = 0;

	/*
	 *	Loop, adding new things to the buffer.
	 */
	for (;;)
	{
		/*
		 *	If there are still characters from a previous call,
		 *	add them to the buffer.  If the buffer fills before
		 *	we are done copying, set up to resume next time.
		 */
		if (buffer && oData)
		{
			unsigned l = oData.length();
			if (l > count)
				l = count;

			oData.copyOut(buffer, l);
			oData.strtrim(l);
			buffer += l;
			count -= l;
			total += l;

			if (count == 0)
			{
				/*
				 *	We filled the buffer.  Drop out and
				 *	allow the caller to consume it.
				 */
				*buffer = '\0';
				return total;
			}
		}

		if (oDone)
		{
			/*
			 *	There is nothing left to output, but we
			 *	are still looping to output what remains.
			 *	So if we added more data to the buffer on
			 *	this call, return true, otherwise continue
			 *	to return false.  We don't reset the output
			 *	state machine, so we can be called more
			 *	than once to output the remaining data;
			 *	the caller must call `output_reset()' before
			 *	beginning another output.
			 */
			if (buffer)
			{
				*buffer = '\0';
				return total;
			}
			else
				return tally;
		}

		/********/
		/*
		 *	If we are just starting the output, we need to output
		 *	the first '('.
		 */
		if (oLevel < 0)
		{
			word = "(";
			oLevel = 0;
			delta = 1;
		}

		/********/
		/*
		 *	If we are on top of a child list, move into it.
		 */
		while (isList())
		{
			down();
			oLevel++;
			word += "(";
			delta++;
		}

		/********/
		/*
		 *	If we are on a data item.  Create an output string
		 *	representation of the data.
		 */
		if (isData())
		{
			Str_t str = data();
			next();

			/********/
			/*
			 *	Look for characters that cannot appear in
			 *	a name; if we find none, we can output this
			 *	string without quotes.  Note that an empty
			 *	string must be quoted.  If the ratio of
			 *	non-name characters is large enough, output
			 *	the string in base-85 format.  The `8' in
			 *	the quote vs. base-85 is a fudge factor that
			 *	will cause quoting to be preferred when there
			 *	are few binary characters and small strings.
			 */
			if (str)
			{
				unsigned	i;
				unsigned	l = str.length();
				unsigned	a = 0;
				unsigned	b = 0;

				for (i = 0; i < l; i++)
				{
					int c = str[i];
					if (!isname(c))
						a = 1;
					if (c == '\\' || c < 0x20 || c > 0x7e)
						b++;
				}

				if (a == 0)
				{
					word += str;
					goto done_data;
				}
				else if (((l-b) + 4*b) < (5*l)/4 + 8)
					goto quote;
				else
					goto base85;
			}

			/********/
			/*
			 *	Generate a quoted version of the data.
			 */
    quote:
			word += "'";
			while (str)
			{
				char		a[8];
				static char	hex[] = "0123456789abcdef";
				int		c;

				c = str[0];
				str.strtrim(1);

				if (c == '\a')
				{
					c = 'a';
    q1:
					a[0] = '\\';
					a[1] = c;
					a[2] = '\0';
				}
				else if (c == '\b')
				{
					c = 'b';
					goto q1;
				}
				else if (c == '\t')
				{
					c = 't';
					goto q1;
				}
				else if (c == '\n')
				{
					c = 'n';
					goto q1;
				}
				else if (c == '\v')
				{
					c = 'v';
					goto q1;
				}
				else if (c == '\f')
				{
					c = 'f';
					goto q1;
				}
				else if (c == '\r')
				{
					c = 'r';
					goto q1;
				}
				else if (c == '\\')
					goto q1;
				else if (c == '\'')
					goto q1;
				else if (c < 0x20 || c > 0x7e)
				{
					a[0] = '\\';
					a[1] = 'x';
					a[2] = hex[(c >> 4) & 0xf];
					a[3] = hex[c & 0xf];
					a[4] = '\0';
				}
				else
				{
					a[0] = c;
					a[1] = '\0';
				}

				word += a;
			}
			word += "'";
			goto done_data;

    base85:
			word += "`";
			while (str)
			{
				char		a[8];
				int		l = str.length();
				int		i;
				unsigned long	n = 0;

				for (i = 0; i < 4; i++)
				{
					n <<= 8;
					if (i < l)
						n |= str[i] & 0xff;
				}
				if (l > 4)
					l = 4;

				i = bin_to_b85(l, n, a);
				word.concat(a, i);

				str.strtrim(l);
			}
			word += "`";
			goto done_data;
		}

    done_data:
		/********/
		/*
		 *	If we are at the end of a list, output the ")",
		 *	and move up one level.	If we hit the top, `o_level'
		 *	will go negative, which is used elseware to break
		 *	out of the output loop.
		 */
		while (oLevel > 0 && isNil())
		{
			oLevel--;
			word += ")";
			delta--;
			up();
			next();
		}

		/********/
		/*
		 *	If we are at the top level and this is the end
		 *	of the list, start to wrap things up.
		 */
		if (oLevel == 0 && isNil())
		{
			oDone = 1;
			word += ")";
		}

		/********/
		/*
		 *	We now have a complete word.  Make sure there is
		 *	room to output the word on this line;  if not,
		 *	start a new line, and indent the correct amount.
		 */
		if (word)
		{
			if (ofColumn > 0)
			{
				if (newLines &&
				    ofColumn + word.length() + 1 > MAXCOL)
				{
					if (count > 0)
						oData += "\r\n";
					tally += 2;
					ofColumn = ofIndent[ofLevel];
					tally += ofColumn;
					if (count > 0)
					{
						int		i;

						for (i = ofColumn; i > 0; i--)
							oData += " ";
					}
				}
				else
				{
					if (count > 0)
						oData += " ";
					tally++;
					ofColumn++;
				}
			}

			ofLevel += delta;
			if (delta > 0)
			{
				int		i;

				for (i = 0; i < delta; i++)
					ofIndent.concat(ofColumn + i + 1);
			}
			else if (delta < 0)
				ofIndent.strtrim(0, ofLevel + 1);
			delta = 0;

			if (count > 0)
				oData += word;
			tally += word.length();
			ofColumn += word.length();

			word = 0;
		}

		/*
		 *	If we have output the final ')', terminate the
		 *	output line.  The next time through the buffer
		 *	copy code above will quit the whole output.
		 */
		if (oDone && newLines && count > 0)
		{
			oData += "\r\n";
			tally += 2;
		}
	}
}


unsigned
LF_t::output(Str_t & s)
{
	unsigned	len;

	outputReset();
	push();
	len = output(0, 1, 0);
	pop();
	s = oData;
	outputReset();

	return len;
}


unsigned
LF_t::outputLength(int newLines)
{
	unsigned	len;

	outputReset();
	push();
	len = output(0, 0, newLines);
	pop();
	outputReset();

	return len;
}


void
LF_t::outputReset()
{
	oData = 0;
	oLevel = -1;
	oDone = 0;
	ofLevel = 0;
	ofColumn = 0;
	ofIndent = 0;
	ofIndent.concat('\0');
}

/**********************************************************************/

void
LF_t::top()
{
	stack_t *	sp = stack;

	sp->list = &root;
	sp->node = sp->list->start;
}


void
LF_t::up()
{
	stack_t *	sp = stack;
	list_t *	lp = sp->list;

	if (lp->parentList)
	{
		sp->node = lp->parent;
		sp->list = lp->parentList;
	}
}


void
LF_t::down()
{
	stack_t *	sp = stack;

	if (sp->node)
	{
		list_t *	lp;

		lp = sp->node->child;
		if (lp)
		{
			sp->list = lp;
			sp->node = lp->start;
		}
	}
}


void
LF_t::next()
{
	stack_t *	sp = stack;

	if (sp->node)
		sp->node = sp->node->next;
}


void
LF_t::prev()
{
	stack_t *	sp = stack;
	node_t *	np = sp->node;

	if (np)
	{
		if (np->prev)
			sp->node = np->prev;
	}
	else
		sp->node = sp->list->end;
}


void
LF_t::first()
{
	stack_t *	sp = stack;

	sp->node = sp->list->start;
}


void
LF_t::end()
{
	stack->node = 0;
}


int
LF_t::find(const char * name)
{
	stack_t *	sp;
	node_t *	np;

	push();
	sp = stack;
	np = sp->node;

	while (np)
	{
		list_t *	lp;
		node_t *	np1;

		if ((lp = np->child) &&
		    (np1 = lp->start) &&
		    np1->data == name)
		{
			sp->node = np1->next;
			sp->list = lp;
			return 1;
		}

		np = np->next;
	}

	pop();
	return 0;
}


int
LF_t::findData(const char * name)
{
	if (find(name))
	{
		if (isData() && !stack->node->next)
			return 1;
		pop();
	}

	return 0;
}


void
LF_t::push()
{
	stack_t *	sp = stack;
	stack_t *	nsp;

	nsp = new stack_t;
	nsp->node = sp->node;
	nsp->list = sp->list;
	nsp->up = sp;
	stack = nsp;
}


void
LF_t::pop()
{
	stack_t *	sp = stack;

	if (sp != &stackBase)
	{
		stack = sp->up;
		delete sp;
	}
}


void
LF_t::popAll()
{
	while (stack != &stackBase)
		pop();
}

/**********************************************************************/

int
LF_t::isNil()
{
	return stack->node == 0;
}


int
LF_t::isList()
{
	node_t *	np = stack->node;

	return np && np->child != 0;
}


int
LF_t::isData()
{
	node_t *	np = stack->node;

	return np && np->child == 0;
}


int
LF_t::isTop()
{
	return stack->list->parentList == 0;
}


const Str_t &
LF_t::data()
{
	node_t *	np = stack->node;

	if (isData())
		return np->data;
	else
		return Str_t::empty;
}

/**********************************************************************/

void
LF_t::add1(node_t * np)
{
	stack_t *	sp = stack;
	list_t *	lp = sp->list;
	node_t *	np1;

	np->next = np1 = sp->node;
	if (np1)
	{
		node_t *	np2;

		np->prev = np2 = np1->prev;
		np1->prev = np;
		if (np2)
			np2->next = np;
		else
			lp->start = np;
	}
	else
	{
		np->prev = np1 = lp->end;
		if (np1)
			np1->next = np;
		else
			lp->start = np;
		lp->end = np;
	}

	np->child = 0;
}


void
LF_t::add(const Str_t & str)
{
	node_t *	np;

	np = new node_t;
	add1(np);
	np->data = str;
}


void
LF_t::add(const char * str)
{
	Str_t		s;

	s = str;
	add(s);
}


void
LF_t::addList()
{
	node_t *	np;
	list_t *	lp;
	stack_t *	sp;

	np = new node_t;
	add1(np);
	sp = stack;

	lp = new list_t;
	lp->start = 0;
	lp->end = 0;
	lp->parent = np;
	lp->parentList = sp->list;
	np->child = lp;

	sp->node = 0;
	sp->list = lp;
}


void
LF_t::copy(LF_t & from)
{
	from.push();
	add("-marker-");
	prev();
	push();
	next();

	while (!from.isNil())
	{
		if (from.isList())
		{
			from.down();
			addList();
			copy(from);
			up();
			next();
			from.up();
			from.next();
		}
		else
		{
			add(from.data());
			from.next();
		}
	}

	pop();
	remove();
	from.pop();
}

/**********************************************************************/

void
LF_t::rem1(node_t *& npx, list_t *& lpx, node_t * np)
{

	while (np)
	{
		list_t *	lp;
		node_t *	np1;

		/*
		 *	If there is a child list, recurse down it.
		 */
		if ((lp = np->child))
		{
			rem1(npx, lpx, lp->start);
			lp->start = (node_t *)-1;
			lp->parentList = lpx;
			lpx = lp;
		}

		/*
		 *	Mark the nodes we are deleting so we can clean
		 *	the stack.
		 */
		np->prev = (node_t *)-1;

		/*
		 *	Add the node to the local remove list.
		 */
		np1 = np->next;
		np->next = npx;
		npx = np;

		/*
		 *	Next?
		 */
		np = np1;
	}
}


void
LF_t::remove()
{
	stack_t *	sp = stack;
	node_t *	np = sp->node;
	list_t *	lp = sp->list;

	/*
	 *	If the current item is NIL, there is nothing to remove.
	 */
	if (!np)
		return;

	/*
	 *	Extract the current item from its friends.
	 */
	sp->node = np->next;
	if (np->prev)
		np->prev->next = np->next;
	else
		lp->start = np->next;
	if (np->next)
		np->next->prev = np->prev;
	else
		lp->end = np->prev;

	/*
	 *	Make a single list of all decendants of this node, marking
	 *	them as we go so we can clean the stack.
	 */
	node_t *	npx = 0;
	list_t *	lpx = 0;

	np->next = 0;
	rem1(npx, lpx, np);

	/*
	 *	Search up the stack for nodes that are now deleted.
	 */
	stack_t *	spt;
	stack_t *	sp1;

	spt = 0;
	for (sp1 = sp; sp1; sp1 = sp1->up)
		if ((sp1->node && sp1->node->prev == (node_t *)-1) ||
		    (sp1->list && sp1->list->start == (node_t *)-1))
			spt = sp1;

	/*
	 *	If we found stale stack slots, pop the stack up to
	 *	that point.
	 */
	if (spt)
	{
		for (;;)
		{
			/*
			 *	If the top most stack slot is stale, it
			 *	must be cleared specially, since it is
			 *	not allocated from free store.
			 */
			if (sp == &stackBase)
			{
				sp->node = 0;
				sp->list = &root;
				break;
			}

			/*
			 *	Grab the next up slot and free this slot.
			 */
			stack_t * spu = sp->up;
			stack = spu;
			delete sp;

			/*
			 *	If we just cleared the top most offending
			 *	slot, quit.
			 */
			if (sp == spt)
				break;

			sp = spu;
		}
	}

	/*
	 *	Now we actually free up the nodes we are to remove.
	 */
	while (npx)
	{
		np = npx->next;
		delete npx;
		npx = np;
	}
	while (lpx)
	{
		lp = lpx->parentList;
		delete lpx;
		lpx = lp;
	}
}

/**********************************************************************/

void
LF_t::clr1(node_t * np)
{
	list_t *	lp;

	while (np)
	{
		node_t *	np1;

		if ((lp = np->child))
		{
			clr1(lp->start);
			delete lp;
		}

		np1 = np->next;
		delete np;
		np = np1;
	}
}


void
LF_t::clear()
{
	clr1(root.start);

	stack_t * sp = stack;
	while (sp != &stackBase)
	{
		stack_t *	sp1;

		sp1 = sp->up;
		delete sp;
		sp = sp1;
	}

	root.start = 0;
	root.end = 0;
	sp->node = 0;
	sp->list = &root;
	stack = sp;
}

#endif
