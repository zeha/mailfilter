/*
 +
 +		MFNLM.cpp
 +		
 +		NetWare Platform Specific Functions
 +
 +      CLib AND LibC
 +
 +		This is MailFilter!
 +		www.mailfilter.cc
 +		
 +		Copyright 2001-2004 Christian Hofstädtler.
 +		
 +		
 +
 +
 +
 +
 +		Welcome to this code monster.
 +		
 */

#ifdef N_PLAT_NLM

// Using _MAIN_ to get variables defined here.
#define _MAILFILTER_MAIN_

#define _MFD_MODULE			"MFNLM.CPP"

// Include MailFilter.h
#include "..\..\Main\MailFilter.h"
#include "MFZip.h"
#include "MFRelayHost.h++"
// Include Version Header
#include "..\..\Main\MFVersion.h"
#include <sys/utsname.h>
#ifdef __NOVELL_LIBC__
#include <nks/netware.h>
#endif

// And undef it ... ;)
#undef _MAILFILTER_MAIN_

#include "MFConfig.h++"


extern void MailFilter_NRM_sigterm();

// NUT: Local Prototypes
static int MF_NutVerifyExit(void);
//static bool MF_NutInit();
static void MF_NutHandlerKeyF4 (void *handle);
static void MF_NutHandlerKeyF5 (void *handle);
static void MF_NutHandlerKeyF6 (void *handle);
static void MF_NutHandlerKeyF7 (void *handle);
static void MF_NutHandlerKeyF8 (void *handle);
static void MF_NutHandlerKeyF9 (void *handle);
// NUT: Status Portal Handle
static PCB *statusPortalPCB = NULL;
static PCB *infoPortalPCB = NULL;

// Thread Group ID
// Event Handle for Server ShutDown event
static LONG eventHandleShutDown = (LONG)-1;
static bool MFT_NUT_GetKey = true;
// Time ...
static tm tmp_TimeTM;
static time_t tmp_Time;

static int MF_NutMutexQueryUser = 0;



//
//
//


void MF_ShutDown(void)
{
	MFT_NLM_Exiting = 255;
//	raise(SIGTERM);
}

#ifdef __NOVELL_LIBC__
void _NonAppStop ( void )
{
	MF_StatusFree();
//	MF_ConfigFree();
	MF_ExitProc();
}
#endif

extern "C" { 
	extern int NWIsNLMLoadedProtected(void);
}

bool mf_nlmisloadedprotected()
{
	return (bool)NWIsNLMLoadedProtected();
}



//
//  Event Handler For NLM ShutDown
//
void MF_ExitProc(void)
{
#ifndef __NOVELL_LIBC__
	NWDSCCODE          ccode;
#endif

	// Shut down NUT	
	if (MF_NutInfo != NULL)
	{
		NWSRestoreNut ( MF_NutInfo );
		MF_NutInfo = NULL;
		
		statusPortalPCB = NULL;
		infoPortalPCB = NULL;
	}
#ifndef __NOVELL_LIBC__
	// Deregister Shutdown events ...
	if(eventHandleShutDown != NULL)
	{
		UnregisterForEvent(eventHandleShutDown);
		eventHandleShutDown = NULL;
	}
	
	// Log out from Server.
	if ( MFT_NLM_Connection_Handle != NULL)
	{
		NWCCCloseConn ( MFT_NLM_Connection_Handle );
	}
	
	if (MFT_NLM_Connection_HandleCLIB != 0)
	{
		SetCurrentConnection(0);
		LogoutObject(MFT_NLM_Connection_HandleCLIB);
	}
	
	// Log out from NDS.
	if (MFT_NLM_DS_Context != NULL)
	{
		// DS Logout
		ccode = NWDSLogout ( MFT_NLM_DS_Context );
		if(ccode)
			fprintf(stderr,"MailFilter: NWDSLogout returned %X\n", ccode);

		// DSLIB Free Context 
		ccode = NWDSFreeContext( MFT_NLM_DS_Context );
		if(ccode)
			fprintf(stderr,"MailFilter: NWDSFreeContext returned %X\n", ccode);

	}

	// Shutdown Unicode
	NWFreeUnicodeTables();

	// Disable NWNET
	NWNetTerm(NULL);

	// Disable NWCLX
	NWCLXTerm(NULL);

	// Disable NWCAL
//	NWCallsTerm(NULL);
#endif
	
	// Destroy Screen ...
	if (MFD_ScreenID)	CloseScreen ( MFD_ScreenID );
#ifndef __NOVELL_LIBC__
	if (MF_ScreenID )	CloseScreen ( MF_ScreenID  );
#endif

}

