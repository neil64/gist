/*
 *	Gist strings.
 */

#ifndef __STRING_H__
#define __STRING_H__

/**********************************************************************/

/*
 *	A string.
 *
 *	Strings are usually immutable in LISP, requiring them to be copied
 *	if they are to be modified.  In Gist, strings keep a reference count
 *	so that copy need not take place if there is only one reference.
 *	A string (really a reference to the string proper) is allocated from the 16-byte object arena.
 */
class String_t : Atom_t
{
	class str_t
	{
		String_t *	back;
		int		refs;
		int		size;
		char		data[0];
	};

	int		length;
	int		skip;
	str_t *		str;
};

/**********************************************************************/

#endif // __STRING_H__
