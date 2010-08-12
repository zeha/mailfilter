/* Metrowerks Standard Library
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 18:23:45 $
 * $Revision: 1.1 $
 */
 
/*
 *		This header provides various runtime declarations.
 */
 
#ifndef _MSL_CRTL_H
#define _MSL_CRTL_H

#include <ansi_parms.h>

_MSL_BEGIN_EXTERN_C

#if (__dest_os	== __win32_os )

	/* This flag is set when a DLL is running terminate code */
	extern int _DllTerminate;		/*- KO 961218 -*/ 

	/* Environment pointers (null-terminated list of "var=value" strings) */
	#define _Environ _environ	/* old MSL name */
	#define environ _environ	/* UNIX compatibility name */
	extern _MSL_IMP_EXP_RUNTIME char **_environ;

	/* Argument list variables (null-terminated list of arguments) */
	extern _MSL_IMP_EXP_RUNTIME char **__argv;
	extern _MSL_IMP_EXP_RUNTIME int __argc, __argv1_offs;

	/* Structure alocated for each ed file handle */
	typedef struct
	{
	    void *handle;
	    char translate;
	    char append;
	} FileStruct;

	/* Pointers to file structure indexed by the handle */
	#define NUM_HANDLES 256

	/* The table of handles */
	extern FileStruct *_HandleTable[NUM_HANDLES];
	extern int _HandPtr;

	/* functions */
	extern char __xc_a;
	_MSL_IMP_EXP_RUNTIME extern void _MSL_CDECL _RunInit(char *funcsptr);
	extern void _MSL_CDECL _SetupArgs(void);
	extern void _MSL_CDECL _SetupEnv(void);
	
	_MSL_IMP_EXP_RUNTIME extern int _MSL_CDECL _CRTStartup(void);

#elif __dest_os == __netware_os

	extern char ___xc_a;
	_MSL_IMP_EXP_RUNTIME extern void _MSL_CDECL _RunInit(char *funcsptr);

#endif /* __dest_os == __win32_os */
	
	/* 	detect CPU instruction set for Pentium or higher: returns bitmask of _CPU_FEATURE_xxx 
		(from CPUID function 1 for non-AMD chips or CPUID function 0x80000001 for AMD chips) */

	#define _CPU_FEATURE_AMD_3DNOW	0x80000000	// from extended flags
	#define _CPU_FEATURE_SSE		0x02000000
	#define _CPU_FEATURE_SSE_2		0x04000000
	#define _CPU_FEATURE_MMX		0x00800000
	#define _CPU_FEATURE_CMOV_FCOMI	0x00008000

	unsigned long _MSL_CDECL __detect_cpu_instruction_set(void);
	void _MSL_CDECL __destroy_global_chain(void);
	int _MSL_CDECL __register_atexit(void (*func)(void));

_MSL_END_EXTERN_C

#endif /*  _MSL_CRTL_H  */

/* Change record:
 * KO  961218 Moved the definition of _DllTerminate to dllmaincrt.c and moved the
 *			  declaration here.
 * hh  971206 Added #include <ansi_parms.h>
 * hh  971206 Added namespace support
 * cc  000316 added _MSL_IMP_EXP_C
 * cc  010409 updated to new namespace macros 		  
 * ejs 011031 added SSE-2 feature in CPU detection
 * ejs 011116 change obsolete/incompatible environment string accessors into macros
 * cc  011120 added _MSL_IMP_EXP_RUNTIME to _SetupEnv 
 * ejs 011120 Moved _SetupEnv/_SetupArgs into DLL (removed IMP_EXP)
 * cc  011203 Added _MSL_CDECL for new name mangling 
 */