void MF_NutHandlerKeyF4 (void *handle)		// Handler F5 = ShowConfig()
{
	int rc = 0;
	handle = handle;			// Keep compiler quiet.

	MFT_NLM_Exiting = 253;
	
	// We need this to wake the UI thread up ...
	NWSUngetKey ( K_ESCAPE , 0 , MF_NutInfo );

	return;
}
void MF_NutHandlerKeyF5 (void *handle)		// Handler F5 = ShowConfig()
{
	handle = handle;			// Keep compiler quiet.
	MF_UI_ShowConfiguration();
}
void MF_NutHandlerKeyF6 (void *handle)		// Handler for configuration reread
{
	int rc = 0;
	handle = handle;			// Keep compiler quiet.

	MFT_NLM_Exiting = 254;
	
	MF_StatusText("Preparing to restart MailFilter...");

	// We need this to wake the UI thread up ...
	NWSUngetKey ( K_ESCAPE , 0 , MF_NutInfo );

	return;
}
void MF_NutHandlerKeyF7 (void *handle)		// Handler F7 = Exit!()
{
	
	if (MF_NutVerifyExit())
	{
		MF_StatusText("Shutting Down On Keyboard Request ...");
		fprintf(stderr,"MAILFILTER: Shutting Down On Keyboard Request ...\n");

		// Disable Func. Keys so user can't exit a 2nd time
		NWSDisableAllInterruptKeys((NUTInfo*)handle);

//		raise(SIGTERM);
		system("MFSTOP");
	}
}
void MF_NutHandlerKeyF8 (void *handle)		// Handler to display release infos
{
	handle = handle;			// Keep compiler quiet.

	char buf[82];
	buf[0] = 0;

#ifdef MAILFILTER_VERSION_BETA
	strcpy(buf,"BETA ");
#endif
#ifdef _TRACE
	strcpy(buf,"DBUG ");
#endif

	strcat(buf,"MailFilter professional Server (NLM) ");
	strcat(buf,MAILFILTERVERNUM);

	MF_StatusUI_UpdateLog(buf);

	sprintf(buf,"Language ID: %d, Messages: %d, Compiled: %s.", MFT_I18N_LanguageID, MFT_I18N_MessageCount, MAILFILTERCOMPILED);
	MF_StatusUI_UpdateLog(buf);

#ifdef MAILFILTER_VERSION_BETA
	sprintf(buf,"** WARNING: BETA - USE AT YOUR OWN RISK! **");
	MF_StatusUI_UpdateLog(buf);
#endif // MAILFILTER_VERSION_BETA
#ifdef _TRACE
	sprintf(buf,"** WARNING: DEBUG = ON | In Debugger: type G to continue! **");
	MF_StatusUI_UpdateLog(buf);
#endif // _TRACE


#ifdef MF_WITH_ZIP
	sprintf(buf,"** Contains EXPERIMENTAL on-the-fly zip support **");
	MF_StatusUI_UpdateLog(buf);
#endif


}
void MF_NutHandlerKeyF9 (void *handle)		// Handler F6 = ShowDetails()
{
	handle = handle;			// Keep compiler quiet.

	char buf[81];

	MF_CheckProblemDirAgeSize();

#ifdef _MF_MEMTRACE
	sprintf(buf,"AllocCount: %d",MFD_AllocCounts);
	MF_StatusUI_UpdateLog(buf);
#endif

	sprintf(buf,"I have %d rules in memory.",MF_CountAllFilters());
	MF_StatusUI_UpdateLog(buf);

	sprintf(buf,"StackAvail: %d, DebugLevel: %d",stackavail(),MFT_Debug);
	MF_StatusUI_UpdateLog(buf);
	
	unlink("SYS:\\System\\mailflt.tmp");
	unlink("SYS:\\System\\mailflt.mfz");

	FILE* f = fopen("SYS:\\System\\mailflt.tmp","wt");
	fprintf(f,"This is a test MFZ (ZIP) file generated by MailFilter pro!\nIt should contain: ReadMe.txt (this file), MailFlt.nlm and if existed MailFlt.bak.\n");	
	fclose(f);
	
	MFZip *zip;
	zip = new MFZip("SYS:\\System\\MailFlt.MFZ",9,1);
	zip->AddFile("MailFlt.NLM","SYS:\\System\\mailflt.nlm");
	zip->AddFile("ReadMe.TXT","SYS:\\System\\mailflt.tmp");
	zip->AddFile("MailFlt.BAK","SYS:\\System\\mailflt.bak");
	delete(zip);		// close it

	unlink("SYS:\\System\\mailflt.tmp");
	MF_StatusUI_UpdateLog("Wrote a test ZIP to SYS:\\System\\MailFlt.MFZ.");
	
	MF_StatusUI_UpdateLog("Note: DNBL-Test always uses bl.spamcop.net.");
	MFRelayHost bl("127.0.0.2"); //("bl.spamcop.net","127.0.0.2");
	if (bl.LookupRBL_DNS("bl.spamcop.net","127.0.0.1"))
		MF_StatusUI_UpdateLog("Test: 127.0.0.2 is blacklisted.");
		else
		MF_StatusUI_UpdateLog("Test: 127.0.0.2 is _not_ blacklisted.");

}


void MFL_VerInfo()
{
	MF_StatusText(MF_Msg(MSG_EVAL_LINE1));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE2));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE3));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE4));
	MF_StatusNothing();
}

