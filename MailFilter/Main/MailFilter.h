/*
 *		MailFilter.h
 *		
 *		MailFilter Master Include File
 *		
 *		Copyright 2001-2004 Christian Hofstädtler.
 *		
 *		
 *		
 *		
 */

#ifdef WIN32
#undef N_PLAT_NLM
#endif

#ifndef _MAILFILTER_H_
#define _MAILFILTER_H_
#define _MAILFILTER_

#include "ix.h"			// include crossplatform library header

#include "MFApp.h"		// compile switches

#include "../Licensing/MFLic.h"

// MailFilter Constants etc.
//
// *Default Buffer Size*
#undef MAX_BUF
#define MAX_BUF 4096

#ifdef N_PLAT_NLM
#define _POSIX_SOURCE
#endif

// Just _some_ includes ;-)
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <sys/stat.h>

#include "streamprintf.h"

/* *****    N E T W A R E    ***** */
#ifdef N_PLAT_NLM
#define MAILFILTERPLATFORM "NLM"

#undef SEVERITY_WARNING		// header fixup
#undef SEVERITY_FATAL
#define _NWNSPACE_H_
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <nwsnut.h>
#undef SEVERITY_WARNING		// fixup 2.
#undef SEVERITY_FATAL


#ifndef __NOVELL_LIBC__		// stuff for clib/mflt50
#include <nwadv.h>
#include <unicode.h>
#include <nwerrno.h>
#include <nwcalls.h>
#include <nwclxcon.h>
#include <nwtypes.h>
#include <nwcalls.h>
#include <nwthread.h>
#include <nwtypes.h>
#include <nwconio.h>
#include <nwsemaph.h>
#include <nwclxcon.h>
#include <nwserver.h>
#include <nwconn.h>
#include <nwfinfo.h>
#include <nwfileio.h>
#include <nwfileng.h>
#include <nwcntask.h>
#include <nwdsapi.h>
#include <nwmalloc.h>
#include <nwlocale.h>
#include <nwdebug.h>
#include <sys/bsdskt.h>

#define NETDB_USE_INTERNET  1

#ifdef __cplusplus
//extern "C" { 
//	int snprintf  (     char* str,     size_t n, const char *  format,  ... ); 
//}
#endif // __cplusplus

	typedef unsigned char BOOL;

#else						// stuff for libc

	typedef int BOOL;

#include <netware.h>
#include <library.h>
#include <nks/thread.h>
#include <sys/utsname.h>
#include <fsio.h>
#include <screen.h>
#include <sys/select.h>
#endif

/* TCP/IP Support */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#undef WIN32

#ifndef HAVE_WINSOCK

#define SOCKET		unsigned int
#define	closesocket	close

#ifndef __NOVELL_LIBC__
static NETDB_DEFINE_CONTEXT
static NETINET_DEFINE_CONTEXT
#endif


#else

#define	sockaddr_in	SOCKADDR_IN
#define	sockaddr	SOCKADDR

#endif

  
// *Compatiblity*
//
// *Maximum Path, for NetWare*
#ifndef MAX_PATH
# define MAX_PATH 255//_MAX_SERVER+_MAX_VOLUME+_MAX_PATH+10
#endif
// *Generic Constants*
#define TRUE  1
#define FALSE 0
// *Windows Defines*
#undef HANDLE
#define HANDLE	int
// *Windows Macros*
#ifndef LOBYTE
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))
#endif
// *Some Windows Types*
#ifndef LONG
#define LONG unsigned long
#endif
#ifndef WORD
#define WORD unsigned short
#endif
#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef __RTAG_T
typedef LONG rtag_t;
#endif

// *End*


