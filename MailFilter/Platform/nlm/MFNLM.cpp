/*
 +
 +		MFNLM.cpp
 +		
 +		NetWare Platform Specific Functions
 +		for CLib AND LibC
 +
 +		
 +		Copyright 2001-2004 Christian Hofstädtler.
 +
 +		
 */

#ifdef N_PLAT_NLM

// Using _MAIN_ to get variables defined here.
#define _MAILFILTER_MAIN_

#define _MFD_MODULE			"MFNLM.CPP"

// Include MailFilter.h
#include "..\..\Main\MailFilter.h"
#include "MFConfig-defines.h"

// And undef it ... ;)
#undef _MAILFILTER_MAIN_

#include "MFVersion.h"
#include "MFZip.h"
#include "MFRelayHost.h++"
#include "MFConfig.h++"
#include "MFAVA-NLM.h"

#include <sys/utsname.h>

#ifdef __NOVELL_LIBC__
	#include <nks/netware.h>
	#include <nks/dirio.h>
	#include <client.h>
#endif

extern int MailFilter_Main_RunAppConfig(bool bStandalone);
extern int MailFilter_Main_RunAppRestore(bool bStandalone);
extern int MailFilter_Main_RunAppNRM(bool bStandalone);


extern char MFL_LicenseKey[MAX_PATH];

extern void MailFilter_NRM_sigterm();

// NUT: Local Prototypes
static int MF_NutVerifyExit(void);

// debug key
static void MF_NutHandlerKeyF2 (void *handle);
// real keys
static void MF_NutHandlerKeyF6 (void *handle);
static void MF_NutHandlerKeyF7 (void *handle);
static void MF_NutHandlerKeyF8 (void *handle);
static void MF_NutHandlerKeyF9 (void *handle);
static void MF_NutHandlerKeyF10 (void *handle);

// NUT: Status Portal Handle
static PCB *statusPortalPCB = NULL;

static bool MFT_NUT_GetKey = true;

// Time ...
static tm tmp_TimeTM;
static time_t tmp_Time;

static int MF_NutMutexQueryUser = 0;


extern void MFWorker_SetupPaths();


//
//
//

#ifndef __NOVELL_LIBC__
extern "C" { 
	extern int NWIsNLMLoadedProtected(void);
	extern int nlmisloadedprotected (void);
}

inline bool mf_nlmisloadedprotected()
{
	return (bool)nlmisloadedprotected();
}
#else
inline bool mf_nlmisloadedprotected()
{
	return (bool)nlmisloadedprotected();
}
#endif


//
//  Event Handler For NLM ShutDown
//
void MF_ExitProc(void)
{
	// Shut down NUT	
	if (MF_NutInfo != NULL)
	{
		NWSRestoreNut ( MF_NutInfo );
		MF_NutInfo = NULL;
		
		statusPortalPCB = NULL;
	}
	
	// Destroy Screen ...
	if (MFD_ScreenID)
#ifdef __NOVELL_LIBC__
		CloseScreen ( MFD_ScreenID );
#else
		DestroyScreen ( MFD_ScreenID );
#endif
}

//	"   F6-Restart  F7-Exit                   F9-Show Configuration  F10 Configure  ",


void MF_NutHandlerKeyF6 (void *handle)		// RESTART
{
	int rc = 0;
	handle = handle;			// Keep compiler quiet.

	MFT_NLM_Exiting = 254;
	
	MF_StatusText("Preparing to restart MailFilter...");

	// We need this to wake the UI thread up ...
	NWSUngetKey ( K_ESCAPE , 0 , MF_NutInfo );

	return;
}
void MF_NutHandlerKeyF7 (void *handle)		// EXIT
{
	if (MF_NutVerifyExit())
	{
		MF_StatusText("Shutting Down On Keyboard Request ...");
		MF_DisplayCriticalError("MAILFILTER: Shutting Down On Keyboard Request ...\n");

		// Disable Func. Keys so user can't exit a 2nd time
		NWSDisableAllInterruptKeys((NUTInfo*)handle);

#ifdef __NOVELL_LIBC__
		raise(SIGTERM);
#else
		system("MFSTOP");
#endif
	}
}

void MF_NutHandlerKeyF8 (void *handle)		// RESTORE ME
{
#pragma unused(handle)

	MFT_NLM_Exiting = 252;
	
	// We need this to wake the UI thread up ...
	NWSUngetKey ( K_ESCAPE , 0 , MF_NutInfo );

	return;
}

void MF_NutHandlerKeyF10 (void *handle)		// CONFIGURE ME
{
#pragma unused(handle)

	MFT_NLM_Exiting = 253;
	
	// We need this to wake the UI thread up ...
	NWSUngetKey ( K_ESCAPE , 0 , MF_NutInfo );

	return;
}
void MF_NutHandlerKeyF9 (void *handle)		// SHOW CONFIG
{
#pragma unused(handle)
	MF_UI_ShowConfiguration();
}

void MF_NutHandlerKeyF2 (void *handle)		// debug stuff
{
	handle = handle;			// Keep compiler quiet.

	char buf[81];

#ifdef _MF_MEMTRACE
	sprintf(buf,"AllocCount: %d",MFD_AllocCounts);
	MF_StatusUI_UpdateLog(buf);
#endif

	sprintf(buf,"StackAvail: %d, DebugLevel: %d",stackavail(),MFT_Debug);
	MF_StatusUI_UpdateLog(buf);

	MFUtil_CheckCurrentVersion();

	MF_LoginUser();
}

void MFL_VerInfo()
{
	MF_StatusText(MF_Msg(MSG_EVAL_LINE1));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE2));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE3));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE4));
	MF_StatusNothing();
}

