/*
 *		MailFilter.h
 *		
 *		MailFilter/ax Master Include File
 *		
 *		Copyright 2001-2004 Christian Hofstädtler.
 *		
 *		
 *		- 17/Aug/2001 ; ch   ; Moved from MailFilter.cpp
 *		
 *		
 */

#ifdef WIN32
#undef N_PLAT_NLM
#endif

#ifndef _MAILFILTER_H_
#define _MAILFILTER_H_
#define _MAILFILTER_

#include "ix.h"			// include ionus crossplatform library header

#include <MFMail.h++>
#include <MFVersion.h>
#include "../Licensing/MFLic.h"

//*EXPERIMENTAL*
#undef MF_WITH_ZIP


// Define this for tracing ...
#undef _TRACE
#undef _MF_WITHOUTNUT

// set this to #define _MF_MEMTRACE 1 and you will get much output :)  (~ 9k)
// also define _trace
//#define _MF_MEMTRACE 1

// Define for use of .msg files
#undef MF_WITH_I18N

// To check for obsolete NDK headers ...
//#define _FIND_OLD_HEADERS_

// Enable this to compile without debug messages, etc ...	(~ 5k)
#undef _MF_CLEANBUILD

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
//#include "tchar.h"
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <sys/stat.h>

/* *****    N E T W A R E     ***** */
#ifdef N_PLAT_NLM

#undef SEVERITY_WARNING
#undef SEVERITY_FATAL

#define MAILFILTERPLATFORM "NLM"
#define _NWNSPACE_H_
#include <errno.h>
#include <unistd.h>
#include <nwsnut.h>
#include <dirent.h>
#include <fcntl.h>

#undef SEVERITY_WARNING
#undef SEVERITY_FATAL

#ifndef __NOVELL_LIBC__
#include <unicode.h>
#include <nwadv.h>
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
// #include <nwsmp.h>
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
extern "C" { 
	int snprintf  (     char* str,     size_t n, const char *  format,  ... ); 
//	#define snprint	LIBC@snprintf
}
#endif // __cplusplus

#else
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

/*
// Windows and Winsock2 Includes.
#define _INC_WINDOWS
#define UINT int
#include <basetsd.h>
#include <ws2nlm.h>


#define HAVE_WINSOCK
*/

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
//#ifndef NULL
//#define NULL 0
//#endif
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
// Console Printf ... 
int __cdecl ConsolePrintf(const char* format, ... );
#define consoleprintf	ConsolePrintf
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

#define HAVE_WINSOCK
//#endif

#endif /* WIN32 */
/* *****  end     W I N 3 2   only   ***** */

//#include "match.h"
//#include "mfpre.h"
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
		 static NUTInfo *MF_NutInfo			= NULL;
		 static LONG allocTag;
	#endif // N_PLAT_NLM

	char	MF_ProductName[]	= "MailFilter/ax professional "MAILFILTERVERNUM" ["MAILFILTERPLATFORM"]";

	#ifdef __cplusplus
		#include "MFConfig.h++"
		MailFilter_Configuration::Configuration MF_GlobalConfiguration;
	#endif

#else // _MAILFILTER_MAIN_

	#define _MFGS(_mfgs_type,_mfgs_name,_mfgs_default)	extern _mfgs_type _mfgs_name

	extern char *MF_ProductName;
	
	// (NUT) String Table
	extern char *programMesgTable[];


	#ifdef __cplusplus
		#include "MFConfig.h++"
		extern MailFilter_Configuration::Configuration MF_GlobalConfiguration;
	#endif

#endif // _MAILFILTER_MAIN_

// Import Configuration Values
//#define	_MAILFILTER_MFCONFIG_DONTWANTFILTERCACHE
//#include "../Config/MFConfig.h"

// Temporary Configuration Storage
#ifdef N_PLAT_NLM
	// NLM Data ...
#ifndef __NOVELL_LIBC__
	_MFGS(	unsigned int,	MF_NLMHandle,						0		);
	_MFGS(	int,			mainThread_ThreadGroupID,			1		);
	_MFGS(	int,			MF_ScreenID,						0		);
	_MFGS(	int,			MFD_ScreenID,						0		);