//
//  NUT: Initialize NUT and Tags
//
bool MF_NutInit(void)
{
	#ifdef __NOVELL_LIBC__
	rtag_t	tagID;
	#else
	LONG	tagID;
	#endif
	long	ccode;


	strcpy(MF_Msg(PROGRAM_VERSION),MAILFILTERVERNUM);
	#ifdef __NOVELL_LIBC__
	tagID=AllocateResourceTag(
		/*	NLMHandle			*/	MF_NLMHandle,
		/*	descriptionString	*/	MF_Msg(PROGRAM_TAG_NAME),
		/*	resourceType		*/	AllocSignature
		);
	#else
	tagID=AllocateResourceTag(
		/*	NLMHandle			*/	MF_NLMHandle,
		/*	descriptionString	*/	(const unsigned char *)MF_Msg(PROGRAM_TAG_NAME),
		/*	resourceType		*/	AllocSignature
		);
	#endif
	if(tagID == NULL) {	return false;	}

	ccode=NWSInitializeNut(
		/*	utility				*/	PROGRAM_NAME,
		/*	version				*/	PROGRAM_VERSION,
		/*	headerType			*/	SMALL_HEADER,	//NORMAL_HEADER,
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
	if(ccode != NULL) {	return false;	}

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
  
	statusPortal = 
		NWSCreatePortal(
			6,
			0,
			18,
			80,
			18,
			80,
			SAVE,
			NULL, //(unsigned char *)"Application Messages",
			VNORMAL,
			SINGLE,
			VNORMAL,
			CURSOR_OFF,
			VIRTUAL,
			MF_NutInfo);

	LONG	infoPortal;
	infoPortal = NWSCreatePortal(
			1,
			0,
			5,
			80,
			5,
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

	NWSGetPCB(&infoPortalPCB, infoPortal, MF_NutInfo);
	NWSSelectPortal(infoPortal, MF_NutInfo);
	NWSClearPortal(infoPortalPCB);
	
	char* szTemp = "Initializing...";
	
	NWSShowPortalLine(0, 0, (_MF_NUTCHAR) szTemp, strlen(szTemp), infoPortalPCB);
	NWSUpdatePortal(infoPortalPCB);
	NWSDeselectPortal(MF_NutInfo);

	NWSEnableFunctionKey (	K_F4 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F5 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F6 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F7 , MF_NutInfo );
	NWSEnableFunctionKey (	K_F8 , MF_NutInfo );
	NWSEnableFunctionKey (  K_F9 , MF_NutInfo );

	NWSEnableInterruptKey (	K_F4 , *MF_NutHandlerKeyF4, MF_NutInfo );
	NWSEnableInterruptKey (	K_F5 , *MF_NutHandlerKeyF5, MF_NutInfo );
	NWSEnableInterruptKey (	K_F6 , *MF_NutHandlerKeyF6, MF_NutInfo );
	NWSEnableInterruptKey (	K_F7 , *MF_NutHandlerKeyF7, MF_NutInfo );
	NWSEnableInterruptKey (	K_F8 , *MF_NutHandlerKeyF8, MF_NutInfo );
	NWSEnableInterruptKey ( K_F9 , *MF_NutHandlerKeyF9, MF_NutInfo );

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

//	if (statusPortalPCB == NULL)	return;

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
			20L,		//Height
			78,			//Width
			VNORMAL,	//LONG   attribute, 
			1L,			//LONG   count, 
			V_UP,		//LONG   direction,
			statusPortalPCB);

	NWSShowPortalLine(15, 0, (_MF_NUTCHAR) szTemp, strlen(szTemp), statusPortalPCB);
	
	NWSUpdatePortal(statusPortalPCB);
}

void MF_StatusUI_Update(int newText)
{
	MF_StatusUI_Update(MF_Msg(newText));
}

					   //  0   1    2   3   
static char cStatus[] = { '|','\\','-','/' };
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

//	if ( (newText != NULL) && (newText[0] != 0) )
//	{
//		MFD_Out("%s\n",newText);

/*
		// Update Status Line
		for (i=0;i<81;i++)
		{
			szTemp[i]=' ';
		}
		strncpy(szTemp,newText,80);
		szTemp[80]=0;
		NWSShowLineAttribute ( 24, 0, (unsigned char*)szTemp, VREVERSE, 80, (struct ScreenStruct*)MF_NutInfo->screenID);
*/	
	
//	}

	// Update Info Portal

// 
	if (!MFT_NLM_Exiting)
	{
		static unsigned long lastClck;
		static int iStatusChar;
		
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
		
		sprintf(szTemp,MF_Msg(INFOPORTAL_LINE1),MFC_MAILSCAN_Enabled == 0 ? "Off" : "On ",days,hours,minutes);
		NWSShowPortalLine(0, 0, (_MF_NUTCHAR) szTemp, strlen(szTemp), infoPortalPCB);
		

		sprintf(szTemp,MF_Msg(INFOPORTAL_LINE2),MFBW_CheckCurrentScheduleState() == false ? "On " : "Off",MFS_MF_MailsInputTotal,MFS_MF_MailsInputFailed);
		NWSShowPortalLine(1, 0, (_MF_NUTCHAR) szTemp, strlen(szTemp), infoPortalPCB);

		sprintf(szTemp,MF_Msg(INFOPORTAL_LINE3),(1500+MFT_SleepTimer)/1000,MFS_MF_MailsOutputTotal,MFS_MF_MailsOutputFailed);
		szTemp[1] = cStatus[iStatusChar];
		NWSShowPortalLine(2, 0, (_MF_NUTCHAR) szTemp, strlen(szTemp), infoPortalPCB);

		NWSUpdatePortal(infoPortalPCB);
	}	
	
	return;
}



