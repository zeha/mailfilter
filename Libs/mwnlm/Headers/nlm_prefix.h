/*  Copyright © 1998-2001 Metrowerks, Inc.
 *  All rights reserved.
 *
 *	nlm_prefix.h
 *
 *	This is a prefix file to define symbols required to compile
 *	some of the NetWare SDK examples
 */
 
#ifndef __nlm_prefix__
#define __nlm_prefix__
 
/* defines target type */
#ifndef N_PLAT_NLM
#define N_PLAT_NLM
#endif

/* the FAR keyword has no meaning in a 32-bit environment 
   but is used in the SDK headers so we take it out */
#ifndef FAR
#define FAR
#endif
#ifndef far
#define far
#endif

/* no-op for Codewarrior C compiler; a functions are cdecl 
   by default */
#ifndef cdecl
#define cdecl
#endif

/* if we have wchar_t enabled in C++, predefine this type to avoid
   a conflict in Novell's header files */
#if !defined(_WCHAR_T) && (__option(cplusplus) && __option(wchar_type))
#define _WCHAR_T
#endif


/* some code may want to use the MS convention for long long */
/* mailfilter: we do */
#ifndef __int64
#define __int64 long long
#endif
#endif

/*
 * Change record
 * blc 981229 Fixed comments to be in C89 format
 * blc 990125 Added int64_t and uint64_t typedefs
 * ejs 010907 Removed __NETWARE__ #define (compiler defines this)
 * ejs 021120 Add #ifndefs around macros
 */
