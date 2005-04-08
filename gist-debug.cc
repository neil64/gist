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
indent(FILE * fd, const char * label, int level)
{
	fprintf(fd, "%.*s%s: ",
		level*2,
		"                                    ",
		label ? label : "");
}


static void	gistPrint(FILE *, gist * gp, const char * label, int level);

void
gistPrintIntIndex(void * fd1, giIndexInt * ip, bool str, int level)
{
	FILE * fd = (FILE *)fd1;

	indent(fd, str ? "string index" : "array index", level);
	fprintf(fd, "levels=%d, maxlevel=%d, min=%d, max=%d\n",
		ip->levels, ip->maxLevel, ip->min, ip->max);

	indent(fd, "head", level+1);
	for (int i = 0; i < ip->levels; i++)
		fprintf(fd, " 0x%08x", (int)ip->head[i]);
	fprintf(fd, "\n");

	for (intKey * kp = ip->head[0]; kp; kp = kp->fwd[0])
	{
		indent(fd, "key", level+2);
		fprintf(fd, "0x%08x, key=%d, ", (int)kp, kp->key);
		if (str)
		{
			giSChunk * cp = kp->schunk;
			unsigned z = cp->len;
			if (z > 32)
				z = 32;
			fprintf(fd, "len=%d, %.*s%s",
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
				gistPrint(fd, &cp->g[i], "", level+3);
		}
	}
}


static void
gistPrint(FILE * fd, gist * gp, const char * label, int level)
{
	if (level >= 16)
	{
		indent(fd, label, 16);
		fprintf(fd, "...\n");
		return;
	}

	indent(fd, label, level);

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
			fprintf(fd, "MSTR  unique=%s, cnt=%d,\n",
				gp->unique ? "true" : "false", gp->str.cnt);
			indent(fd, "    ", level);
			fprintf(fd, "     "
					"dat=0x%08x, size=%d\n",
					(int)gp->str.dat, gp->str.sz);

			indent(fd, "data", level+1);
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
			giSChunk * cp = sp->chunk;

			fprintf(fd, "STR  unique=%s, skip=%d, cnt=%d, "
					"idx=0x%08x, \n",
				gp->unique ? "true" : "false",
				gp->str.skp, gp->str.cnt, (int)sp);
			indent(fd, "    ", level);
			fprintf(fd, "     "
					"chunk=0x%08x\n", (int)cp);

			gistPrintIntIndex(fd, &sp->index, true, level+1);
		}
		break;

	case gist::GT_ARRAY:
		{
			giArray * ap = gp->arr;

			fprintf(fd, "ARRAY  unique=%s, "
					"arr=0x%08x, \n",
				gp->unique ? "true" : "false",
				(int)ap);
			indent(fd, "    ", level);
			fprintf(fd, "     "
					"skip=%d, len=%d\n",
				ap->skip, ap->len);

			gistPrintIntIndex(fd, &ap->index, false, level+1);
		}
		break;

	case gist::GT_TABLE:
		fprintf(fd, "TABLE\n");
		break;

	case gist::GT_PTR:
		fprintf(fd, "PTR  0x%08x\n", (int)gp->ptr);
		break;

	case gist::GT_REGEX:
		fprintf(fd, "REGEX\n");
		break;

	case gist::GT_FILE:
		{
			giFile * fp = gp->fil;
			fprintf(fd, "FILE  unique=%s, fil=0x%08x, "
					"%c%c%c%c, fd=%d\n",
				gp->unique ? "true" : "false",
				(int)fp,
				fp->open ? 'o' : '-',
				fp->readable ? 'r' : '-',
				fp->writable ? 'w' : '-',
				fp->eof ? 'e' : '-',
				fp->fd);
			gistPrint(fd, &fp->buffer, "buffer", level+1);
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
	gistPrint(stderr, gp, "gist", 0);
}


void
GistPr1(gist * gp, int level)
{
	gistPrint(stdout, gp, "gist", level);
}


void
GistPr2(gist * gp, int level)
{
	gistPrint(stderr, gp, "gist", level);
}
