/*
 *	Gist -- Debugging code.
 */

#include <stdio.h>

#include "gist.h"
#include "gist-internal.h"


/**********************************************************************/
/*
 *	Print a gist object.
 */

static void
tab(FILE * fd, int indent)
{
	fprintf(fd, "%*s", indent, "");
}


static void	gistPrint(FILE *, gist * gp, int indent, int col);

void
gistPrintIntIndex(void * fd1, giIndexInt * ip, bool str, int indent)
{
	FILE * fd = (FILE *)fd1;

	tab(fd, indent);
	fprintf(fd, "levels=%d, maxlevel=%d, min=%d, max=%d",
		ip->levels, ip->maxLevel, ip->min, ip->max);

	for (int i = 0; i < ip->levels; i++)
	{
		if (i % 4 == 0)
		{
			fprintf(fd, "\n");
			tab(fd, indent);
			fprintf(fd, "heads: ");
		}
#if __x86_64__
		fprintf(fd, " 0x%16lx", (size_t)ip->head[i]);
#else // __x86_64__
		fprintf(fd, " 0x%08x", (int)ip->head[i]);
#endif // __x86_64__
	}
	fprintf(fd, "\n");

	for (intKey * kp = ip->head[0]; kp; kp = kp->fwd[0])
	{
		tab(fd, indent);
#if __x86_64__
		fprintf(fd, "key: 0x%16lx, key=%d, ", (size_t)kp, kp->key);
#else // __x86_64__
		fprintf(fd, "key: 0x%08x, key=%d, ", (int)kp, kp->key);
#endif // __x86_64__
		if (str)
		{
			giSChunk * cp = kp->schunk;
			unsigned z = cp->len;
			if (z > 16)
				z = 16;
			fprintf(fd, "len=%d, \"%.*s\"%s",
				cp->len,
				z, &cp->data[0],
				z < cp->len ? " ..." : "");
			fprintf(fd, "\n");
		}
		else
		{
			fprintf(fd, "\n");
			giAChunk * cp = kp->achunk;
			for (int i = 0; i < giAChunk::items; i++)
				gistPrint(fd, &cp->g[i], indent + 4, 0);
		}
	}
}


