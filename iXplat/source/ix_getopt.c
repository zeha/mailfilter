/*
 * ix_getopt.c
 *
 * This Code is public domain, adopted for use within iXplat.
 *
 * 
 * Modifications Copyright 2002 Christian Hofstaedtler.
 * 
*/ 

#define _IX_LIBRARY
#include "ix.h"

#include <unistd.h>
#include <string.h>

#define IX_GETOPT_NULL	0
#define IX_GETOPT_EOF	(-1)
#define IX_GETOPT_ERR(s, c)	if(ix_opterr){\
	char errbuf[2];\
	errbuf[0] = c; errbuf[1] = '\n';\
	(void) write(2, argv[0], (unsigned)strlen(argv[0]));\
	(void) write(2, s, (unsigned)strlen(s));\
	(void) write(2, errbuf, 2);}

//int	ix_opterr = 1;
//int	ix_optind = 1;
//int	ix_optopt;
//char *ix_optarg;

IXStatus ix_getopt_init()
{
	ix_opterr = 1;
	ix_optind = 1;
	return IX_SUCCESS;
}

int ix_getopt(int argc, char **argv, char *opts)
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1)
		if(ix_optind >= argc ||
		   argv[ix_optind][0] != '-' || argv[ix_optind][1] == '\0')
			return(IX_GETOPT_EOF);
		else if(strcmp(argv[ix_optind], "--") == IX_GETOPT_NULL) {
			ix_optind++;
			return(IX_GETOPT_EOF);
		}
	ix_optopt = c = argv[ix_optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == IX_GETOPT_NULL) {
		IX_GETOPT_ERR(": illegal option -- ", c);
		if(argv[ix_optind][++sp] == '\0') {
			ix_optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[ix_optind][sp+1] != '\0')
			ix_optarg = &argv[ix_optind++][sp+1];
		else if(++ix_optind >= argc) {
			IX_GETOPT_ERR(": option requires an argument -- ", c);
			sp = 1;
			return('?');
		} else
			ix_optarg = argv[ix_optind++];
		sp = 1;
	} else {
		if(argv[ix_optind][++sp] == '\0') {
			sp = 1;
			ix_optind++;
		}
		ix_optarg = IX_GETOPT_NULL;
	}
	return(c);
}

/* eof */