bool _mf_nutinit2(rtag_t tagID)
{
	long	ccode;
	ccode=NWSInitializeNut(
		/*	utility				*/	MSG_PROGRAM_NAME,
		/*	version				*/	(unsigned long)-1,
		/*	headerType			*/	NO_HEADER,	//SMALL_HEADER,
		/*	compatibilityType	*/	NUT_REVISION_LEVEL,
#ifdef __NOVELL_LIBC__
		/*	messageTable		*/	(char **)programMesgTable,
#else
		/*	messageTable		*/	(unsigned char **)programMesgTable,
#endif
		/*	helpScreens			*/	NULL,
		/*	screenID			*/	MF_ScreenID,
		/*	resourceTag			*/	tagID,
		/*	handle				*/	&MF_NutInfo
		);
	if(ccode != 0)
		return false;
	return true;
}

//
//  NUT: Initialize NUT and Tags
//
bool MF_NutInit(void)
{
	rtag_t	tagID;
	char szTemp[80+2];


	#ifdef __NOVELL_LIBC__
	tagID=AllocateResourceTag(
		/*	NLMHandle			*/	MF_NLMHandle,
		/*	descriptionString	*/	MF_Msg(MSG_PROGRAM_TAG_NAME),
		/*	resourceType		*/	AllocSignature
		);
	#else
	tagID=AllocateResourceTag(
		/*	NLMHandle			*/	MF_NLMHandle,
		/*	descriptionString	*/	 (const unsigned char*)MF_Msg(MSG_PROGRAM_TAG_NAME),
		/*	resourceType		*/	AllocSignature
		);
	#endif
	if(tagID == NULL) 
	{
		MF_DisplayCriticalError("MAILFILTER: Error allocating Resource Tag for NUT!\n");
		return false;	
	}

	if (!_mf_nutinit2(tagID))
	{
	#ifdef __NOVELL_LIBC__
		// load threads.nlm for nwsnut
		MF_DisplayCriticalError("MAILFILTER: Loading THREADS.NLM for NWSNUT.\n");
		LoadModule(0, "THREADS.NLM", 0);

		if (!_mf_nutinit2(tagID))
	#endif
		{
			MF_DisplayCriticalError("MAILFILTER: Error initializing NWSNUT!\n");
			return false;	
		}
	}

	memset(szTemp,' ',81);
	sprintf(szTemp,"  MailFilter Server");

#ifdef MAILFILTER_VERSION_BETA
	char* szBeta = "** BETA **";
	memcpy(szTemp+35,szBeta,strlen(szBeta));
#endif
#ifdef _TRACE
	char* szTrace = "** DEBUG **";
	memcpy(szTemp+35,szTrace,strlen(szTrace));
#endif

	memcpy(szTemp+62,"Version",7);
	memcpy(szTemp+70,MAILFILTERVERNUM,strlen(MAILFILTERVERNUM));

	NWSShowLineAttribute ( 0 , 0 , (_MF_NUTCHAR)szTemp , VINTENSE , 80 , (struct ScreenStruct*)MF_NutInfo->screenID );


	return true;
}

bool MF_NutDeinit(void)
{
	if (MF_NutInfo != NULL)
		NWSRestoreNut(MF_NutInfo);
	MF_NutInfo = NULL;
	return true;
}

static bool MailFilterApp_Server_InitNut()
{
	if (!MF_NutInit())
		return false;

	LONG	statusPortal;

	statusPortal = NWSCreatePortal(
						4,
						0,
						20,
						80,
						20,
						80,
						SAVE,
						NULL, //(unsigned char *)"Application Messages",
						VNORMAL,
						SINGLE,
						VNORMAL,
						CURSOR_OFF,
						VIRTUAL,
						MF_NutInfo);

	NWSGetPCB(&statusPortalPCB, statusPortal, MF_NutInfo);
	NWSSelectPortal(statusPortal, MF_NutInfo);
	NWSClearPortal(statusPortalPCB);
	NWSDeselectPortal(MF_NutInfo);

	NWSEnableFunctionKey (	K_F6 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F7 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F8 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F9 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F10 , MF_NutInfo );

	NWSEnableInterruptKey (	K_F6 , *MF_NutHandlerKeyF6, MF_NutInfo );
	NWSEnableInterruptKey (	K_F7 , *MF_NutHandlerKeyF7, MF_NutInfo );
	NWSEnableInterruptKey ( K_F8 , *MF_NutHandlerKeyF8, MF_NutInfo );
	NWSEnableInterruptKey (	K_F9 , *MF_NutHandlerKeyF9, MF_NutInfo );
	NWSEnableInterruptKey (	K_F10 , *MF_NutHandlerKeyF10, MF_NutInfo );

	// debug key
	NWSEnableFunctionKey (  K_F2 , MF_NutInfo );
	NWSEnableInterruptKey (	K_F2 , *MF_NutHandlerKeyF2, MF_NutInfo );

	return true;
}

//
//  NUT: Prompt User To Unload NLM
//
static int MF_NutVerifyExit(void)
{
	signed int cCode = 0;
	BYTE kKey;
	LONG kType;

	if (MF_NutInfo == NULL)		return 0;
	
	MFT_NUT_GetKey = false;
	MF_NutMutexQueryUser = 1;
	
	char szTemp[82];

	for (int i=0;i<81;i++) szTemp[i]=' ';
	szTemp[80]=0;
	strncpy(szTemp,MF_Msg(MENU_MAIN_EXIT),75);
	NWSShowLineAttribute ( 24, 0, (_MF_NUTCHAR) szTemp, VREVERSE /*VINTENSE*/, 80, (struct ScreenStruct*)MF_NutInfo->screenID);

	while (cCode == 0)
	{

		NWSGetKey ( &kType, &kKey, MF_NutInfo);

		if (kType == K_ESCAPE)
			cCode = -2;
		if (kKey == K_SELECT)
			cCode = -1;
		if (kType == K_ESCAPE)
			cCode = -2;
		if (kKey == K_SELECT)
			cCode = -1;
		if (kKey == 121)		// y
			cCode = -1;
		if (kKey == 110)		// n
			cCode = -2;
		if (kKey == 89)			// Y
			cCode = -1;
		if (kKey == 78)			// N
			cCode = -2;
	}
	
	MF_NutMutexQueryUser = 0;
	MFT_NUT_GetKey = true;
	
	MF_StatusUI_Update(NULL);

	if (cCode == -2)
		return 0;		//NO
	if (cCode == -1)
		return 1;		//YES
		
	return 0;
}

