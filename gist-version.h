/*
 *	Gist version.
 */

#ifndef __GIST_VERSION_H__
#define __GIST_VERSION_H__

/**********************************************************************/

#define VERSION_VERSION		1
#define VERSION_RELEASE		0
#define VERSION_REVISION	967

#define VERSION		((VERSION_VERSION << 16) |	\
			 (VERSION_RELEASE << 8) |	\
			 (VERSION_REVISION))

#define VERSION_STRING2(r,l,v)	#r "." #l "." #v
#define VERSION_STRING1(r,l,v)	VERSION_STRING2(r,l,v)
#define VERSION_STRING		VERSION_STRING1(VERSION_VERSION,	\
						VERSION_RELEASE,	\
						VERSION_REVISION)

/**********************************************************************/

#endif // __GIST_VERSION_H__
