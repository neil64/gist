/*
 *	Based loosely on the W3C SGML parser.
 */

#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>

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
	data = "";
	tag.clear();
	attrs.table(true);
	attrv = 0;
}


void
SGML_t::doHandleData()
{
	if (data)
		handleData(data);
	data = "";
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
			data = "";
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
				data += c;
			break;

		case S_nl:
			if (c == '<')
			{
				doHandleData();
				state = S_nl_tago;
			}
			else
			{
				data += '\n';
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
				data += c;
			else
			{
				char * x = 0;

				if (!data)
					x = "& ";
				else if (data == "lt")
					x = "<";
				else if (data == "gt")
					x = ">";
				else if (data == "amp")
					x = "&";
				else if (data == "quot")
					x = "\"";
				else if (data == "apos")
					x = "'";

				if (x)
				{
					gist y = x;
					handleData(y);
				}
				else
					unknownEntity(data);

				data = "";
				if (c != ';')
					goto S_text;
				state = S_text;
			}
			break;

		case S_cro:
			if (isalnum(c))
				data += c;
			else
			{
				char x[2];
				x[0] = atoi(data);
				x[1] = '\0';
				gist y = x;
				handleData(y);

				data = "";
				if (c != ';')
					goto S_text;
				state = S_text;
			}
			break;

		case S_tag:
		handle_S_tag:
			if (isalnum(c))
				data += c;
			else
			{
				if (c == '/')
					state = S_end;
				else if (c == '!')
					state = S_md;
				else
				{
					tag = strlower(data);
					data = "";
					goto S_tag_gap;
				}

				data = "";
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
				data += c;
				break;
			}

			/*
			 *	Subtle:	 A table subscript to an entry that
			 *	does not exist creates a new entry with the
			 *	value NIL, and returns a reference to it.
			 *	We store that reference an maybe assign to
			 *	it later.
			 */
			attrv = &attrs[strlower(data)];
			data = "";

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
				*attrv = data;
				data = "";
				goto S_tag_gap;
			}
			else
				data += c;
			break;
		
		case S_squoted:
			if (c == '\'')
			{
				*attrv = data;
				data = "";
				state = S_tag_gap;
			}
			else if (c && c != '\n' && c != '\r')
				data += c;
			break;
	
		case S_dquoted:
			if (c == '"')
			{
				*attrv = data;
				data = "";
				state = S_tag_gap;
			}
			else if (c && c != '\n' && c != '\r')
				data += c;
			break;

		case S_end:
			if (isalnum(c))
				data += c;
			else
			{
				gist x = strlower(data);
				data = "";
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
				data = "";
				state = S_text;
			}
			break;

		case S_md_dqs:
			if (c == '"')
				state = S_md;
			else if (c == '>')
			{
				data = "";
				state = S_text;
			}
			break;

		case S_md_sqs:
			if (c == '\'')
				state = S_md;
			else if (c == '>')
			{
				data = "";
				state = S_text;
			}
			break;

		case S_com_1:
			state = (c == '-') ? S_com : S_md;
			if (c == '>')
			{
				data = "";
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
				data = "";
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
