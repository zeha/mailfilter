#ifndef _IX_H_INCLUDED
#define _IX_H_INCLUDED 1

#ifndef __cplusplus
#ifndef bool
#define bool BOOL
#endif
#endif

// platform selection
#ifdef WIN32
#undef  IXPLAT_DOS
#undef  IXPLAT_NETWARE
#undef  IXPLAT_NETWARE_CLIB
#undef  IXPLAT_NETWARE_LIBC
#undef  IXPLAT_UNIX
#define IXPLAT_WINDOWS			1
#define IXPLAT_WIN32			1
#undef  IXPLAT_WIN64
#define _WIN32_WINNT 0x0400
#endif // WIN32
#ifdef WIN64
#undef  IXPLAT_DOS
#undef  IXPLAT_NETWARE
#undef  IXPLAT_NETWARE_CLIB
#undef  IXPLAT_NETWARE_LIBC
#undef  IXPLAT_UNIX
#define IXPLAT_WINDOWS			1
#undef  IXPLAT_WIN32
#define IXPLAT_WIN64			1
#define _WIN32_WINNT 0x0400
#endif // WIN64
#ifdef N_PLAT_NLM
#undef  IXPLAT_DOS
#define IXPLAT_NETWARE			1
#define IXPLAT_NETWARE_CLIB		1
#undef  IXPLAT_NETWARE_LIBC
#undef  IXPLAT_UNIX
#undef  IXPLAT_WIN32			
#undef  IXPLAT_WIN64
#endif // N_PLAT_NLM
#ifdef __NOVELL_LIBC__
#undef  IXPLAT_DOS
#define IXPLAT_NETWARE			1
#undef  IXPLAT_NETWARE_CLIB
#define IXPLAT_NETWARE_LIBC		1
#undef  IXPLAT_UNIX
#undef  IXPLAT_WIN32			
#undef  IXPLAT_WIN64
#endif // __NOVELL_LIBC__
#if defined(M_UNIX) || defined(LINUX) || defined(linux)
#undef  IXPLAT_DOS
#undef  IXPLAT_NETWARE
#undef  IXPLAT_NETWARE_CLIB
#undef  IXPLAT_NETWARE_LIBC
#define IXPLAT_UNIX				1
#undef  IXPLAT_WIN32			
#undef  IXPLAT_WIN64
#endif // UNIX



typedef int IX_Bool;
#define IX_TRUE 1
#define IX_FALSE 0

#define IX_CR '\r'
#define IX_LF '\n'

typedef enum { IX_FAILURE = -1, IX_SUCCESS = 0 } IXStatus;

//#ifdef _IX_LIBRARY
#include <dirent.h>
#include <unistd.h>

#ifdef IXPLAT_WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <winsock2.h>
#	include <errno.h>
#	include <time.h>
#	include <stdlib.h>
#else
#	include <sys/time.h>
#endif

//#endif

#define IX_PATH_MAX					255

#ifdef IXPLAT_NETWARE
#define IX_DIRECTORY_SEPARATOR			'\\'
#define IX_DIRECTORY_SEPARATOR_STR		"\\"
#define IX_PATH_SEPARATOR				';'
#define IX_PATH_SEPARATOR_STR			";"
#endif // IXPLAT_NETWARE

#ifdef IXPLAT_UNIX
#define IX_DIRECTORY_SEPARATOR			'/'
#define IX_DIRECTORY_SEPARATOR_STR		"/"
#define IX_PATH_SEPARATOR				':'
#define IX_PATH_SEPARATOR_STR			":"
#endif // IXPLAT_UNIX

#ifdef IXPLAT_WIN32
#define IX_DIRECTORY_SEPARATOR			'\\'
#define IX_DIRECTORY_SEPARATOR_STR		"\\"
#define IX_PATH_SEPARATOR				';'
#define IX_PATH_SEPARATOR_STR			";"
#endif // IXPLAT_WIN32


#ifndef _IX_LIBRARY
#define IXApi(ret)	extern ret 
#define IXDefault(value)
#else
#define	IXApi(ret)	ret
#define IXDefault(value)  = value
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// getopt
IXApi(int)				ix_opterr;
IXApi(int)				ix_optind;
IXApi(int)				ix_optopt;
IXApi(char*) 			ix_optarg;
IXApi(IXStatus)			ix_getopt_init();
IXApi(int)				ix_getopt(int argc, char **argv, char *opts);

// memicmp
IXApi(int)				ix_memicmp (const void * first, const void * last, unsigned int count);

/*
// directory functions
IXApi(IXStatus)			ix_delete(const char *name);
IXApi(IXStatus) 		ix_diropen(struct IX__Dir *d, const char *name);
IXApi(IXStatus) 		ix_dirclose(struct IX__Dir *d);
IXApi(char*)			ix_dirread(struct IX__Dir *d, IX_DirFlags flags);
IXApi(char*)			ix_dirread_83(struct IX__Dir *d, IX_DirFlags flags);
IXApi(struct dirent *)	ix_direntry(struct IX__Dir *d, IX_DirFlags flags);
*/

// checksums
IXApi(unsigned long)	ix_adler32(unsigned long adler, const unsigned char *buf, unsigned int len);

#ifdef __cplusplus
}


	#include "iXList.h"
	#include "iXDir.h"
#endif


#endif //!_IX_H_INCLUDED