inline void MF_StatusUI_UpdateLog(int newText)
{
	MF_StatusUI_UpdateLog(MF_Msg(newText));
}

void MF_StatusUI_UpdateLog(const char* newText)
{
	char 	szTemp[82];

	// Update Status (History) Window

	for (int i=0;i<81;i++) szTemp[i]=' ';
	szTemp[80]=0;

	time(&tmp_Time);
	tmp_TimeTM = *localtime (&tmp_Time);
	
	sprintf(szTemp,"%02d:%02d:%02d ",tmp_TimeTM.tm_hour,tmp_TimeTM.tm_min,tmp_TimeTM.tm_sec);
	strncat(szTemp , newText , 69 );
	szTemp[80]=0;

	NWSScrollPortalZone (
			0L,			//LONG Line
			0L,			//LONG Columns
			22L,		//Height
			78,			//Width
			VNORMAL,	//LONG   attribute, 
			1L,			//LONG   count, 
			V_UP,		//LONG   direction,
			statusPortalPCB);

	NWSShowPortalLine(17, 0, (_MF_NUTCHAR) szTemp, strlen(szTemp), statusPortalPCB);
	
	NWSUpdatePortal(statusPortalPCB);
}

void MF_StatusUI_Update(int newText)
{
	MF_StatusUI_Update(MF_Msg(newText));
}

					   //  0   1    2   3   
static char cStatus[] = { '|','/','-','\\' };
extern unsigned int MFT_SleepTimer;

//
//  NUT: Update Status Line
//
void MF_StatusUI_Update(const char* newText)
{
#pragma unused(newText)

	char 	szTemp[90];
	int		i;

	if (!MF_NutMutexQueryUser)
	{

		// Update Function-Key-Text Line ...
		for (i=0;i<81;i++)
		{
			szTemp[i]=' ';
		}
		szTemp[80]=0;
		
		if (MFT_NLM_Exiting == 0)
		{
			strncpy(szTemp,MF_Msg(STATUS_STATISTICS),79);
			NWSShowLineAttribute ( 24, 0, (_MF_NUTCHAR) szTemp, VREVERSE /*VINTENSE*/, 80, (struct ScreenStruct*)MF_NutInfo->screenID);
		}

	}

	// Update Statistics in top of NUT screen
	if (!MFT_NLM_Exiting)
	{
		static unsigned long lastClck;
		static int iStatusChar;
		std::string szDynamic;
		unsigned int chk;
		bool bHaveFeatures = false;
	
	
#ifdef __NOVELL_LIBC__
		if ( MF_GlobalConfiguration.RequireAVA && (MailFilter_AV_Check() != 0) && MFT_bTriedAVInit)
		{
//
//			for (i=0;i<81;i++)
//				szTemp[i]=' ';
//			szTemp[80]=0;
			
//			NWSShowLineAttribute( 1, 0, (_MF_NUTCHAR) szTemp, VBLINK, strlen(szTemp), (struct ScreenStruct*)MF_NutInfo->screenID);
//			NWSShowLineAttribute( 2, 0, (_MF_NUTCHAR) szTemp, VBLINK, strlen(szTemp), (struct ScreenStruct*)MF_NutInfo->screenID);
//			NWSShowLineAttribute( 3, 0, (_MF_NUTCHAR) szTemp, VBLINK, strlen(szTemp), (struct ScreenStruct*)MF_NutInfo->screenID);

			strcpy(szTemp,"NO AntiVirus NLM DETECTED - NO E-MAIL TRANSPORT");
			
			FillScreenArea(getscreenhandle(),1,0,3,80,' ',COLOR_ATTR_YELLOW);
			DisplayScreenTextWithAttribute(getscreenhandle(),2,((80-strlen(szTemp))/2)-1,strlen(szTemp),COLOR_ATTR_YELLOW,szTemp);

		} else {
#endif
		
			unsigned long clck = clock() / CLOCKS_PER_SEC;
			
			if (lastClck != clck)
			{
				lastClck = clck;
				++iStatusChar;
				if (iStatusChar > 3)
					iStatusChar = 0;
			}
			
			clck = clck / 60;
			
			unsigned long days = clck / 24 / 60;
			unsigned long hours = (clck - (days * 24 * 60)) / 60;
			unsigned long minutes = clck - (days * 24 * 60) - (hours * 60);
			
			sprintf(szTemp,MF_Msg(MSG_INFOPORTAL_LINE1), days,hours,minutes);
			chk = MF_GlobalConfiguration.DomainHostname.length(); if (chk>55) chk=55;
			memcpy(szTemp+2,MF_GlobalConfiguration.DomainHostname.c_str(),chk);
			NWSShowLineAttribute( 1, 0, (_MF_NUTCHAR) szTemp, VNORMAL, strlen(szTemp), (struct ScreenStruct*)MF_NutInfo->screenID);



			sprintf(szTemp,MF_Msg(MSG_INFOPORTAL_LINE2),MFS_MF_MailsInputTotal,MFS_MF_MailsInputFailed);

			if ( MF_GlobalConfiguration.RequireAVA && !MFT_bTriedAVInit && MFL_GetFlag(MAILFILTER_MC_M_VIRUSSCAN) )
			{
				szDynamic = "Waiting for AntiVirus NLM";
			} else {
				szDynamic = "Running ";
				szDynamic += cStatus[iStatusChar];
			}
			memcpy(szTemp+2,szDynamic.c_str(),szDynamic.length());
			NWSShowLineAttribute( 2, 0, (_MF_NUTCHAR) szTemp, VNORMAL, strlen(szTemp), (struct ScreenStruct*)MF_NutInfo->screenID);



			sprintf(szTemp,MF_Msg(MSG_INFOPORTAL_LINE3),MFS_MF_MailsOutputTotal,MFS_MF_MailsOutputFailed);
			
			
			szDynamic = "Modules: ";
	#ifdef MF_WITH_ZIP
			bHaveFeatures = true;
			szDynamic += "ZIP ";
	#endif

			if (MFC_MAILSCAN_Enabled)
			{
				szDynamic += "Decode ";
				bHaveFeatures = true;
			}
			if (MFBW_CheckCurrentScheduleState() == false)
			{
				szDynamic += "Schedule ";
				bHaveFeatures = true;
			}
			if (MailFilter_AV_Check() == 0)
			{
				szDynamic += "AV ";
				bHaveFeatures = true;
			}
			if (MF_GlobalConfiguration.NRMInitialized == true)
			{
				szDynamic += "NRM ";
				bHaveFeatures = true;
			}
			
			if (!bHaveFeatures)
				szDynamic += "None";
				
			memcpy(szTemp+2,szDynamic.c_str(),szDynamic.length());
			NWSShowLineAttribute( 3, 0, (_MF_NUTCHAR) szTemp, VNORMAL, strlen(szTemp), (struct ScreenStruct*)MF_NutInfo->screenID);

#ifdef __NOVELL_LIBC__
		}
#endif
	}
	
	return;
}

