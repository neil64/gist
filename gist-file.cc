/*
 *	Gist -- files.
 *
 *	Contains:	file(), read(), readline(), write(), flush(),
 *			close().
 */

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>

#include	"gist.h"
#include	"gist-internal.h"


/**********************************************************************/

void
gist::file(const char * fn, const char * mode)
{
	giFile * fp;

	/*
	 *	Make sure the object is a file object.
	 */
	if (typ != GT_PTR)
	{
		typ = GT_FILE;
		fp = new giFile;
		fp->open = false;
		intern = fp;
	}
	else
		fp = (giFile *)intern;

	/*
	 *	Close the file if it is currently open.
	 */
	if (fp->open)
		close();

	/*
	 *	If there is no new file name, we are done.  We are supposed
	 *	to leave the object as a closed file object.
	 */
	if (!fn || !*fn)
		return;

	/*
	 *	Figure out the file modes.
	 */
	fp->readable = true;
	fp->writable = false;
	bool create = false;
	bool truncate = false;
	bool append = false;

	if (mode[0] == 'r')
	{
		if (mode[1] == '+')
			fp->writable = true;
		else if (mode[1] != '\0')
  badMode:
			throw valueError("file mode");
	}
	else if (mode[0] == 'w')
	{
		fp->readable = false;
		fp->writable = true;
		create = true;
		truncate = true;

		if (mode[1] == '+')
			fp->readable = true;
		else if (mode[1] != '\0')
			goto badMode;
	}
	else if (mode[0] == 'a')
	{
		fp->readable = false;
		fp->writable = true;
		create = true;
		append = true;

		if (mode[1] == '+')
			fp->readable = true;
		else if (mode[1] != '\0')
			goto badMode;
	}
	else if (mode[0] != '\0')
		goto badMode;

	int md;
	if (fp->readable)
		if (fp->writable)
			md = O_RDWR;
		else
			md = O_RDONLY;
	else if (fp->writable)
		md = O_WRONLY;
	else
		throw internalError("file mode");

	if (create)
		md |= O_CREAT;
	if (truncate)
		md |= O_TRUNC;
	if (append)
		md |= O_APPEND;

	/*
	 *	Try to open (create, truncate, append) the file.
	 */
	int fd = ::open(fn, md);
	if (fd == -1)
		if (errno == ENOENT)
			throw noSuchFileError("file not found");
		else
			throw ioError("can't open");

	/*
	 *	All is well.  Tidy up.
	 */
	fp->open = true;
	fp->eof = false;
	fp->fd = fd;
	fp->buffer.set("");
}


void
gist::file(const gist & fn, const char * mode)
{
	file(fn.CCS(), mode);
}


void
gist::file(const char * fn, const gist & mode)
{
	file(fn, mode.CCS());
}


void
gist::file(const gist & fn, const gist & mode)
{
	file(fn.CCS(), mode.CCS());
}


void
gist::close()
{
	throw notYetError("close");
}


gist
gist::read(unsigned amount)
{
	if (typ != GT_FILE)
		throw typeError("file read");

	gist r = "";
	giFile * fp = (giFile *)intern;

	if (!fp->readable)
		throw fileError("not readable");

	while (amount > 0)
	{
		if (fp->buffer.cnt == 0)
		{
			if (fp->eof)
  eof:
				throw eofError("eof");

			/*
			 *	Read some more from the file.
			 */
			char * buf = fp->buffer.strbuf(giFile::readBufferSize);

			int x = ::read(fp->fd, buf, giFile::readBufferSize);
			if (x < 0)
			{
				fp->buffer.set("");
				throw ioError("read error");
			}

			if (x == 0)
			{
				fp->buffer.set("");
				fp->eof = true;
				goto eof;
			}

			fp->buffer.cnt = x;
		}

		/*
		 *	Copy some data from the buffer to the return string.
		 *	(This is not a memory to memory copy;  it is setting
		 *	 up multiple strings to point to the same buffer.)
		 */
		unsigned x = fp->buffer.cnt;
		if (x > amount)
			x = amount;

		r.strcat(fp->buffer.substr(0, x));
		fp->buffer.strtrim(x);
		amount -= x;
	}

	return r;
}
