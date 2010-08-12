#ifndef __prelude_h__
#define __prelude_h__
/*============================================================================
=	NetWare Loadable Modules / Server Library Internal Header
=
=	Unpublished Copyright (C) 1996-1997 by Novell, Inc. All rights reserved.
=
=	No part of this file may be duplicated, revised, translated, localized or
=	modified in any manner or compiled, linked or uploaded or downloaded to or
=	from any computer system without the prior written consent of Novell, Inc.
==============================================================================
=							I n t e r n a l   H e a d e r
=
=  prelude.h
==============================================================================
*/
#define TERMINATE_BY_EXTERNAL_THREAD	0
#define TERMINATE_BY_UNLOAD				5

#define TRADITIONAL_NLM_INFO_SIGNATURE	0x00000000
#define NLM_INFO_SIGNATURE					0x494D4C4E	/* 'NLMI' */

#define TRADITIONAL_FLAVOR					0

/* versions within the traditional flavor: */
#define TRADITIONAL_VERSION				0
#define LIBERTY_VERSION						1
#define NPL_THREADS							2

/*
** The following structure is used to provide:
** 1) SDK version information the NLM C libraries,
** 2) certain compiler characteristics which need to be known to the
**		standard C library, and
** 3) a mechansim to pass the NLMID from _StartNLM() to _TerminateNLM().
**
** In previous Prelude source modules, only #3 was provided for. Now a
** flexible mechanism works correctly with a larger number of compilers
** and allows the NLM C library user to specify what 'flavor' and version
** differences to expect in the NLM C library.
*/
typedef union
{
	LONG	signature;					/* indicates NLM historical ties... */
	int	ID;							/* 0 provides for backward compatibility */
} NLMSig;								/* with older versions of CLib.NLM */

typedef struct NLMInfo
{
	NLMSig	sig;						/* absolutely must be first! */
	LONG		flavor;					/* indicates major modality differences */
	LONG		version;					/* indicates minor deviation within flavor */
	size_t	sizeof_long_double;
	size_t	sizeof_wchar_t;
} NLMInfo;

#ifdef __cplusplus
extern "C"
{
#endif

/*
** _Prelude() is the default name for the NLM Start function. Start() receives
** the parameters shown, all of which must be passed on to _StartNLM().
*/
LONG	_Prelude( void *NLMHandle, void *errorScreen, BYTE *cmdLine,
			BYTE *loadDirPath, LONG uninitializedDataLength, LONG NLMFileHandle,
			LONG (*readRoutineP)(), LONG customDataOffset, LONG customDataSize );

/*
** _Stop() is the default name for the NLM Stop function.
*/
void	_Stop ( void );

/*
** _StartNLM() sets up the context under which userStartFunc() will begin
** running. To support the ANSI C "hosted" execution model, the function
** called at at program startup is main(), which receives 'argc' and 'argv.'
** The traditional way to accomplish this is by calling _SetupArgv() with
** main as its parameter in userStartFunc(). It also provides for some
** parameter preprocessing, to wit: it handles (CLIB_OPT). Alternate methods
** for calling main() or for calling another function as the standalone
** start-up are possible.
** NOTE: userStartFunc() will run after _StartNLM() has returned if the
** synchronization load flag has not been set. If that flag is set then the
** XXXX function call must be made before _StartNLM() returns successfully.
** The integer value returned by userStartFunc()--when it is allowed to run
** to completion--is the exit code for the NLM.
*/
LONG	_StartNLM( void *NLMHandle, void *initErrorScreenID, BYTE *cmdLineP,
			BYTE *loadDirectoryPath, LONG uninitializedDataLength,
			LONG NLMFileHandle, LONG (*readRoutineP)(), LONG customDataOffset,
			LONG customDataSize, NLMInfo *NLMInformation, int (*userStartFunc)() );

int	_StartNPL( void *NLMHandle, void *initErrorScreenID, BYTE *cmdLineP,
			BYTE *loadDirectoryPath, LONG uninitializedDataLength,
			LONG NLMFileHandle, LONG (*readRoutineP)(), LONG customDataOffset,
			LONG customDataSize, NLMInfo *NLMInformation, int (*userStartFunc)() );

/*
** After a successful call to _StartNLM(), _TerminateNLM() must be called in
** order for the NLM to unload properly. The NLMID returned from _StartNLM()
** is passed as the first parameter. The second parameter should have the
** value TERMINATE_BY_EXTERNAL_THREAD and the third should have the parameter
** TERMINATE_BY_UNLOAD. Currently _TerminateNLM() always returns 0.
*/
LONG	_TerminateNLM ( void *NCSp, void *threadID, int status );
int	_TerminateNPL ( void *NCSp, void *threadID, int status );

/*
** In order to use a shared C library other than Novell's default standard C
** library, _NWRegisterNLMLibraryUser() must be called before any facilities
** of either the default or selected shared standard C library are used. Some
** mechanism must be provided by the selected shared C library to provide that
** library's handle. That handle, along with the NLMID returned by _StartNLM()
** are passed to _NWRegisterNLMLibraryUser(). A nonzero return indicates an
** error occurred registering that library as this NLM's C library.
*/
int	_NWRegisterNLMLibraryUser ( int NLMID, int libraryHandle );

/*
** This is the traditional function called to provide the 'argc' and 'argv'
** parameters for a call to main(). In addition it handles the (CLIB_OPT)
** parameters. It is typically called from userStartFunc() and passed to
** _StartNLM() which is run as the NLM's first thread.
** _SetupArgv() is called from _cstart_() in Prelude.Obj and _SetupArgV_411()
** from NWPre.Obj. New NLMs wanting full ANSI compliance and command-line
** functionality should use NWPre.Obj.
*/
int	_SetupArgv ( int (*main)(int arg, char *argv[]) );
int	_SetupArgV_411 ( int (*main)(int arg, char *argv[]) );

#ifdef __cplusplus
}
#endif

#endif