#ifndef _MF_CLEANBUILD
void MFD_UseMainScreen()
{
	if (MFD_ScreenID)
#ifdef __NOVELL_LIBC__	
		CloseScreen ( MFD_ScreenID );
#else
		DestroyScreen ( MFD_ScreenID );
#endif
	MFD_ScreenID = NULL;
}

#ifndef COLOR_ATTR_NONE
extern "C" {
	int      vsnprintf( char * restrict, size_t n, const char * restrict, va_list );
}

// borrowed from LibC's screen.h
/* attributes for OutputToScreenWithAttributes(); cf. HTML color names */
#define COLOR_ATTR_NONE       0           /* (black, no color at all)        */
#define COLOR_ATTR_NAVY       1           /* (dim blue)                      */
#define COLOR_ATTR_BLUE       (0x01|8)
#define COLOR_ATTR_GREEN      2
#define COLOR_ATTR_LIME       (0x02|8)    /* (bright green)                  */
#define COLOR_ATTR_TEAL       3           /* (dim cyan)                      */
#define COLOR_ATTR_CYAN       (0x03|8)
#define COLOR_ATTR_MAROON     4           /* (dim red)                       */
#define COLOR_ATTR_RED        (0x04|8)
#define COLOR_ATTR_PURPLE     5
#define COLOR_ATTR_MAGENTA    (0x05|8)    /* (bright purple)                 */
#define COLOR_ATTR_OLIVE      6           /* (brown, dim yellow)             */
#define COLOR_ATTR_YELLOW     (0x06|8)
#define COLOR_ATTR_SILVER     7           /* normal white, dim/unhighlighted */
#define COLOR_ATTR_GREY       8           /* (dimmed white)                  */
#define COLOR_ATTR_WHITE      15          /* bright, highlighted white       */
#endif

void MFD_Out_func(int source, const char* fmt, ...) {

	va_list	argList;
	unsigned char attr = 0;

	if (!chkFlag(MFT_Debug,source))
		return;

	switch (source)
	{
		case MFD_SOURCE_GENERIC:	{	attr = COLOR_ATTR_SILVER;	break;	}
		case MFD_SOURCE_WORKER:		{	attr = COLOR_ATTR_CYAN;		break;	}
		case MFD_SOURCE_CONFIG:		{	attr = COLOR_ATTR_GREEN;	break;	}
		case MFD_SOURCE_VSCAN:		{	attr = COLOR_ATTR_LIME;		break;  }
		case MFD_SOURCE_ZIP:		{	attr = COLOR_ATTR_TEAL;		break;  }
		case MFD_SOURCE_ERROR:		{	attr = COLOR_ATTR_RED;		break;  }
		case MFD_SOURCE_RULE:		{	attr = COLOR_ATTR_YELLOW;	break;	}
		case MFD_SOURCE_MAIL:		{	attr = COLOR_ATTR_PURPLE;	break;	}
		case MFD_SOURCE_SMTP:		{	attr = COLOR_ATTR_WHITE;	break;	}
		default:					{	attr = COLOR_ATTR_GREY;		break;  }
	}

	va_start(argList, fmt);

#ifndef __NOVELL_LIBC__

	int oldScreen = 0;
	if (MFD_ScreenID)	oldScreen=SetCurrentScreen((int)MFD_ScreenID);   
	
	vprintf(fmt, argList);
	if (oldScreen)		SetCurrentScreen(oldScreen);

#else 
	char buffer[5000];
	vsnprintf(buffer,4999,fmt,argList);
	buffer[4999] = 0;



#ifdef _TRACE

	MF_DisplayCriticalError("%s",buffer);
	
#else

	// MF_UseMainScreen() support
	if (MFD_ScreenID)
		OutputToScreenWithAttribute(MFD_ScreenID,attr,buffer);
		else
		OutputToScreenWithAttribute(MF_ScreenID,attr,buffer);

#endif

#endif

	va_end (argList);
}
#endif	//!_MF_CLEANBUILD