#ifndef _MF_CLEANBUILD
void MFD_UseMainScreen()
{
	if (MFD_ScreenID)	CloseScreen ( MFD_ScreenID );
	MFD_ScreenID = NULL;
}

void MFD_Out_func(int attr, const char* format, ...) {

	va_list	argList;
	bool show = ( chkFlag(MFT_Debug,attr) == 1 );

	if (!MFT_Verbose)
		return;

	if (MFT_NLM_Exiting)
		return;

	switch (attr)
	{
		case MFD_SOURCE_GENERIC:	{	attr = COLOR_SILVER;	break;	}
		case MFD_SOURCE_WORKER:		{	attr = COLOR_CYAN;		break;	}
		case MFD_SOURCE_CONFIG:		{	attr = COLOR_GREEN;		break;	}
		case MFD_SOURCE_VSCAN:		{	attr = COLOR_BLUE;		break;  }
		case MFD_SOURCE_ZIP:		{	attr = COLOR_TEAL;		break;  }
		case MFD_SOURCE_ERROR:		{	attr = COLOR_RED;		break;  }
		case MFD_SOURCE_RULE:		{	attr = COLOR_YELLOW;	break;	}
		case MFD_SOURCE_MAIL:		{	attr = COLOR_PURPLE;	break;	}
		case MFD_SOURCE_SMTP:		{	attr = COLOR_WHITE;		break;	}
		default:					{	attr = COLOR_GREY;		break;  }
	}

	if (!show)
		return;

	va_start(argList, format);

#ifndef __NOVELL_LIBC__

	int oldScreen = 0;
	if (MFD_ScreenID)	oldScreen=SetCurrentScreen(MFD_ScreenID);   
	
	vprintf(format, argList);
	if (oldScreen)		SetCurrentScreen(oldScreen);

#else

	char buffer[5000];
	vsnprintf(buffer,4999,format,argList);
	buffer[4999] = 0;
	
	// MF_UseMainScreen() support
	if (MFD_ScreenID)
		OutputToScreenWithAttribute(MFD_ScreenID,(unsigned char)attr,buffer);
		else
		OutputToScreenWithAttribute(MF_ScreenID,(unsigned char)attr,buffer);
	
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
#else
				if (MF_Thread_Work > 0)
					NXThreadContinue (MF_Thread_Work);

				if (MF_Thread_SMTP > 0)
					NXThreadContinue (MF_Thread_SMTP);
#endif
				// NLM SDK functions may be called here
				MF_StatusText(MF_Msg(MSG_UNLOADWAIT));

				if (MF_NutInfo != NULL)
				{
					// We need this to wake the UI thread up ...
					NWSUngetKey ( K_ESCAPE , 0 , MF_NutInfo );
				}
				
				if ( (MF_GlobalConfiguration.ApplicationMode == MailFilter_Configuration::NRM) || (MF_GlobalConfiguration.NRMInitialized == true) )
				{
					MailFilter_NRM_sigterm();
				}

#ifndef __NOVELL_LIBC__
				SetThreadGroupID(handlerThreadGroupID);
#else
				ungetcharacter(27);
#endif

				while (MFT_NLM_ThreadCount > 0)
				{
//					gotorowcol(49,1);
//					printf("%d",MFT_NLM_ThreadCount);
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

static bool MF_NLM_InitDS()
{
#if 0 //ndef __NOVELL_LIBC__
//	NWCCODE				ccode;
	NWDSCCODE			dsccode;
	LCONV				lConvInfo;


	/* Initialize NWCalls */
/*	ccode = NWCallsInit(NULL, NULL);
	if(ccode)
	{
		fprintf(stderr,"MailFilter: NWCallsInit returned %X\n", ccode);
	 	return false;
	}
*/
	/* Initialize NWClx */
	dsccode = NWCLXInit(NULL, NULL);
	if(dsccode)
	{
		fprintf(stderr,"MailFilter: NWCLXInit returned %X\n", dsccode);
	 	return false;
	}

	/* Initialize the unicode tables */
	NWLlocaleconv(&lConvInfo);
	dsccode = NWInitUnicodeTables( lConvInfo.country_id, lConvInfo.code_page );
	if(dsccode)
	{
		fprintf(stderr,"MailFilter: NWInitUnicodeTables error %X \n", dsccode);
		return false;
	}

	/* Initialize NWNet */
	dsccode = NWNetInit(NULL,NULL);
	if(dsccode)
	{
		fprintf(stderr,"MailFilter: NWNetInit returned %X\n",dsccode);
	 	return false;
	}



	if (!MFT_RemoteDirectories)
	{
		
/*		NWDSCreateContextHandle(&MFT_NLM_DS_Context);
		
		// Login as the Server object ...
		ccode = NWDSLoginAsServer ( MFT_NLM_DS_Context );
		if(ccode){
			switch (ccode) {
				case -601:
					// user object not found
					MF_StatusText("Login: User object not found!");
					break;
				case -669:
					// auth failed
					MF_StatusText("Login: Failed authentication. Check Password.");
					break;
				default:
					MF_StatusText("Login: Generic Error. Please check Console.");
					fprintf(stderr,"MailFilter: Login Error: NWDSLogin failed (%d)!\n",ccode);
			}
			return false;
		}

		SetCurrentFileServerID(0);
//		SetCurrentConnection(-1);
		MFT_NLM_Connection_HandleCLIB = GetCurrentConnection();
		fprintf(stderr,"CLIB Connection (Local): %d\n",MFT_NLM_Connection_HandleCLIB);
//		NWGetConnectionNumber( (NWCONN_HANDLE)MFT_NLM_Connection_HandleCLIB, &connnum );

/*		// Create authenticated Connection.
	//	dsccode = NWDSAuthenticate ( (NWCONN_HANDLE)MFT_NLM_Connection_HandleCLIB , 0 , NULL );
		dsccode = NWDSAuthenticateConnEx ( MFT_NLM_DS_Context , MFT_NLM_Connection_Handle );
		if (dsccode != 0)
		{
			MF_StatusText("Login: Authentication to server failed.");
			fprintf(stderr,"MailFilter: Login Error: NWDSAuthenticate returned %d!\n",ccode);
		}

*//*		LONG oldTask = SetCurrentTask(-1);
		LONG myTask = GetCurrentTask();
*//*		SetCurrentConnection(-1);
		MFT_NLM_Connection_HandleCLIB = GetCurrentConnection();
		fprintf(stderr,"CLIB Connection (Local): %d\n",MFT_NLM_Connection_HandleCLIB);
*//*

		// Login as the Server object ...
	//	dsccode = LoginObject  ( MFT_NLM_Connection_Handle ,
	//					MFT_Local_ServerName,
	//					0x0004,
	//					""); 
		dsccode = LoginToFileServer (
			MFT_Local_ServerName,
			0x0004,
			"");
		MFT_NLM_Connection_HandleCLIB = GetCurrentConnection();
		fprintf(stderr,"CLIB Connection (Local): %d\n",MFT_NLM_Connection_HandleCLIB);

		if(dsccode){
			switch (dsccode) {
				case -601:
					// user object not found
					MF_StatusText("Login: User object not found!");
					break;
				case -669:
					// auth failed
					MF_StatusText("Login: Failed authentication. Check Password.");
					break;
				default:
					MF_StatusText("Login: Generic Error. Please check Console.");
					fprintf(stderr,"MailFilter: Login Error: NWDSLogin failed (%d)!\n",dsccode);
			}
			ReturnBlockOfTasks(myTask,1);
			Logout();
			return false;
		}

		MFT_NLM_Connection_HandleCLIB = GetCurrentConnection();
		fprintf(stderr,"CLIB Connection (Local): %d\n",MFT_NLM_Connection_HandleCLIB);
		SetCurrentTask(oldTask);
*/
		SetCurrentConnection(0);
	} else {
	
		/*******************************************************************
		NDS: create context and set current context to Root
		*/
/*		dsccode = NWDSCreateContextHandle (&MFT_NLM_DS_Context);
		if(dsccode) {
			 fprintf(stderr,"MailFilter: Error creating context. %d.\n",dsccode);
			 MFT_NLM_DS_Context = NULL;
			 return false;
		}

		dsccode = NWDSSetContext(MFT_NLM_DS_Context, DCK_NAME_CONTEXT, (void*)"[Root]");
		if(dsccode){
			 fprintf(stderr,"MailFilter: Error: NWDSSetContext() returned: %d\n",dsccode);
			 return false;
		}

*/
		/*******************************************************************
		Server: Open Connection
		*/
		char* szServerName = (char*)_mfd_malloc(MAX_PATH,"MFNLM: szServerName");
		MF_GetServerName ( szServerName );

		fprintf(stderr, "MailFilter: Opening Connection to %s ...\n", szServerName );
		
		dsccode = NWCCOpenConnByName(
	             /* Handle to resolve name */  (nuint)0, 
	             /* Server name            */  strupr(szServerName),
	             /* Server name format     */  NWCC_NAME_FORMAT_BIND, 
	             /* Connection open state  */  NWCC_OPEN_LICENSED, 
	             /* Transport Type         */  NWCC_TRAN_TYPE_WILD,
	             /* Connection Handle      */  &MFT_NLM_Connection_Handle);

		_mfd_free(szServerName,"MFNLM: szServerName");

		if (dsccode)
		{
			MF_StatusText("Error connecting to target server!");
			fprintf(stderr,"MailFilter: NWCCOpenConnByName returned %d.\n",dsccode);
			MFT_NLM_Connection_Handle = NULL;
			return false;
		}
		

		NWCCSetCurrentConnection ( MFT_NLM_Connection_Handle );
		
		MFT_NLM_Connection_HandleCLIB = GetCurrentConnection();
		fprintf(stderr,"Connection: %d\n",MFT_NLM_Connection_HandleCLIB);



		NWCCSetCurrentConnection ( MFT_NLM_Connection_Handle );
		MFT_NLM_Connection_HandleCLIB = GetCurrentConnection();
		fprintf(stderr,"CLIB Connection (Local): %d\n",MFT_NLM_Connection_HandleCLIB);



		/*******************************************************************
		Now Login into NDS
		*/
/*
		// login
	//	ccode = NWDSLogin(MFT_NLM_DS_Context, 0, ".admin.system", "dakava", 0);

		// Login as the Server object ...
		ccode = NWDSLoginAsServer ( MFT_NLM_DS_Context );
		if(ccode){
			switch (ccode) {
				case -601:
					// user object not found
					MF_StatusText("Login: User object not found!");
					break;
				case -669:
					// auth failed
					MF_StatusText("Login: Failed authentication. Check Password.");
					break;
				default:
					MF_StatusText("Login: Generic Error. Please check Console.");
					fprintf(stderr,"MailFilter: Login Error: NWDSLogin failed (%d)!\n",ccode);
			}
			return false;
		}

		// Create authenticated Connection.
		dsccode = NWDSAuthenticate ( MFT_NLM_Connection_Handle , 0 , NULL );
	//	dsccode = NWDSAuthenticateConnEx ( MFT_NLM_DS_Context , MFT_NLM_Connection_Handle );
		if (dsccode != 0)
		{
			MF_StatusText("Login: Authentication to server failed.");
			fprintf(stderr,"MailFilter: Login Error: NWDSAuthenticate returned %d!\n",ccode);
		}
*/

	// this should work:
/*		ccode = NWLoginToFileServer  ( MFT_NLM_Connection_Handle,
						MFT_Local_ServerName,
						OT_WILD,
						0);  
						
		if (ccode)
		{
			switch (ccode)
			{
				case -1:
					MF_StatusText("Login Failed.");
					break;
				case 150:
					fprintf(stderr,"MailFilter: Out of memory!\n");
					break;
				default:
					fprintf(stderr,"MailFilter: Login Failed with error %d.\n",ccode);
					break;
			}
			return false;
		}
		
		NWCCSetCurrentConnection ( MFT_NLM_Connection_Handle );
		MFT_NLM_Connection_HandleCLIB = GetCurrentConnection();
		fprintf(stderr,"CLIB Connection (probably remote): %d\n",MFT_NLM_Connection_HandleCLIB);
	*/}
#endif
	return true;
}

#ifdef _MF_MEMTRACE
static void _mfd_tellallocccountonexit()
{
	fprintf(stderr,"MAILFILTER: AllocCount: %d\n",MFD_AllocCounts);
}
#endif //_MF_MEMTRACE



static void LoadNRMThreadStartup(void *dummy)
{
#pragma unused(dummy)

	MF_GlobalConfiguration.NRMInitialized = true;
	MailFilter_Main_RunAppNRM();
}

//
//
//  NLM Application Startup Function:
//   SERVER
//
//   * Start Thread(s) ...
//   * Run NUT in foreground thread.
//
static int MailFilter_Main_RunAppServer()
{	
	// Init NWSNut
	printf(MF_Msg(MSG_BOOT_USERINTERFACE));

	if (!MailFilterApp_Server_InitNut())
		goto MF_MAIN_TERMINATE;
	
	// DS and Server Connections ...
	if (!MF_NLM_InitDS())
		goto MF_MAIN_TERMINATE;
	
MF_MAIN_RUNLOOP:
	{
		// needed for mf restart
		MFT_NLM_Exiting = 0;

		MFD_Out(MFD_SOURCE_GENERIC,"Initializing Logging...\n");

		// Init Status
		if (!MF_StatusInit())
			goto MF_MAIN_TERMINATE;

		// get OS info
		{
			char szStatusMsg[82];
		#ifndef __NOVELL_LIBC__
			struct utsname un;
			
			MF_StatusLog("MailFilter CLIB Version "MAILFILTERVERNUM);
			MF_StatusText("MailFilter for NetWare 5.0, 5.1 - 5.1 SP5, 6.0 - 6.0 SP2");

			uname(&un);
			
			sprintf(szStatusMsg,"Operating System: Novell NetWare %s.%s",un.release,un.version);

			MF_StatusText(szStatusMsg);
			
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
	    
		// Start Thread: ** WORK **
	#ifdef __NOVELL_LIBC__
		NXContext_t ctx;

		if (NXThreadCreateSx( MF_Work_Startup , 
			NULL,
			NX_THR_JOINABLE|NX_THR_BIND_CONTEXT,
			&ctx, 
			&MF_Thread_Work
			))
	#else
		MF_Thread_Work = BeginThread(MF_Work_Startup,NULL,2*65536,NULL);						// Set 64k Stack for new thread
		if( MF_Thread_Work == EFAILURE )
	#endif
		{
			fprintf(stderr,MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
			MF_Thread_Work = 0;
			goto MF_MAIN_TERMINATE;
		}
		
		if (MF_GlobalConfiguration.EnableNRMThread == true)
		{
			if (!mf_nlmisloadedprotected())
			{
				MFD_Out(MFD_SOURCE_CONFIG,"NRM thread will be started.\n");
			
				// Start Thread: ** NRM **
				#ifdef __NOVELL_LIBC__
				NXContext_t ctx;

				if (NXThreadCreateSx( LoadNRMThreadStartup , 
					NULL,
					NX_THR_JOINABLE|NX_THR_BIND_CONTEXT,
					&ctx, 
					&MF_Thread_NRM
					))
				#else
				MF_Thread_NRM = BeginThread(LoadNRMThreadStartup,NULL,2*65536,NULL);						// Set 64k Stack for new thread
				if( MF_Thread_NRM == EFAILURE )
				#endif
				{
					//fprintf(stderr,MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
					MF_StatusText("ERROR: NRM Thread Startup Error. NRM NOT loaded.");
					MF_Thread_NRM = 0;
		//			goto MF_MAIN_TERMINATE;
				}
			} else {
				MFD_Out(MFD_SOURCE_CONFIG,"NRM thread will NOT be started --> not in OS space.\n");
				MF_StatusText("NOTE: NOT loading NRM -- MailFilter is not loaded in OS address space");
			}
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
				fprintf(stderr,MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
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
		while (MFT_NLM_ThreadCount>0)
		{
			if (MFT_NLM_Exiting > 0)
				break;

			if (MFT_NUT_GetKey)
				NWSGetKey (
					&tmp_Key_Type,
					&tmp_Key_Value,
					MF_NutInfo
					);
			
			ThreadSwitch();
		}

		// 254 = restart
		// 253 = config
		if ((MFT_NLM_Exiting == 254) || (MFT_NLM_Exiting == 253))
		{
			bool bDoConfig = false;
			if (MFT_NLM_Exiting == 253) bDoConfig = true;
		
			MF_StatusText("  Terminating modules for restart...");

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
#endif

			if ( MF_GlobalConfiguration.NRMInitialized == true )
			{
				MFD_Out(MFD_SOURCE_CONFIG,"shutting down NRM thread\n");
				MailFilter_NRM_sigterm();
			}


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
				fprintf(stderr,"MAILFILTER: Restart failed\n");
				fprintf(stderr,"\tConfiguration Module reported an unrecoverable error.\n");
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
					fprintf(stderr,"MAILFILTER: Restart failed\n");
					fprintf(stderr,"\tConfiguration Module reported an unrecoverable error.\n");
					goto MF_MAIN_TERMINATE;
				}
			}
							
			if (!MailFilterApp_Server_InitNut())
			{
				fprintf(stderr,"MAILFILTER: Restart failed\n");
				fprintf(stderr,"\tUI Module reported an unrecoverable error.\n");
				goto MF_MAIN_TERMINATE;
			}

			// increase thread count so we can decrease it above again.
			++MFT_NLM_ThreadCount;

			goto MF_MAIN_RUNLOOP;
		}
	}

MF_MAIN_TERMINATE:
	return 0;
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

#ifdef _MF_MEMTRACE
	atexit(_mfd_tellallocccountonexit);
#endif //_MF_MEMTRACE

#ifdef MF_WITH_I18N
	if (LoadLanguageMessageTable(&programMesgTable, &MFT_I18N_MessageCount, &MFT_I18N_LanguageID))
	{
		fprintf(stderr,"MAILFILTER: CRITICAL ERROR:\n\tCan't load Message Tables.\n");
		exit(0); 
	}
#ifndef _MF_CLEANBUILD
	fprintf(stderr,"MAILFILTER: Language ID: %l, Messages Loaded: %l\n",MFT_I18N_LanguageID,MFT_I18N_MessageCount);
#endif
#endif

	printf("%s\n",MF_Msg(MSG_BOOT_LOADING));
	
	
#ifndef __NOVELL_LIBC__
	// Save Thread Group
	mainThread_ThreadGroupID = GetThreadGroupID();

	// Rename UI Thread
	RenameThread( GetThreadID() , MF_Msg(THREADNAME_MAIN) );
	
	fprintf(stderr,"MAILFILTER: Info: This is the Legacy version of MailFilter!\n\tUse only on NetWare 5.0, NetWare 5.1 prior SP6 or NetWare 6.0 prior SP3.\n");
#else
	{
		struct utsname u;
		uname(&u);
	
		if ( 
			(u.netware_major == 6) &&
			(u.netware_minor == 0) &&
			(u.servicepack < 2)
			)
			{
				fprintf(stderr,"MAILFILTER: Detected NetWare 6.0 prior to SP3.\n\tPlease upgrade to a newer version or use the legacy MFLT50.NLM\n");
				return 0;
			}
		if ( 
			(u.netware_major == 5) &&
			(u.netware_minor == 1) &&
			(u.servicepack < 6)
			)
			{
				fprintf(stderr,"MAILFILTER: Detected NetWare 5.1 prior to SP6.\n\tPlease upgrade to a newer version or use the legacy MFLT50.NLM\n");
				return 0;
			}
		if ( 
			(u.netware_major == 5) &&
			(u.netware_minor == 0)
			)
			{
				fprintf(stderr,"MAILFILTER: Detected NetWare 5.0.\n\tPlease upgrade to a newer version or use the legacy MFLT50.NLM\n");
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
	MF_NLMHandle = getnlmhandle();
	MF_ScreenID = getscreenhandle();
	setscreenmode(SCR_AUTOCLOSE_ON_EXIT|0x00000002);
#else
	MF_NLMHandle = GetNLMHandle();
	MF_ScreenID = GetCurrentScreen();
	SetCtrlCharCheckMode(false);
#endif

	// init Namespaces and so on... **** DONT DO THIS **** ON NW411 THIS WONT WORK ****
//	SetCurrentNameSpace (NW_NS_LONG);
//	SetTargetNameSpace(LONGNameSpace);

	if (MFT_Verbose)
	{
#ifdef __NOVELL_LIBC__
		MFD_ScreenTag = AllocateResourceTag( MF_NLMHandle, "MailFilter Debug Screen", ScreenSignature);
		if (OpenScreen ( "MailFilter Debug", MFD_ScreenTag, &MFD_ScreenID))
		{
			fprintf(stderr,"MAILFILTER: Unable to create debug screen!\n");
			goto MF_MAIN_TERMINATE;
		}
#else
		MFD_ScreenID = CreateScreen ( "MailFilter Debug", DONT_AUTO_ACTIVATE | DONT_CHECK_CTRL_CHARS ); 	// | AUTO_DESTROY_SCREEN
#endif
	}

#ifndef __NOVELL_LIBC__
	// Register Event Handle for Shutdown
	eventHandleShutDown = RegisterForEvent(EVENT_DOWN_SERVER,	eventShutDownReport,	eventShutDownWarn);
	if(eventHandleShutDown == -1)
	{
		fprintf(stderr,MF_Msg(CONMSG_MAIN_ERRREGSHUTDOWNHANDLER));
		goto MF_MAIN_TERMINATE;
	}
	SetAutoScreenDestructionMode(true);
#endif

	// Register Exit Proc ...
#ifndef __NOVELL_LIBC__
	atexit(MF_ExitProc);
#endif
	signal(	SIGTERM	, NLM_SignalHandler	);
	signal(	SIGINT	, NLM_SignalHandler	);


	int rc = 0;

	switch (MF_GlobalConfiguration.ApplicationMode)
	{
	case MailFilter_Configuration::SERVER:
	
		#ifdef __NOVELL_LIBC__
		NXContextSetName(NXContextGet(),"MailFilterServer");
		#endif

		// Read Configuration from File
		printf(MF_Msg(MSG_BOOT_CONFIGURATION));
		MF_GlobalConfiguration.config_mode_strict = true;
		if (!MF_GlobalConfiguration.ReadFromFile(""))	goto MF_MAIN_TERMINATE;

		rc = MailFilter_Main_RunAppServer();
		
		break;
		
	case MailFilter_Configuration::CONFIG:
	
		#ifdef __NOVELL_LIBC__
		NXContextSetName(NXContextGet(),"MailFilterConfig");
		#endif

		// Read Configuration from File
		printf(MF_Msg(MSG_BOOT_CONFIGURATION));
		MF_GlobalConfiguration.config_mode_strict = false;
		if (!MF_GlobalConfiguration.ReadFromFile(""))	goto MF_MAIN_TERMINATE;

		--MFT_NLM_ThreadCount;
		rc = MailFilter_Main_RunAppConfig(true);

		break;

	case MailFilter_Configuration::RESTORE:

		system("LOAD MFREST.NLM");
		rc = 0;
		
		break;

	case MailFilter_Configuration::NRM:
	
		#ifdef __NOVELL_LIBC__
		NXContextSetName(NXContextGet(),"MailFilterNRM");
		#endif

		// Read Configuration from File
		printf(MF_Msg(MSG_BOOT_CONFIGURATION));
		MF_GlobalConfiguration.config_mode_strict = true;
		if (!MF_GlobalConfiguration.ReadFromFile(""))	goto MF_MAIN_TERMINATE;

		rc = MailFilter_Main_RunAppNRM();
		
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
		
	default:

		fprintf(stderr,"MAILFILTER: Could not run your selected application.\n\tMaybe it is not compiled-in.\n");
	
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
 *
 *
 *  --- eof ---
 *
 *
 */