/*
#ifdef _MF_WITHOUTNUT
#undef NWSDisableAllInterruptKeys
#define NWSDisableAllInterruptKeys(a)	0
#undef NWSInitializeNut
#define NWSInitializeNut(fuck,utility,version,headerType,compatibilityLevel,messageTable,helpScreens,scr,resourceTag,handle)	0
#undef NWSShowPortalLine
#define NWSShowPortalLine
#undef NWSShowPortalLineAttribute
#define NWSShowPortalLineAttribute	0
#undef NWSCreatePortal
#define NWSCreatePortal(a,b,c,d,e,f,g,h,i,j,k,l,m,n)				0
#undef NWSDestroyPortal
#define NWSDestroyPortal			0
#undef NWSSelectPortal
#define NWSSelectPortal(a,b)		0
#undef NWSGetKey
#define NWSGetKey(a,b,c)			sleep(3)
#undef NWSUngetKey
#define NWSUngetKey(a,b,c)			0
#undef NWSEnableFunctionKey
#define NWSEnableFunctionKey(a,b)	0
#undef NWSDisableFunctionKey
#define NWSDisableFunctionKey(a,b)	0
#undef NWSDisableInterruptKey
#define NWSDisableInterruptKey(a,b)	0
#undef NWSGetPCB
#define NWSGetPCB(a,b,c)			0
#undef NWSShowLineAttribute
#define NWSShowLineAttribute(a,b,c,d,e,f)	0
#undef NWSUpdatePortal
#define NWSUpdatePortal(a)			0
#undef NWSDeselectPortal
#define NWSDeselectPortal(a)		0
#undef NWSEnableInterruptKey
#define NWSEnableInterruptKey(a,b,c)	0
#undef NWSScrollPortalZone
#define NWSScrollPortalZone(a,b,c,d,e,f,g,h)	0
#undef NWSClearPortal
#define NWSClearPortal(a)			0
#undef NWSRestoreNut
#define NWSRestoreNut(a)			0
#endif
*/


#endif /* N_PLAT_NLM */


/* *****       W I N 3 2   only     ***** */
#ifdef WIN32
#define MAILFILTERPLATFORM "WIN32"
#define _WIN32_WINNT 0x0400
#include <process.h>
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <dirent.h>
//#include <fcntl.h>
//#include <sys/types.h>
#include <stat.h>
#include <winsock2.h>
#include "resource.h"


/* Define Compatiblity things ...	*/

// open -> _open etc.
#define open			_open
#define delay(nTime)	Sleep(nTime)
#define _MAX_SERVER		(MAX_COMPUTERNAME_LENGTH + 1)		// From the Win32 Docs ...
#define _MAX_VOLUME		3		// C: ?

/* Metrowerks Win32 */
#ifdef __MSL__
#define strlwr			_strlwr
#define strdup			_strdup
#define memicmp			ix_memicmp
#endif // __MSL__

#define ThreadSwitch()

#define HAVE_WINSOCK
//#endif

#endif /* WIN32 */
/* *****  end     W I N 3 2   only   ***** */





#include "mfpcre.h"

// Include I18N header
#include "../Included/mime/comi18n.h"

// Include String Table
#include "mailflt.mlh"

// Define _MAILFILTER_MAIN_ to get host file of global variables ...
#ifdef _MAILFILTER_MAIN_		// MailFilter Global Storage

	#define _MFGS(_mfgs_type,_mfgs_name,_mfgs_default)	_mfgs_type _mfgs_name = _mfgs_default
	#define _MAILFILTER_MFCONFIG_STORAGE

	// (NUT) String Table
	#ifndef MF_WITH_I18N
		#include "mailflt.mlc"
	#else
		char **programMesgTable;
	#endif

	#ifdef N_PLAT_NLM
		// NWSNUT State Information for MAIN THREAD ONLY!
		 NUTInfo *MF_NutInfo			= NULL;
		 static LONG allocTag;
	#endif // N_PLAT_NLM

	char*	MF_ProductName;

	#ifdef __cplusplus
		#include "MFConfig.h++"
		MailFilter_Configuration::Configuration *MF_GlobalConfiguration;
	#endif

#else // _MAILFILTER_MAIN_

	#define _MFGS(_mfgs_type,_mfgs_name,_mfgs_default)	extern _mfgs_type _mfgs_name

	extern char *MF_ProductName;
	
	// (NUT) String Table
	extern char *programMesgTable[];


	#ifdef N_PLAT_NLM
		 extern NUTInfo *MF_NutInfo;
	#endif

	#ifdef __cplusplus
		#include "MFConfig.h++"
		extern MailFilter_Configuration::Configuration *MF_GlobalConfiguration;
	#endif

#endif // _MAILFILTER_MAIN_


// Temporary Configuration Storage
#ifdef N_PLAT_NLM

	// NLM Data ...
#ifndef __NOVELL_LIBC__
	_MFGS(	unsigned int,	MF_NLMHandle,						0		);
	_MFGS(	int,			mainThread_ThreadGroupID,			1		);
	_MFGS(	int,			MF_ScreenID,						0		);
	_MFGS(	int,			MFD_ScreenID,						NULL	);
