/*
 *	Gist -- Debugging code.
 */

#include <stdio.h>

#include "gist.h"
#include "gist-internal.h"


/**********************************************************************/
/*
 *	Print a gist object to stderr.
 */

static void
indent(const char * label, int level)
{
	fprintf(stderr, "%.*s%s: ",
		level*2,
		"                                    ",
		label ? label : "");
}


static void	gistPrint(gist * gp, const char * label, int level);

void
gistPrintIntIndex(giIndexInt * ip, bool str, int level)
{
	indent(str ? "string index" : "array index", level);
	fprintf(stderr, "levels=%d, maxlevel=%d, min=%d, max=%d\n",
		ip->levels, ip->maxLevel, ip->min, ip->max);

	indent("head", level+1);
	for (int i = 0; i < ip->levels; i++)
		fprintf(stderr, " 0x%08x", (int)ip->head[i]);
	fprintf(stderr, "\n");

	for (intKey * kp = ip->head[0]; kp; kp = kp->fwd[0])
	{
		indent("key", level+2);
		fprintf(stderr, "0x%08x, key=%d, ", (int)kp, kp->key);
		if (str)
		{
			giSChunk * cp = kp->schunk;
			unsigned z = cp->len;
			if (z > 32)
				z = 32;
			fprintf(stderr, "len=%d, %.*s%s",
				cp->len,
				z, &cp->data[0],
				z < cp->len ? " ..." : "");
			fprintf(stderr, "\n");
		}
		else
		{
			fprintf(stderr, "\n");
			giAChunk * cp = kp->achunk;
			for (int i = 0; i < giAChunk::items; i++)
				gistPrint(&cp->g[i], "", level+3);
		}
	}
}


static void
gistPrint(gist * gp, const char * label, int level)
{
	if (level >= 16)
	{
		indent(label, 16);
		fprintf(stderr, "...\n");
		return;
	}

	indent(label, level);

	switch (gp->typ)
	{
	case gist::GT_NIL:
		fprintf(stderr, "NIL\n");
		break;

	case gist::GT_INT:
		fprintf(stderr, "INT  %ld\n", gp->val);
		break;

	case gist::GT_FLOAT:
		fprintf(stderr, "FLOAT  %g\n", gp->dval);
		break;

	case gist::GT_STR:
		{
			giStr * sp = (giStr *)gp->intern;
			giIndexInt * ip = sp->index;
			giSChunk * cp = sp->chunk;

			fprintf(stderr, "STR  unique=%s, skip=%d, cnt=%d, "
					"intern=0x%08x, \n",
				gp->unique ? "true" : "false",
				gp->skip, gp->cnt, (int)sp);
			indent("    ", level);
			fprintf(stderr, "     "
					"index=0x%08x, data=0x%08x, "
					"size=%d, chunk=0x%08x\n",
				(int)ip,
				(int)sp->data, sp->size, (int)cp);

			if (!ip)
			{
				indent("data", level+1);
				unsigned z = gp->cnt;
				if (z > 32)
					z = 32;
				fprintf(stderr, "%.*s%s\n",
					z, &sp->data[gp->skip],
					z < gp->cnt ? " ..." : "");
			}
			else
				gistPrintIntIndex(ip, true, level+1);
		}
		break;

	case gist::GT_ARRAY:
		{
			giArray * ap = (giArray *)gp->intern;
			giIndexInt * ip = ap->index;

			fprintf(stderr, "ARRAY  unique=%s, skip=%d, cnt=%d, "
					"intern=0x%08x, \n",
				gp->unique ? "true" : "false",
				gp->skip, gp->cnt, (int)ap);
			indent("    ", level);
			fprintf(stderr, "     "
					"skip=%d, len=%d, "
					"index=0x%08x\n",
				ap->skip, ap->len,
				(int)ip);

			gistPrintIntIndex(ip, false, level+1);
		}
		break;

	case gist::GT_TABLE:
		fprintf(stderr, "TABLE\n");
		break;

	case gist::GT_PTR:
		fprintf(stderr, "PTR  0x%08x\n", (int)gp->ptr);
		break;

	case gist::GT_REGEX:
		fprintf(stderr, "REGEX\n");
		break;

	case gist::GT_FILE:
		{
			giFile * fp = (giFile *)gp->intern;
			fprintf(stderr, "FILE  unique=%s, intern=0x%08x, "
					"%c%c%c%c, fd=%d\n",
				gp->unique ? "true" : "false",
				(int)fp,
				fp->open ? 'o' : '-',
				fp->readable ? 'r' : '-',
				fp->writable ? 'w' : '-',
				fp->eof ? 'e' : '-',
				fp->fd);
			gistPrint(&fp->buffer, "buffer", level+1);
		}
		break;

	case gist::GT_CODE:
		fprintf(stderr, "CODE\n");
		break;

	case gist::GT_LONG:
		fprintf(stderr, "LONG\n");
		break;

	case gist::GT_REAL:
		fprintf(stderr, "REAL\n");
		break;

	}
}


void
GistPrint(gist * gp)
{
	gistPrint(gp, "gist", 0);
}
