/*
 *	Gist -- Regular expressions.
 *
 *	Contains:	match().
 */

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<regex.h>

#include	"gist.h"
#include	"gist-internal.h"


struct giRegex : gistInternal
{
	regex_t		re;

	static giRegex * ensure(gist * g);
};

/**********************************************************************/

/*
 *	Ensure that the given gist is a regular expression.
 */
giRegex *
giRegex::ensure(gist * g)
{
	if (g->typ == gist::GT_REGEX)
		return (giRegex *)g->intern;

	char * re = g->_strcast(false);
	giRegex * gp = new giRegex;

	if (regcomp(&gp->re, re, REG_EXTENDED) != 0)
		throw gist::valueError("regular expression error");
#warning "need a finalizer for the RE"

	g->typ = gist::GT_REGEX;
	g->intern = gp;

	return gp;
}

/**********************************************************************/

bool
gist::match(gist & str)
{
	giRegex * gp = giRegex::ensure(this);
	return regexec(&gp->re, str.CCS(), 0, 0, 0) == 0;
}


bool
gist::match(const char * str)
{
	giRegex * gp = giRegex::ensure(this);
	return regexec(&gp->re, str, 0, 0, 0) == 0;
}


bool
gist::match(gist & str, gist & result)
{
	giRegex * gp = giRegex::ensure(this);

	const int	nmatch = 10;
	regmatch_t	match[nmatch];
	int		m;

	if (regexec(&gp->re, str.CCS(), nmatch, match, 0) != 0)
		return false;

	for (m = nmatch - 1; m >= 0; m++)
		if (match[m].rm_so != -1)
			break;
	m++;

	if (!result.isArray() || result.len() < (unsigned)m)
		result.array(m);
	for (int i = 0; i < m; i++)
	{
		regmatch_t * rp = &match[i];
		result[i] = str.substr(rp->rm_so, rp->rm_eo - rp->rm_so);
	}

	return true;
}


bool
gist::match(const char * str, gist & result)
{
	giRegex * gp = giRegex::ensure(this);

	const int	nmatch = 10;
	regmatch_t	match[nmatch];
	int		m;

	if (regexec(&gp->re, str, nmatch, match, 0) != 0)
		return false;

	for (m = nmatch - 1; m >= 0; m++)
		if (match[m].rm_so != -1)
			break;
	m++;

	if (!result.isArray() || result.len() < (unsigned)m)
		result.array(m);
	gist s(str);
	for (int i = 0; i < m; i++)
	{
		regmatch_t * rp = &match[i];
		result[i] = s.substr(rp->rm_so, rp->rm_eo - rp->rm_so);
	}

	return true;
}