#else
	_MFGS(	void*,			MF_NLMHandle,						0		);
	_MFGS(	void*,			MF_ScreenID,						0		);
	_MFGS(	void*,			MFD_ScreenID,						NULL	);
	_MFGS(	rtag_t,			MFD_ScreenTag,						NULL	);
#endif


	// DS Handles ...
#ifndef __NOVELL_LIBC__
	_MFGS(	NWDSContextHandle, MFT_NLM_DS_Context, 				NULL	);
	_MFGS(	LONG,			MFT_NLM_Connection_HandleCLIB,		NULL	);
	_MFGS(	NWCONN_HANDLE,	MFT_NLM_Connection_Handle,			NULL	);
#endif
#endif
	// Temporary Configuration Data	(MFT_)
	_MFGS(	bool,			MFT_InOutSameServerVolume,			false	);
	_MFGS(	bool,			MFT_RemoteDirectories,				false	);
	_MFGS(	bool,			MFT_AVAVerbose,						false	);
	_MFGS(	int,			MFT_Debug,							0		);
	_MFGS(	unsigned long,	MFT_I18N_MessageCount,				0		);
	_MFGS(	unsigned long,	MFT_I18N_LanguageID,				0		);
	_MFGS(	bool,			MFC_MAILSCAN_Enabled,				0		);
	_MFGS(	bool,			MFT_bTriedAVInit,					false	);
	_MFGS(	bool,			MFT_bStartupComplete,				false	);

	_MFGS(	int,			MFD_AllocCounts,					0		);
	_MFGS(	int,			MFD_AllocCountsCPP,					0		);

	// Thread Handles
#ifdef __NOVELL_LIBC__
	_MFGS(	NXThreadId_t,	MF_Thread_Work,						0		);
	_MFGS(	NXThreadId_t,	MF_Thread_NRM,						0		);
	_MFGS(	NXThreadId_t,	MF_Thread_SMTP,						0		);
	_MFGS(	NXThreadId_t,	MF_Thread_Cleanup,					0		);
#else
	_MFGS(	HANDLE,			MF_Thread_Work,						0		);
	_MFGS(	HANDLE,			MF_Thread_NRM,						0		);
	_MFGS(	HANDLE,			MF_Thread_SMTP,						0		);
	_MFGS(	HANDLE,			MF_Thread_Cleanup,					0		);
#endif

	// When were we last alive ?
	_MFGS(	int,			MF_LastAlive,						1		);

	// Counters ...	(MFS_)
	_MFGS(	LONG,			MFS_MF_MailsInputTotal,				0		);
	_MFGS(	LONG,			MFS_MF_MailsInputFailed,			0		);
	_MFGS(	LONG,			MFS_MF_MailsOutputTotal	,			0		);
	_MFGS(	LONG,			MFS_MF_MailsOutputFailed,			0		);

	// Cleanup Counters
	_MFGS(	int,			MFT_NLM_ThreadCount,				0		);
	_MFGS(	int,			MFT_NLM_Exiting,					0		);


#ifdef _TRACE
	//DEBUG
	_MFGS(	long,			MFD_StackPointer,					0		);
#endif

#ifdef N_PLAT_NLM
#ifdef __NOVELL_LIBC__
#define ThreadSwitch	NXThreadYield
#define ConsolePrintf	consoleprintf
#define _MF_NUTCHAR		char*
#else
#define NXThreadYield	ThreadSwitch
#define consoleprintf	ConsolePrintf
#define mkdir(a,b)		mkdir(a)
#define _MF_NUTCHAR		unsigned char*
#define ActivateScreen	DisplayScreen
#endif
#endif

#define MFUI_KEYS_NONE			0x0000
#define MFUI_KEYS_IMPORT		0x0001
#define MFUI_KEYS_EXPORT		0x0002
#define MFUI_KEYS_NEW			0x0004
#define MFUI_KEYS_DELETE		0x0008
#define MFUI_KEYS_SELECT		0x0010
#define MFUI_KEYS_EXIT			0x0020
#define MFUI_KEYS_SAVE			0x0040
#define MFUI_KEYS_CANCEL		0x0080
#define MFUI_KEYS_SHOW			0x0100
#define MFUI_KEYS_SORT			0x0200