//
//  Event Handler For Server ShutDown
//
//   * Tell NLM to unload. Nothing special ...
//
void eventShutDownReport(LONG par1,LONG par2)
{

	MF_StatusText(MF_Msg(MSG_CLOSEFILES));

	MFT_NLM_Exiting = 255;

	MF_StatusFree();


	par1 = par1;	par2 = par2;		// Get rid of compiler warnings ;)
}

//
//  Event Handler For Server ShutDown
//
//   * Tell NLM to unload. Nothing special ...
//
LONG eventShutDownWarn(void (*OutPutFunc)(const void *fmt,...),LONG par1,LONG par2)
{
#pragma unused (OutPutFunc)

//	Warning: DO NOT do anything to the parameters passed, DO NOT call OutPutFunc ...
//	OutPutFunc("MAILFILTER: Will Terminate Worker Thread on Server Shut Down.\n");
	par1=par1; par2=par2;

	return 0;	// Simply Allow Shutdown ...
}

void NLM_SignalHandler(int sig)
{
#ifndef __NOVELL_LIBC__
	int handlerThreadGroupID;
#endif

	switch(sig)
	{
		case SIGTERM:
			{
				MFT_NLM_Exiting = 255;
				
#ifndef __NOVELL_LIBC__
				if (MF_Thread_Work > 0)
					ResumeThread(MF_Thread_Work);

				if (MF_Thread_SMTP > 0)
					ResumeThread(MF_Thread_SMTP);

				handlerThreadGroupID = GetThreadGroupID();
				SetThreadGroupID(mainThread_ThreadGroupID);
#endif
				// NLM SDK functions may be called here

				// We need this to wake the UI thread up ...
#ifndef __NOVELL_LIBC__
				if (MF_NutInfo != NULL)
				{
					NWSUngetKey ( K_ESCAPE , 0 , MF_NutInfo );
				}

				SetThreadGroupID(handlerThreadGroupID);
#else
				ungetcharacter(27);

				if ( (MF_GlobalConfiguration.ApplicationMode == MailFilter_Configuration::NRM) || (MF_GlobalConfiguration.NRMInitialized == true) )
				{
					MailFilter_NRM_sigterm();
				}
#endif

				while (MFT_NLM_ThreadCount > 0)
				{
					// wait for MailFilter threads to terminate
					NXThreadYield();
				}

			}
			break;
		case SIGINT:
			{	// ignore CTRL-C
				signal(SIGINT, NLM_SignalHandler);
				break;
			}
	}
	return;
}

#ifdef _MF_MEMTRACE
static void _mfd_tellallocccountonexit()
{
	MF_DisplayCriticalError("MAILFILTER: AllocCount: %d\n",MFD_AllocCounts);
}
#endif //_MF_MEMTRACE


static void MF_Cleanup_Startup(void *dummy)
{
#pragma unused(dummy)

	// Rename this Thread
#if defined( N_PLAT_NLM ) && (defined(__NOVELL_LIBC__))
	NXContextSetName(NXContextGet(),"MailFilterCleaner");
#endif

	MFT_NLM_ThreadCount++;
	unsigned long counter = 0;
	
	MF_CheckProblemDirAgeSize();

	while (MFT_NLM_Exiting == 0)
	{
		++counter;
		
		if (counter > (2*3600))	// wait two hours between checks.
		{
			counter = 0;
			
			MF_CheckProblemDirAgeSize();
		}
		
		delay(1000);
		NXThreadYield();

	}

	MFT_NLM_ThreadCount--;
}


static void LoadNRMThreadStartup(void *dummy)
{
#pragma unused(dummy)

	MF_GlobalConfiguration.NRMInitialized = true;
	MailFilter_Main_RunAppNRM(false);
}