#else
	_MFGS(	void*,			MF_NLMHandle,						0		);
	_MFGS(	void*,			MF_ScreenID,						NULL	);
	_MFGS(	void*,			MFD_ScreenID,						NULL	);
	_MFGS(	rtag_t,			MFD_ScreenTag,						NULL	);
#endif

	// DS Handles ...
#ifndef __NOVELL_LIBC__
	_MFGS(		NWDSContextHandle, MFT_NLM_DS_Context, 		NULL	);
	_MFGS(	LONG,			MFT_NLM_Connection_HandleCLIB,		NULL	);
	_MFGS(	NWCONN_HANDLE,	MFT_NLM_Connection_Handle,			NULL	);
#endif
#endif
	// Temporary Configuration Data	(MFT_)
//	_MFGS(	char*, 			MFT_Local_ServerName,				NULL	);
//	_MFGS(	char*,	 		MFT_MF_LogDir,						NULL	);
	_MFGS(	bool,			MFT_InOutSameServerVolume,			false	);
	_MFGS(	bool,			MFT_RemoteDirectories,				false	);
	_MFGS(	bool,			MFT_Verbose,						false	);
	_MFGS(	int,			MFT_Debug,							0		);
	_MFGS(	unsigned long,	MFT_I18N_MessageCount,				0		);
	_MFGS(	unsigned long,	MFT_I18N_LanguageID,				0		);
	_MFGS(	bool,			MFC_MAILSCAN_Enabled,				0		);

	_MFGS(	int,			MFD_AllocCounts,					0		);
	_MFGS(	int,			MFD_AllocCountsCPP,					0		);

	// Thread Handles
#ifdef __NOVELL_LIBC__
	_MFGS(	NXThreadId_t,	MF_Thread_Work,						0		);
	_MFGS(	NXThreadId_t,	MF_Thread_SMTP,						0		);
#else
	_MFGS(	HANDLE,			MF_Thread_Work,						0		);
	_MFGS(	HANDLE,			MF_Thread_SMTP,						0		);
#endif

	// When were we last alive ?
	_MFGS(	int,			MF_LastAlive,						1		);

	// Counters ...	(MFS_)
	_MFGS(	LONG,			MFS_MF_MailsInputTotal,				0		);
	_MFGS(	LONG,			MFS_MF_MailsInputFailed,			0		);
//	_MFGS(	LONG,			MFS_MF_MailsInputTotalSize,			0		);
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
#define _MF_NUTCHAR		(const char*)
#else
#define NXThreadYield	ThreadSwitch
#define consoleprintf	ConsolePrintf
#define mkdir(a,b)		mkdir(a)
#define CloseScreen		DestroyScreen
#define _MF_NUTCHAR		(unsigned char*)
#define ActivateScreen	DisplayScreen
#endif
#endif

#ifndef COLOR_NONE
#define COLOR_NONE            0
#define COLOR_GREY            8
#define COLOR_SILVER          7
#define COLOR_WHITE           15
#define COLOR_NAVY            1
#define COLOR_BLUE            (0x01|8)
#define COLOR_GREEN           2
#define COLOR_LIME            (0x02|8)
#define COLOR_TEAL            3
#define COLOR_CYAN            (0x03|8)
#define COLOR_MAROON          4
#define COLOR_RED             (0x04|8)
#define COLOR_PURPLE          5
#define COLOR_MAGENTA         (0x05|8)
#define COLOR_OLIVE           6
#define COLOR_YELLOW          (0x06|8) 
#endif

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
//#define MFD_Out 0
#undef MFT_Verbose
#undef MFT_Debug
#define MFT_Verbose	0
#define MFT_Debug	0

#define MFD_Out(...)

#else

void MFD_Out_func(int attr, const char* format, ...);
#define MFD_Out				MFD_Out_func

#endif

// MALLOC TRACEING MACROS
static void *_mfd_nt_malloc(size_t size, const char* szFuncName)
{	szFuncName=szFuncName; return malloc(size);	}
static char *_mfd_nt_strdup(const char * str, const char* szFuncName)
{	szFuncName=szFuncName; return strdup(str);	}
static void _mfd_nt_free(void *ptr, const char* szFuncName)
{	szFuncName=szFuncName; return free(ptr);	}