static void
gistPrint(FILE * fd, gist * gp, int indent, int col)
{
	if (indent >= 40)
	{
		tab(fd, 40);
		fprintf(fd, "...\n");
		return;
	}

	tab(fd, indent - col);

	switch (gp->typ)
	{
	case gist::GT_NIL:
		fprintf(fd, "NIL\n");
		break;

	case gist::GT_INT:
		fprintf(fd, "INT  %ld\n", gp->val);
		break;

	case gist::GT_FLOAT:
		fprintf(fd, "FLOAT  %g\n", gp->dval);
		break;

	case gist::GT_SSTR:
		fprintf(fd, "SSTR  scnt=%d, data: %.*s\n",
					gp->scnt, gp->scnt, gp->sstr);
		break;

	case gist::GT_MSTR:
		{
			fprintf(fd, "MSTR  unique=%s, cnt=%d, sz=%d\n",
				gp->unique ? "true" : "false",
				gp->str.cnt,
				gp->str.sz);
			tab(fd, indent + 6);
#if __x86_64__
			fprintf(fd, "dat=0x%16lx: ", (size_t)gp->str.dat);
#else // __x86_64__
			fprintf(fd, "dat=0x%08x: ", (int)gp->str.dat);
#endif // __x86_64__

			unsigned z = gp->str.cnt;
			if (z > 32)
				z = 32;
			fprintf(fd, "%.*s%s\n",
				z, gp->str.dat,
				z < gp->str.cnt ? " ..." : "");
		}
		break;

	case gist::GT_LSTR:
		{
			giStr * sp = gp->str.idx;

#if __x86_64__
			fprintf(fd, "LSTR  unique=%s, skp=%d, cnt=%d, "
					"idx=0x%16lx, \n",
				gp->unique ? "true" : "false",
				gp->str.skp, gp->str.cnt, (size_t)sp);
#else // __x86_64__
			fprintf(fd, "LSTR  unique=%s, skp=%d, cnt=%d, "
					"idx=0x%08x, \n",
				gp->unique ? "true" : "false",
				gp->str.skp, gp->str.cnt, (int)sp);
#endif // __x86_64__
			tab(fd, indent + 6);
#if __x86_64__
			fprintf(fd, "idx: "
					"chunk=0x%16lx, size=%d\n",
					(size_t)sp->chunk, sp->size);
#else // __x86_64__
			fprintf(fd, "idx: "
					"chunk=0x%08x, size=%d\n",
					(int)sp->chunk, sp->size);
#endif // __x86_64__

			tab(fd, indent + 6);
			fprintf(fd, "idx.index:\n");
			gistPrintIntIndex(fd, &sp->index, true, indent + 10);
		}
		break;

	case gist::GT_ARRAY:
		{
			giArray * ap = gp->arr;

#if __x86_64__
			fprintf(fd, "ARRAY  unique=%s, "
					"arr=0x%16lx\n",
				gp->unique ? "true" : "false",
				(size_t)ap);
#else // __x86_64__
			fprintf(fd, "ARRAY  unique=%s, "
					"arr=0x%08x\n",
				gp->unique ? "true" : "false",
				(int)ap);
#endif // __x86_64__
			tab(fd, indent + 7);
			fprintf(fd, "arr: skip=%d, len=%d\n",
				ap->skip, ap->len);

			tab(fd, indent + 7);
			fprintf(fd, "arr.index:\n");
			gistPrintIntIndex(fd, &ap->index, false, indent + 11);
		}
		break;

	case gist::GT_TABLE:
		fprintf(fd, "TABLE\n");
		break;

	case gist::GT_PTR:
#if __x86_64__
		fprintf(fd, "PTR  0x%16lx\n", (size_t)gp->ptr);
#else // __x86_64__
		fprintf(fd, "PTR  0x%08x\n", (int)gp->ptr);
#endif // __x86_64__
		break;

	case gist::GT_REGEX:
		fprintf(fd, "REGEX\n");
		break;

	case gist::GT_FILE:
		{
			giFile * fp = gp->fil;
#if __x86_64__
			fprintf(fd, "FILE  unique=%s, fil=0x%16lx, "
					"%c%c%c%c, fd=%d\n",
				gp->unique ? "true" : "false",
				(size_t)fp,
				fp->open ? 'o' : '-',
				fp->readable ? 'r' : '-',
				fp->writable ? 'w' : '-',
				fp->eof ? 'e' : '-',
				fp->fd);
#else // __x86_64__
			fprintf(fd, "FILE  unique=%s, fil=0x%08x, "
					"%c%c%c%c, fd=%d\n",
				gp->unique ? "true" : "false",
				(int)fp,
				fp->open ? 'o' : '-',
				fp->readable ? 'r' : '-',
				fp->writable ? 'w' : '-',
				fp->eof ? 'e' : '-',
				fp->fd);
#endif // __x86_64__
			gistPrint(fd, &fp->buffer, indent + 6, 0);
		}
		break;

	case gist::GT_CODE:
		fprintf(fd, "CODE\n");
		break;

	case gist::GT_LONG:
		fprintf(fd, "LONG\n");
		break;

	case gist::GT_REAL:
		fprintf(fd, "REAL\n");
		break;

	}
}


void
GistPrint(gist * gp)
{
	gistPrint(stderr, gp, 0, 0);
}


void
GistPr1(gist * gp, int indent, int col)
{
	gistPrint(stdout, gp, indent, col);
}


void
GistPr2(gist * gp, int indent, int col)
{
	gistPrint(stderr, gp, indent, col);
}
