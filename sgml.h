/*
 *	Based loosely on the W3C SGML parser.
 */

#ifndef __SGML_H__
#define __SGML_H__


#include	"gist.h"

/**********************************************************************/

/*
 *	Derive from this class to create a parser for a specific purpose.
 */
class SGML_t
{
    public:
	SGML_t()		{ reset(); }
	virtual ~SGML_t();

    public:
	/*
	 *	Parse the given SGML/HTML text, calling the appropriate
	 *	handlers.  When the parser completes parsing the given
	 *	data, it stored bytes that are not yet parsed, and returns.
	 *	The next call will continue parsing using the saved data
	 *	and the new data.
	 */
	void		parse(const char * input, int sz = -1);

	/*
	 *	Reset the parser for another run.
	 */
	void		reset();

    public:
	/*
	 *	These funcitons are called when the various parts of the
	 *	SGML data are successfully parsed.
	 */
	virtual void	handleData(const gist & d);
	virtual void	startTag(const gist & tag, gist & attrs);
	virtual void	endTag(const gist & tag);
	virtual void	unknownEntity(const gist & e);
	virtual void	unknownChar(const gist & c);

    private:
	gist		data;		// Accumulated data
	gist		tag;		// Current tag we are parsing
	gist		attrs;		// Collected attributes (table)
	gist *		attrv;		// Reference to last attribute value

	/*
	 *	Parser state.
	 */
	enum state_e
	{
		S_text,		S_tag,		S_tag_gap,
		S_attr,		S_attr_gap,	S_equals,	S_value,
		S_after_open,	S_nl,		S_nl_tago,	S_ero,
		S_cro,		S_squoted,	S_dquoted,	S_end,
		S_entity,	S_junk_tag,	S_md,		S_md_sqs,
		S_md_dqs,	S_com_1,	S_com,		S_com_2,
		S_com_2a
	}
		state;

	void		doHandleData();
};

/**********************************************************************/

#endif // __SGML_H__
