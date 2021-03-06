/*
 *	Gist -- 32-bit integers.
 *
 *	Contains:	(int)
 */

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

gist::operator long() const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		return 0;

	case GT_SSTR:
	case GT_MSTR:
	case GT_LSTR:
		{
			try
			{
				return _toInt(true, 10);
			}
			catch (valueError)	{}
			catch (overflowError)	{}

			if (typ == GT_SSTR)
				return scnt > 0;
			else
				return str.cnt > 0;
		}

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (long)dval;
	}
}


gist::operator int() const
{
	return (int)(long)*this;
}


gist::operator unsigned long() const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		return 0;

	case GT_SSTR:
	case GT_MSTR:
	case GT_LSTR:
		{
			try
			{
				return _toInt(false, 10);
			}
			catch (valueError)	{}
			catch (overflowError)	{}

			if (typ == GT_SSTR)
				return scnt > 0;
			else
				return str.cnt > 0;
		}

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (unsigned long)dval;
	}
}


gist::operator unsigned() const
{
	return (unsigned)(unsigned long)*this;
}


long
gist::toInt(unsigned base) const
{
	switch (typ)
	{
	case GT_NIL:
	case GT_ARRAY:
	case GT_TABLE:
	case GT_PTR:
	case GT_CODE:
	case GT_LONG:
	case GT_REAL:
	default:
		throw valueError("toInt");

	case GT_SSTR:
	case GT_MSTR:
	case GT_LSTR:
		return _toInt(true, base);

	case GT_INT:
		return val;

	case GT_FLOAT:
		return (long)dval;
	}
}


long
gist::_toInt(bool sign, unsigned base) const
{
	unsigned long n = 0;
	bool neg = false;
	bool seen = false;
	int c;

	/*
	 *	We use flatten here so we can get a C string to parse.
	 *	This is a bit lazy -- we could use strpiece() to avoid
	 *	a possible string copy, or at least not insist on a '\0'
	 *	being added and use a count here.  My only defense is that
	 *	most strings converted here will already conform to the
	 *	shape needed to avoid the copy in strflatten().
	 */
	char * str = _strflatten(false, true, 0);

	if (base == 1 || base > 36)
		throw gist::valueError("bad base in toInt");

	while (*str == ' ' || *str == '\t')
		str++;

	if (sign && *str == '-')
	{
		neg = true;
		str++;
	}
	else if (base == 0 && *str == '0')
	{
		c = str[1];
		if (c == 'x' || c == 'X')
		{
			base = 16;
			str += 2;
		}
		else
		{
			base = 8;
			str++;
			seen = true;
		}
	}
	if (base == 0)
		base = 10;

	while ((c = *str))
	{
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			break;
		str++;

		if (c >= (signed)base)
			goto value;

		seen = true;

		unsigned long n1 = n * base;
		if (n1 < n)
			goto overflow;
		n = n1 + c;
	}

	while (c == ' ' || c == '\t' || c == '\n')
		c = *str++;
	if (*str != '\0')
		goto value;

	if (sign)
	{
		if (neg && n == (~0U>>1) + 1)
			return n;
		if (n > (~0U>>1))
			goto overflow;
		if (neg)
			return -(long)n;
		else
			return n;
	}
	else
		return n;

  value:
	throw gist::valueError("bad digit in toInt");
  overflow:
	throw gist::overflowError("toInt");
}