//
//
//  NLM Application Startup Function:
//   SERVER
//
//   * Start Thread(s) ...
//   * Run NUT in foreground thread.
//
static int MailFilter_Main_RunAppServer(const char* szProgramName)
{	
	int rc = 0;

	// Init NWSNut
	printf(MF_Msg(MSG_BOOT_USERINTERFACE));

	if (!MailFilterApp_Server_InitNut())
	{
		MF_DisplayCriticalError("MAILFILTER: Error communicating with NWSNUT. Terminating!\n");
		goto MF_MAIN_TERMINATE;
	}
	
	if (!MailFilterApp_Server_LoginToServer())
	{
		MF_DisplayCriticalError("MAILFILTER: Error logging into the Server. Terminating!\n");
		goto MF_MAIN_TERMINATE;
	}
	
	if (!MailFilterApp_Server_SelectUserConnection())
	{
		MF_DisplayCriticalError("MAILFILTER: Error selecting server connection. Terminating!\n");
		goto MF_MAIN_TERMINATE;
	}
	
MF_MAIN_RUNLOOP:
	{
		// needed for mf restart
		MFT_NLM_Exiting = 0;
		MFT_bStartupComplete = false;

		MFD_Out(MFD_SOURCE_GENERIC,"Initializing Logging...\n");

		// Init Status
		if (!MF_StatusInit())
		{
			MF_DisplayCriticalError("MAILFILTER: Error initializing Logging. Terminating!\n");
			goto MF_MAIN_TERMINATE;
		}

		// get OS info
		{
			char szStatusMsg[82];
		#ifndef __NOVELL_LIBC__
			struct utsname un;
			
			MF_StatusLog("MailFilter CLIB Version "MAILFILTERVERNUM);

			uname(&un);
			
			sprintf(szStatusMsg,"Operating System: Novell NetWare %s.%s",un.release,un.version);

			MF_StatusText(szStatusMsg);
			
			MF_StatusText("MFLT50: Limited Functionality MailFilter.");
			MF_StatusText("Please upgrade to a modern NetWare OS to get full functionality!");

		#else
			MF_StatusLog("MailFilter LIBC Version "MAILFILTERVERNUM);

			struct utsname u;
			uname(&u);

			sprintf(szStatusMsg,"Operating System: Novell NetWare %d.%d SP %d",
					u.netware_major,
					u.netware_minor,
					u.servicepack);
			MF_StatusText(szStatusMsg);

		#endif
	    }

		MF_StatusUI_Update(MSG_BOOT_LOADING);
		
		MFWorker_SetupPaths();
		
		strncpy(MFL_LicenseKey,MF_GlobalConfiguration.LicenseKey.c_str(),MF_GlobalConfiguration.LicenseKey.size() > MAX_PATH ? MAX_PATH : MF_GlobalConfiguration.LicenseKey.size() );
	    
		// Start Thread: ** WORK **
	#ifdef __NOVELL_LIBC__
		NXContext_t ctxWorker;

		if (NXThreadCreateSx( MF_Work_Startup , 
			NULL, NX_THR_JOINABLE|NX_THR_BIND_CONTEXT ,
			&ctxWorker , 
			&MF_Thread_Work
			))
	#else
		MF_Thread_Work = BeginThread(MF_Work_Startup,NULL,2*65536,NULL);						// Set 64k Stack for new thread
		if( MF_Thread_Work == EFAILURE )
	#endif
		{
			MF_DisplayCriticalError(MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
			MF_Thread_Work = 0;
			goto MF_MAIN_TERMINATE;
		}

		// Start Thread: ** Cleanup **
	#ifdef __NOVELL_LIBC__
		NXContext_t ctxCleanup;

		if (NXThreadCreateSx( MF_Cleanup_Startup , 
			NULL, NX_THR_JOINABLE|NX_THR_BIND_CONTEXT ,
			&ctxCleanup , 
			&MF_Thread_Cleanup
			))
	#else
		MF_Thread_Cleanup = BeginThread(MF_Cleanup_Startup,NULL,2*65536,NULL);						// Set 64k Stack for new thread
		if( MF_Thread_Cleanup == EFAILURE )
	#endif
		{
			MF_DisplayCriticalError(MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
			MF_Thread_Cleanup = 0;
			goto MF_MAIN_TERMINATE;
		}

		
		if (MF_GlobalConfiguration.EnableNRMThread == true)
		{
		#ifdef __NOVELL_LIBC__
			if (!mf_nlmisloadedprotected())
			{
				MFD_Out(MFD_SOURCE_CONFIG,"NRM thread will be started.\n");
			
				// Start Thread: ** NRM **
				#ifdef __NOVELL_LIBC__
				NXContext_t ctxNrm;

				if (NXThreadCreateSx( LoadNRMThreadStartup , 
					NULL, NX_THR_JOINABLE|NX_THR_BIND_CONTEXT,
					&ctxNrm, 
					&MF_Thread_NRM
					))
				#else
				MF_Thread_NRM = BeginThread(LoadNRMThreadStartup,NULL,2*65536,NULL);						// Set 64k Stack for new thread
				if( MF_Thread_NRM == EFAILURE )
				#endif
				{
					//MF_DisplayCriticalError(MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
					MF_StatusText("ERROR: NRM Startup Error. NRM NOT loaded.");
					MF_Thread_NRM = 0;
		//			goto MF_MAIN_TERMINATE;
				}
			} else {
				MF_StatusText("Loading NRM out-of-process.");
				std::string loadCmd = "LOAD ";
				loadCmd += szProgramName;
				loadCmd += " -t nrm ";
				loadCmd += MF_GlobalConfiguration.config_directory;
				system(loadCmd.c_str());
			}
		#else
			MF_StatusText("NOTE: NRM is not available for MFLT50.");
		#endif
		}

		
			/*
	#ifndef _MF_CLEANBUILD
		MFD_Out("Starting SMTP Thread...\n");

		// SMTP is enabled only in debug mode as this is not yet released.
		if (MFT_Debug)
		{
			// Start Thread: ** SMTP **
			MF_Thread_SMTP = BeginThread(MF_SMTP_Startup,NULL,65536,NULL);			// Set 64k Stack for new thread
			if( MF_Thread_SMTP == EFAILURE )
			{
				MF_DisplayCriticalError(MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
				MF_Thread_SMTP = 0;
				goto MF_MAIN_TERMINATE;
			}
		}
	#endif
		*/
		
		// Allow a Thread Switch to occour - start the worker!
		ThreadSwitch();

		// Lie that we've already exited ...
		--MFT_NLM_ThreadCount;

		MF_StatusUI_Update(MSG_BOOT_COMPLETE);
		MFD_Out(MFD_SOURCE_GENERIC,"Startup Complete.\n");

		// 
		LONG tmp_Key_Type;
		BYTE tmp_Key_Value;

		// Don't exit thread until all other threads are terminated ...
		while ( (MFT_NLM_ThreadCount > 0) || (!MFT_bStartupComplete) )
		{
			if (MFT_NLM_Exiting > 0)
				break;

			ThreadSwitch();

			if (MFT_NUT_GetKey)
			{
				NWSGetKey (	&tmp_Key_Type, &tmp_Key_Value,
								MF_NutInfo );
			}
		}
		
		// 254 = restart
		// 253 = config
		// 252 = restore
		if ((MFT_NLM_Exiting == 254) || (MFT_NLM_Exiting == 253) || (MFT_NLM_Exiting == 252))
		{
			bool bDoConfig = false;
			if (MFT_NLM_Exiting == 253)		bDoConfig = true;

			bool bDoRestore = false;
			if (MFT_NLM_Exiting == 252)		bDoRestore = true;
		
			MF_StatusText("  Please wait ... (may take a few minutes)");
			NXThreadYield();

#ifndef __NOVELL_LIBC__
			if (MF_Thread_Work > 0)
				ResumeThread(MF_Thread_Work);

			if (MF_Thread_SMTP > 0)
				ResumeThread(MF_Thread_SMTP);
#else
			if (MF_Thread_Work > 0)
				NXThreadContinue (MF_Thread_Work);

			if (MF_Thread_SMTP > 0)
				NXThreadContinue (MF_Thread_SMTP);

			if ( MF_GlobalConfiguration.NRMInitialized == true )
			{
				MFD_Out(MFD_SOURCE_CONFIG,"shutting down NRM thread\n");
				MailFilter_NRM_sigterm();
			} else {
				if (mf_nlmisloadedprotected())
					system("UNLOAD ADDRESS SPACE = OS MAILFLT.NLM");
			}
#endif

			MFT_NLM_Exiting = 255;

			// wait for other threads to exit
			// we lied above, anyway.
			while (MFT_NLM_ThreadCount > 0)
				NXThreadYield();

			MFT_NLM_Exiting = 0;
			
			MF_StatusFree();


			MF_NutDeinit();
			

			// ok... here we go:
			
			// * reinit config
			// Read Configuration from File
			if (!MF_GlobalConfiguration.ReadFromFile(""))
			{
				MF_DisplayCriticalError("MAILFILTER: Restart failed\n");
				MF_DisplayCriticalError("\tConfiguration Module reported an unrecoverable error.\n");
				goto MF_MAIN_TERMINATE;
			}
			
			if (bDoConfig == true)
			{
				// reconfigure mf
				MailFilter_Main_RunAppConfig(false);

				// * reinit config
				// Read Configuration from File
				if (!MF_GlobalConfiguration.ReadFromFile(""))
				{
					MF_DisplayCriticalError("MAILFILTER: Restart failed\n");
					MF_DisplayCriticalError("\tConfiguration Module reported an unrecoverable error.\n");
					goto MF_MAIN_TERMINATE;
				}
			}
			
			if (bDoRestore == true)
			{
				// open restore list
				MailFilter_Main_RunAppRestore(false);
			}
							
			if (!MailFilterApp_Server_InitNut())
			{
				MF_DisplayCriticalError("MAILFILTER: Restart failed\n");
				MF_DisplayCriticalError("\tUI Module reported an unrecoverable error.\n");
				goto MF_MAIN_TERMINATE;
			}

			// increase thread count so we can decrease it above again.
			++MFT_NLM_ThreadCount;
			goto MF_MAIN_RUNLOOP;
		}
	}

	rc = 666;

MF_MAIN_TERMINATE:
	MailFilterApp_Server_LogoutFromServer();
	return rc;
}


//
//  Main NLM Function
//
//   * Startup
//   * Read Configuration
//   * Run the selected MF app
//
int main( int argc, char *argv[ ])
{

	++MFT_NLM_ThreadCount;
	MF_NutInfo = NULL;
	MF_ScreenID = NULL;

#ifdef _MF_MEMTRACE
	atexit(_mfd_tellallocccountonexit);
#endif //_MF_MEMTRACE

	MF_ProductName = "MailFilter professional "MAILFILTERVERNUM" ["MAILFILTERPLATFORM"]";
	printf("%s\n",MF_ProductName);
	
#ifndef __NOVELL_LIBC__
	// Save Thread Group
	mainThread_ThreadGroupID = GetThreadGroupID();

	// Rename UI Thread
	RenameThread( GetThreadID() , MF_Msg(MSG_THREADNAME_MAIN) );
	
	MF_DisplayCriticalError("MAILFILTER: Info: This is the Legacy version of MailFilter!\n\tUse only on NetWare 5.0, NetWare 5.1 prior SP6 or NetWare 6.0 prior SP3.\n");
#else
	NXContextSetName(NXContextGet(),"MailFilterUI");

	{
		struct utsname u;
		uname(&u);
	
		if ( 
			(u.netware_major == 6) &&
			(u.netware_minor == 0) &&
			(u.servicepack < 2)
			)
			{
				MF_DisplayCriticalError("MAILFILTER: Detected NetWare 6.0 prior to SP3.\n\tPlease upgrade to a newer version or use the legacy MFLT50.NLM\n");
				return 0;
			}
		if ( 
			(u.netware_major == 5) &&
			(u.netware_minor == 1) &&
			(u.servicepack < 6)
			)
			{
				MF_DisplayCriticalError("MAILFILTER: Detected NetWare 5.1 prior to SP6.\n\tPlease upgrade to a newer version or use the legacy MFLT50.NLM\n");
				return 0;
			}
		if ( 
			(u.netware_major == 5) &&
			(u.netware_minor == 0)
			)
			{
				MF_DisplayCriticalError("MAILFILTER: Detected NetWare 5.0.\n\tPlease upgrade to a newer version or use the legacy MFLT50.NLM\n");
				return 0;
			}
	}
#endif

extern int MF_ParseCommandLine( int argc, char **argv );

	// Parse Command Line and build some vars...
	if (!MF_ParseCommandLine(argc,argv))
		goto MF_MAIN_TERMINATE;

	// Get NLM and Screen Handles
#ifdef __NOVELL_LIBC__
	MFD_ScreenTag = NULL;
	MF_NLMHandle = getnlmhandle();
	MF_ScreenID = getscreenhandle();
	setscreenmode(SCR_AUTOCLOSE_ON_EXIT|0x00000002);
#else
	MF_NLMHandle = GetNLMHandle();
	MF_ScreenID = GetCurrentScreen();
	SetCtrlCharCheckMode(false);
	SetAutoScreenDestructionMode(true);
#endif

	// debug screen
	if (MFT_Debug)
	{
		extern bool MF_NLM_OpenDebugScreen();
		if (!MF_NLM_OpenDebugScreen())
		{
			MF_DisplayCriticalError("MAILFILTER: Unable to create debug screen!\n");
			goto MF_MAIN_TERMINATE;
		}
	}

	// Register Exit Proc ...
#ifndef __NOVELL_LIBC__
	atexit(MF_ExitProc);
#endif
	signal(	SIGTERM	, NLM_SignalHandler	);
	signal(	SIGINT	, NLM_SignalHandler	);


	int rc = 0;
	
	unlink("SYS:\\SYSTEM\\MFINST.NLM");
	unlink("SYS:\\SYSTEM\\MFREST.NLM");
	unlink("SYS:\\SYSTEM\\MFNRM.NLM");
	unlink("SYS:\\SYSTEM\\MFUPGR.NLM");

	switch (MF_GlobalConfiguration.ApplicationMode)
	{
	case MailFilter_Configuration::SERVER:
	
		#ifdef __NOVELL_LIBC__
		NXContextSetName(NXContextGet(),"MailFilterServer");
		#endif

		// Read Configuration from File
		printf(MF_Msg(MSG_BOOT_CONFIGURATION));
		MF_GlobalConfiguration.config_mode_strict = true;
		if (!MF_GlobalConfiguration.ReadFromFile(""))
		{
			MF_DisplayCriticalError("MAILFILTER: Could not read configuration. Terminating!\n");
			goto MF_MAIN_TERMINATE;
		}
		
		rc = MailFilter_Main_RunAppServer(argv[0]);
		break;
		
	case MailFilter_Configuration::CONFIG:
	
		#ifdef __NOVELL_LIBC__
		NXContextSetName(NXContextGet(),"MailFilterConfig");
		#endif

		// Read Configuration from File
		printf(MF_Msg(MSG_BOOT_CONFIGURATION));
		MF_GlobalConfiguration.config_mode_strict = false;
		if (!MF_GlobalConfiguration.ReadFromFile(""))
		{
			MF_DisplayCriticalError("MAILFILTER: Could not read configuration. Terminating!\n");
			goto MF_MAIN_TERMINATE;
		}
		
		--MFT_NLM_ThreadCount;
		rc = MailFilter_Main_RunAppConfig(true);

		break;

	case MailFilter_Configuration::RESTORE:

		#ifdef __NOVELL_LIBC__
		NXContextSetName(NXContextGet(),"MailFilterRestore");
		#endif

		// Read Configuration from File
		printf(MF_Msg(MSG_BOOT_CONFIGURATION));
		MF_GlobalConfiguration.config_mode_strict = true;
		if (!MF_GlobalConfiguration.ReadFromFile(""))
		{
			MF_DisplayCriticalError("MAILFILTER: Could not read configuration. Terminating!\n");
			goto MF_MAIN_TERMINATE;
		}

		--MFT_NLM_ThreadCount;
		rc = MailFilter_Main_RunAppRestore(true);

		rc = 0;
		
		break;

	case MailFilter_Configuration::NRM:
	
		#ifdef __NOVELL_LIBC__
		NXContextSetName(NXContextGet(),"MailFilterNRM");

		// Read Configuration from File
		printf(MF_Msg(MSG_BOOT_CONFIGURATION));
		MF_GlobalConfiguration.config_mode_strict = true;
		if (!MF_GlobalConfiguration.ReadFromFile(""))
		{
			MF_DisplayCriticalError("MAILFILTER: Could not read configuration. Terminating!\n");
			goto MF_MAIN_TERMINATE;
		}

		--MFT_NLM_ThreadCount;
		rc = MailFilter_Main_RunAppNRM(true);

		#else
		rc = -1;
		MF_DisplayCriticalError("MAILFILTER: NRM is not available for MFLT50. Please Upgrade!\n");
		#endif
		
		break;
		
	case MailFilter_Configuration::INSTALL:
		MFD_UseMainScreen();
		printf("  MailFilter_Configuration::INSTALL kick off!\n");
		
		MF_GlobalConfiguration.CreateFromInstallFile(MF_GlobalConfiguration.config_directory + "\\INSTALL.CFG");
		printf("    Saving new configuration to file...\n");
		MF_GlobalConfiguration.WriteToFile("");
		printf("  MailFilter_Configuration::INSTALL complete!\n");
#ifdef __NOVELL_LIBC__
		if (MFT_Debug)
			pressenter();
#endif
		
		break;
		
	case MailFilter_Configuration::UPGRADE:
		MFD_UseMainScreen();
		printf("  MailFilter_Configuration::UPGRADE kick off!\n");
		
		MF_GlobalConfiguration.config_mode_strict = false;
		MF_GlobalConfiguration.ReadFromFile("");
		printf("    Saving new configuration to file...\n");
		MF_GlobalConfiguration.WriteToFile("");
		printf("  MailFilter_Configuration::UPGRADE complete!\n");
#ifdef __NOVELL_LIBC__
		if (MFT_Debug)
			pressenter();
#endif
		
		break;
		
	default:

		MF_DisplayCriticalError("MAILFILTER: Could not run your selected application.\n\tMaybe it is not compiled-in.\n");
	
		rc = -1;
		break;
	}

	extern char* MFT_Local_ServerName;
	if (MFT_Local_ServerName != NULL)
	{
		_mfd_free(MFT_Local_ServerName,"Config:LocalServer");
	}


MF_MAIN_TERMINATE:
	return rc;
}


#endif // N_PLAT_NLM

/*
 *  --- eof ---
 */
