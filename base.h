/*
 *	Gist fundamental data structures.
 */

#ifndef __BASE_H__
#define __BASE_H__

/**********************************************************************/

/*
 *	Pairs (dotted pairs, cells, cons cell, ...).
 *
 *	Pairs are the basis for all objects in Gist.  A reference to an
 *	object is a pointer to its Pair_t.  The first value in the Pair_t
 *	determines the type of the object, and the type of the second
 *	value depends on that object type.
 *
 *	If the Pair_t is part of a list, the first value is a pointer to
 *	another Pair_t, or NIL (0).  In this case, the least significant
 *	three bits will always be zero (they are 8-bytes big and 8-byte
 *	aligned), allowing them to be used for other things without loosing
 *	data in the pointer.  These bits are used as follows:
 *
 *		Bit 0	-  ATOM:  set if this Pair_t is an atom,
 *				  clear if its a list.
 *		Bit 1	-  MARK:  set in the mark phase of the garbage
 *				  collector if this Pair_t is still
 *				  referenced, and cleared in the sweep.
 *		Bit 2	-  GC:    garbage colector flag, used in marking,
 *				  cleared in the sweep.
 *
 *	If the Pair_t is an atom, the remainder of the first value is a
 *	constant denoting the atom's type.  The second value may be the
 *	atom's value or may be a pointer to storage containing the atom's
 *	value.
 *
 *	Pair_t's are always fixed in memory, and allocated from a Pair_t
 *	free list.  If an atom points to additional storage, that storage
 *	is allocated from the variable size object arena, which is memory
 *	that is compacted by the garbage collector.
 *
 *	Note that the following class is not strictly portable but will
 *	on all 32-bit machines.  It relies on both pointers and ints being
 *	32-bits in size and the same endian and alignment.  This may not
 *	be the case on some 64-bit CPUs.
 */

class Pair_t
{
	union
	{
		struct
		{
			Pair_t *	car;
			Pair_t *	cdr;
		};
		struct
		{
			int		type;
			union
			{
				int		value;
				class Atom_t *	atom;
			};
		};
	};
};


/*
 *	Atoms.
 *
 *	If a Pair_t is an atom (see above), and uses additional storage in
 *	the compacting arena, this class forms the header of that storage.
 */
class Atom_t
{
	/*
	 *	Back pointer, to allow the garbage collector to move this
	 *	value in the compacted arena.
	 */
	Pair_t *	back;

	/*
	 *	The remainder of the storage for the atom is variable size,
	 *	and usually used by deriving from this class.
	 */
};

/**********************************************************************/

#endif // __BASE_H__
