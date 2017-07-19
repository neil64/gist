/*
 *	Based loosely on the W3C SGML parser.
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>

#include	"gist.h"
#include	"sgml.h"


/**********************************************************************/

/*
 *	Destructor only exists because of the virtual functions.
 */
SGML_t::~SGML_t()
{
}

/********************/
/*
 *	Default virtual functions;  they do nothing.
 */

void
SGML_t::handleData(const gist & d)
{
}


void
SGML_t::startTag(const gist & tag, gist & attrs)
{
}


void
SGML_t::endTag(const gist & tag)
{
}


void
SGML_t::unknownEntity(const gist & e)
{
}


void
SGML_t::unknownChar(const gist & c)
{
}

/**********************************************************************/

void
SGML_t::reset()
{
	state = S_text;
	datx = 0;
	tag.clear();
	attrs.table(true);
	attrv = 0;
}


void
SGML_t::doHandleData()
{
	if (datx > 0)
	{
		data[datx] = '\0';
		gist da(data);
		handleData(da);
		datx = 0;
	}
}


void
SGML_t::add(int c, bool dat)
{
	if (datx >= datsz - 1)
	{
		if (!dat)
			throw badSGML();
		doHandleData();
	}
	data[datx++] = c;
}


void
SGML_t::lower()
{
	for (char * cp = &data[0]; cp < &data[datx]; cp++)
	{
		int c = *cp;
		if (c >= 'A' && c <= 'Z')
			*cp = c + ('a' - 'A');
	}
	data[datx] = '\0';
}


