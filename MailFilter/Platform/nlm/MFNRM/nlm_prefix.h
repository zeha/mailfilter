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
#define N_PLAT_NLM

/* the FAR keyword has no meaning in a 32-bit environment 
   but is used in the SDK headers so we take it out */
#define FAR
#define far

/* no-op for Codewarrior C compiler; a functions are cdecl 
   by default */
#define cdecl

/* if we have wchar_t enabled in C++, predefine this type to avoid
   a conflict in Novell's header files */
#if (__option(cplusplus) && __option(wchar_type))
#define _WCHAR_T
#endif

/* C9X defintion used by MSL C++ library */
#define DECIMAL_DIG 17

/* define long long typedefs for Watcom compatiblity */
typedef long long int64_t;
typedef unsigned long long uint64_t;

/* some code may want to use the MS convention for long long */
#ifndef __int64
#define __int64 long long
#endif

#endif

/*
 * Change record
 * blc 981229 Fixed comments to be in C89 format
 * blc 990125 Added int64_t and uint64_t typedefs
 * ejs 010907 Removed __NETWARE__ #define (compiler defines this)
 */