#ifdef _MF_MEMTRACE
static void _mfd_cpp_allocX( const char* szArea, size_t approxSize )
{	++MFD_AllocCountsCPP;	
//	consoleprintf("MAILFILTER: ++ALLC in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCountsCPP,szArea,approxSize);
}
static void _mfd_cpp_freeX( const char* szArea, size_t approxSize )
{	--MFD_AllocCountsCPP;	
//	consoleprintf("MAILFILTER: ++FREE in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCountsCPP,szArea,approxSize);
}

static void _mfd_free( void *ptr , const char* szFuncName )
{
	--MFD_AllocCounts;
#ifdef __NOVELL_LIBC__
	consoleprintf("MAILFILTER: FREE   in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCounts,szFuncName,msize(ptr));
#else
	consoleprintf("MAILFILTER: FREE   in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCounts,szFuncName,_msize(ptr));
#endif
	free(ptr);
}
static void *_mfd_malloc( size_t size , const char* szFuncName )
{
	++MFD_AllocCounts;
	consoleprintf("MAILFILTER: MALLOC in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCounts,szFuncName,size);
	return malloc(size);
}
static char *_mfd_strdup( const char * str , const char* szFuncName )
{
	++MFD_AllocCounts;
	ConsolePrintf("MAILFILTER: STRDUP in "_MFD_MODULE" ; count: %d ; %s\n",MFD_AllocCounts,szFuncName);
	return strdup(str);
}

#undef malloc
#undef free
#undef strdup
#define malloc(x)						_mfd_malloc(x,"")
#define free(x)							_mfd_free(x,"")
#define strdup(x)						_mfd_strdup(x,"")
#define _mfd_cpp_alloc					_mfd_cpp_allocX
#define _mfd_cpp_free					_mfd_cpp_freeX

#else
#define _mfd_malloc(x,y)				malloc(x)
#define _mfd_free(x,y)					free(x)
#define _mfd_strdup(x,y)				strdup(x)
#define _mfd_cpp_alloc					0
#define _mfd_cpp_free					0
#endif 

// Message Support Macro
#define MF_Msg(id)						programMesgTable[id]
#define MF_DisplayCriticalError(id)		consoleprintf(MF_Msg(id))
#define MF_StatusNothing(void)			MF_StatusText("")

// MFL Prototypes
int MFL_Init(int rLvl);
bool MFL_GetFlag(int flag);
extern int MFL_Certified;

// Macros

// Prototypes
void MF_ShutDown(void);
int MF_ConfigReadString(char ConfigFile[MAX_PATH], int Entry, char Value[]);
//bool MF_ConfigRead(void);
//void MF_ConfigFree(void);
void MF_ExitProc(void);
void MF_StatusText( const char *newText );
void MF_MakeValidPath(char* thePath);
void MF_GetServerName(char* serverName);
void MF_StatusUI_Update(const char* newText);
void MF_StatusUI_UpdateLog(const char* newText);
#ifdef __cplusplus
void MF_StatusUI_Update(int newText);
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
//bool MF_Filter_InitLists(void);
//void MF_Filter_FreeLists(void);
void MFL_VerInfo(void);
int MF_MailProblemReport(MailFilter_MailData* mMailInfo);
int MF_Notification_Send2(const char messageType, const char* bounceRcpt, MailFilter_MailData* mMailInfo);
void MF_OutOfMemoryHandler(void);
int MF_EMailPostmasterGeneric(const char* Subject, const char* Text, const char* szAttachFileName, const char* szAttachDisplayName);
void MF_UI_ShowConfiguration(void);
int MFAPI_FilterCheck( char *szScan , int mailSource , int matchfield );
bool MFBW_CheckCurrentScheduleState();
int MF_CountFilters(int action);

// Thread Functions
#ifndef WIN32
void MF_Work_Startup(void *dummy);
void MF_SMTP_Startup(void *dummy);
#else
DWORD WINAPI MF_Work_Startup(void *dummy);
DWORD WINAPI MF_SMTP_Startup(void *dummy);
#endif // WIN32


#define ERROR_SUCCESS	0

/*
 *
 *
 *  --- eof ---
 *
 *
 */

#endif	// _MAILFILTER_H_