void
SGML_t::parse(const char * in, int sz)
{
	const char *	ep;

	if (sz >= 0)
		ep = in + sz;
	else
		ep = in + strlen(in);

	/*
	 *	Loop though the input.
	 */
	while (in < ep)
	{
		int		c;

		c = *in++;

		switch (state)
		{
		got_element_open:
			startTag(tag, attrs);
			tag.clear();
			attrs.table(true);
			state = S_after_open;
			datx = 0;
			break;

		case S_after_open:
			if (c == '\n')
			{
				state = S_text;
				break;
			}

			// fall through

		S_text:
			state = S_text;
		case S_text:
			if (c == '&')
			{
				doHandleData();
				state = S_ero;
			}
			else if (c == '<')
			{
				doHandleData();
				state = S_tag;
			}
			else if (c == '\n')
				state = S_nl;
			else
				add(c, true);
			break;

		case S_nl:
			if (c == '<')
			{
				doHandleData();
				state = S_nl_tago;
			}
			else
			{
				add('\n', true);
				goto S_text;
			}
			break;

		case S_nl_tago:
			if (c != '/')
			{
				gist x("\n");
				handleData(x);
			}
			state = S_tag;
			goto handle_S_tag;

		case S_ero:
			if (c == '#')
			{
				state = S_cro;
				break;
			}
			state = S_entity;

			// fall though

		case S_entity:
			if (isalnum(c))
				add(c, true);
			else
			{
				data[datx] = '\0';
				char * x = 0;

				if (datx == 0)
					x = (char *)"& ";
				else if (strcmp(data, "lt") == 0)
					x = (char *)"<";
				else if (strcmp(data, "gt") == 0)
					x = (char *)">";
				else if (strcmp(data, "amp") == 0)
					x = (char *)"&";
				else if (strcmp(data, "quot") == 0)
					x = (char *)"\"";
				else if (strcmp(data, "apos") == 0)
					x = (char *)"'";

				if (x)
				{
					gist y = x;
					handleData(y);
				}
				else
					unknownEntity(data);

				datx = 0;
				if (c != ';')
					goto S_text;
				state = S_text;
			}
			break;

		case S_cro:
			if (isalnum(c))
				add(c, false);
			else
			{
				data[datx] = '\0';
				char x[2];
				x[0] = atoi(data);
				x[1] = '\0';
				gist y = x;
				handleData(y);

				datx = 0;
				if (c != ';')
					goto S_text;
				state = S_text;
			}
			break;

		case S_tag:
		handle_S_tag:
			if (isalnum(c))
				add(c, false);
			else
			{
				if (c == '/')
					state = S_end;
				else if (c == '!')
					state = S_md;
				else
				{
					lower();
					tag = data;
					datx = 0;
					goto S_tag_gap;
				}

				datx = 0;
			}
			break;

		S_tag_gap:
			state = S_tag_gap;
		case S_tag_gap:
			if (isspace(c))
				break;

			if (c == '>')
				goto got_element_open;

			// fall though

		S_attr:
			state = S_attr;
		case S_attr:
			if (!isspace(c) && c != '>' && c != '=')
			{
				add(c, false);
				break;
			}

			/*
			 *	Subtle:	 A table subscript to an entry that
			 *	does not exist creates a new entry with the
			 *	value NIL, and returns a reference to it.
			 *	We store that reference an maybe assign to
			 *	it later.
			 */
			lower();
			attrv = &attrs[&data[0]];
			datx = 0;

			state = S_attr_gap;
			// fall though

		case S_attr_gap:
			if (isspace(c))
				break;

			if (c == '=')
			{
				state = S_equals;
				break;
			}

			/*
			 *	No value, so assign an integer one.
			 *	An integer is not a possible value from
			 *	the input, so an application can tell the
			 *	difference if it needs to.  Otherwise,
			 *	a one gives a positive value when used as
			 *	a boolean.
			 */
			*attrv = 1;

			if (c == '>')
				goto got_element_open;
			else
				goto S_attr;

		case S_equals:
			if (isspace(c))
				break;

			if (c == '>')
			{
				*attrv = 1;
				goto got_element_open;
			}

			if (c == '\'')
			{
				state = S_squoted;
				break;
			}
			else if (c == '"')
			{
				state = S_dquoted;
				break;
			}

			state = S_value;
			// fall through

		case S_value:
			if (isspace(c) || c == '>')
			{
				data[datx] = '\0';
				*attrv = data;
				datx = 0;
				goto S_tag_gap;
			}
			else
				add(c, false);
			break;
		
		case S_squoted:
			if (c == '\'')
			{
				data[datx] = '\0';
				*attrv = data;
				datx = 0;
				state = S_tag_gap;
			}
			else if (c && c != '\n' && c != '\r')
				add(c, false);
			break;
	
		case S_dquoted:
			if (c == '"')
			{
				data[datx] = '\0';
				*attrv = data;
				datx = 0;
				state = S_tag_gap;
			}
			else if (c && c != '\n' && c != '\r')
				add(c, false);
			break;

		case S_end:
			if (isalnum(c))
				add(c, false);
			else
			{
				lower();
				gist x = data;
				datx = 0;
				endTag(x);

				if (c != '>')
					state = S_junk_tag;
				else
					state = S_text;
			}
			break;

		case S_junk_tag:
			if (c == '>')
				state = S_text;
			break;

		case S_md:
			if (c == '-')
				state = S_com_1;
			else if (c == '"')
				state = S_md_dqs;
			else if (c == '\'')
				state = S_md_sqs;
			else if (c == '>')
			{
				datx = 0;
				state = S_text;
			}
			break;

		case S_md_dqs:
			if (c == '"')
				state = S_md;
			else if (c == '>')
			{
				datx = 0;
				state = S_text;
			}
			break;

		case S_md_sqs:
			if (c == '\'')
				state = S_md;
			else if (c == '>')
			{
				datx = 0;
				state = S_text;
			}
			break;

		case S_com_1:
			state = (c == '-') ? S_com : S_md;
			if (c == '>')
			{
				datx = 0;
				state = S_text;
			}
			break;

		case S_com:
			if (c == '-')
				state = S_com_2;
			break;

		case S_com_2:
			state = (c == '-') ? S_com_2a : S_com;
			break;

		case S_com_2a:
			if (c == '>')
			{
				datx = 0;
				state = S_text;
			}
			else if (c == '-')
				state = S_com_2a;
			else
				state = S_com;
			break;
		}
	}
}
