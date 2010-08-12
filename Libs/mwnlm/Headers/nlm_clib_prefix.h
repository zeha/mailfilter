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

/* using CLib, not LibC */
#undef _NLM_USING_CLIB
#define _NLM_USING_CLIB 1
 
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

/* turn off use of wchar.h for CLib (nwsdk) */
#define _MSL_NO_WCHART_C_SUPPORT

/* C9X defintion used by MSL C++ library */
#ifndef DECIMAL_DIG
#define DECIMAL_DIG 17
#endif

/* some code may want to use the MS convention for long long */
#ifndef __int64
#define __int64 long long
#endif

#endif

/*
 * Change record
 * ejs 030523 Added for CLib support
 */