#define MFD_SOURCE_GENERIC		0x0001
#define MFD_SOURCE_WORKER		0x0002
#define MFD_SOURCE_CONFIG		0x0004
#define MFD_SOURCE_VSCAN		0x0008
#define MFD_SOURCE_ZIP			0x0010
#define MFD_SOURCE_ERROR		0x0020
#define MFD_SOURCE_RULE			0x0040
#define MFD_SOURCE_REGEX		MFD_SOURCE_RULE
#define MFD_SOURCE_MAIL			0x0080
#define MFD_SOURCE_SMTP			0x0100

// Debug Prototypes
#ifdef _MF_CLEANBUILD

	// non-debug build.
	#undef MFT_AVAVerbose
	#undef MFT_Debug
	#define MFT_AVAVerbose	0
	#define MFT_Debug	0

	#define MFD_Out(...)

#else
	// Debug build.
	void MFD_Out_func(int attr, const char* fmt, ...);
	
	#define MFD_Out					MFD_Out_func

#endif

#include "MFMalloc.h"

// Message Support Macro
#define MF_Msg(id)							programMesgTable[id]
#define MF_NMsg(id)							(_MF_NUTCHAR)programMesgTable[id]
#define MF_DisplayCriticalErrorId(id)		MF_DisplayCriticalError(MF_Msg(id))
#define MF_StatusNothing(void)				MF_StatusText("")

// MFL Prototypes
int MFL_Init(int rLvl);
bool MFL_GetFlag(int flag);
extern int MFL_Certified;

// Macros

// checks if contained has flag -- OR-ing required -- returns 1 if flag is contained, 0 otherwise
#define chkFlag(contained,flag)		((contained | flag) == contained)


// Prototypes
void MF_ShutDown(void);
//int MF_ConfigReadString(char ConfigFile[MAX_PATH], int Entry, char Value[]);
void MF_ExitProc(void);
void MF_StatusText( const char *newText );
void MF_MakeValidPath(char* thePath);
void MF_GetServerName(char* serverName, unsigned long bufSize);
void MF_StatusUI_Update(const char* newText);
void MF_StatusUI_UpdateLog(const char* newText);
#ifdef __cplusplus
void MF_StatusUI_UpdateLog(int newText);
#endif
void MF_StatusLog( const char *newText );
bool MF_StatusInit(void);
void MF_StatusFree(void);
int MF_MoveFileToFile(const char *fileIn, const char *workingFile, int beVerbose);
int MF_MoveFileToDir(const char *fileIn, const char *destDir, int beVerbose);
bool MF_Util_MakeUniqueFileName(char* fileName, int addVal);
bool MF_StatusCycleLog(void);
int MF_CheckPathSameServer(char* thePath1 , char* thePath2);
int MF_CheckPathSameVolume(char* thePath1 , char* thePath2);
void MFL_VerInfo();
void MF_OutOfMemoryHandler(void);
int MF_EMailPostmasterGeneric(const char* Subject, const char* Text, const char* szAttachFileName, const char* szAttachDisplayName);
void MF_UI_ShowConfiguration(void);
int MFAPI_FilterCheck( char *szScan , int mailSource , int matchfield );
bool MFBW_CheckCurrentScheduleState();
int MF_CountFilters(int action);
int MF_CountAllFilters();
void MF_CheckProblemDirAgeSize();
bool MF_NutInit(void);
bool MF_NutDeinit(void);
int MFUtil_CheckCurrentVersion();

bool MF_UI_ShowKeys(int keys);

bool MF_LoginUser(void);

bool MailFilterApp_Server_SelectServerConnection(void);
bool MailFilterApp_Server_SelectUserConnection(void);

bool MailFilterApp_Server_LoginToServer(void);
bool MailFilterApp_Server_LogoutFromServer(void);


extern "C" {
	void MF_DisplayCriticalError(const char* format, ...);
}


#ifdef HAVE_WINSOCK

bool WinSockStartup();
bool WinSockShutdown();

#else

__inline bool WinSockStartup()		{ return true; }
__inline bool WinSockShutdown()		{ return true; }

#endif /* HAVE_WINSOCK */


// Thread Functions
#ifndef WIN32
void MF_Work_Startup(void *dummy);
void MF_SMTP_Startup(void *dummy);
#else
DWORD WINAPI MF_Work_Startup(void *dummy);
DWORD WINAPI MF_SMTP_Startup(void *dummy);
#endif // WIN32

#define ERROR_SUCCESS	0

#undef consoleprintf
#undef ConsolePrintf

#endif	// _MAILFILTER_H_

/* eof */
