/****************************************************************************
**	File:	MFNRM.C
**
**	Desc:	MailFilter Remote Manager Plug-In
**
**  (C) Copyright 2001-2002 Christian Hofstädtler. All Rights Reserved.
**							
*/

#define _MFD_MODULE			"MFNLM-NRM.CPP"
//#include "nrm.h"
#include "MailFilter.h"
#include "dynaload-nrm.h"
//#include <nwfileio.h> 

/* not to exceed 35 chars*/
#define MAILFILTER_NRM_HEADER "MailFilter"
#define MAILFILTER_NRM_SERVICETAG_STATUS 		"MailFilterStatus"
#define MAILFILTER_NRM_SERVICETAG_STATUS_LEN 16
#define MAILFILTER_NRM_SERVICETAG_RESTORE 		"MailFilterRestore"
#define MAILFILTER_NRM_SERVICETAG_RESTORE_LEN 17

//extern UINT32 MF_NLM_RM_HttpHandler_Restore(HINTERNET hndl,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits);

#ifdef __NOVELL_LIBC__
	#include <synch.h> 
	static cond_t condMainThread;
	static void* rTag;
#else
	static LONG rTag;
#endif

static struct TOCregistration menuEntry;
static char *MF_NLM_RM_rTagName		=	"MailFilterRM";


int MF_NRM_InitComplete = 0;
bool MF_NRM_WantRestore =	FALSE;
//char	MF_ProductName[]	= "MailFilter/ax 1.5 Web Restore [NLM]";



/** Remote Manager Function Calls **/

UINT32 MF_NLM_RM_HttpHandler_Restore(
		/* I- httpHndl		*/	HINTERNET hndl,
		/* I- pExtraInfo	*/	void *pExtraInfo,
		/* I- szExtraInfo	*/	UINT32 szExtraInfo,
		/* I- InfoBits		*/	UINT32 InformationBits
		)
{
}

UINT32 MF_NLM_RM_HttpHandler(
		/* I- httpHndl		*/	HINTERNET hndl,
		/* I- pExtraInfo	*/	void *pExtraInfo,
		/* I- szExtraInfo	*/	UINT32 szExtraInfo,
		/* I- InfoBits		*/	UINT32 InformationBits
		)
{
//	int handlerThreadGroupID;
	UINT32 rCode = 0;
	long statRet = -1;
	long (*MailFilter_GetStats)(int statID, int statSubID);
	
	if (MF_NRM_InitComplete != 1)
		return 0;

	++MFT_NLM_ThreadCount;

//	handlerThreadGroupID = GetThreadGroupID();
	
	/*------------------------------------------------------------------------
	**	I did this so the compiler would not generate warnings that the
	**	variables were never used.
	*/
	pExtraInfo = pExtraInfo;
	szExtraInfo = szExtraInfo;
	
	/*------------------------------------------------------------------------
	**	Check to see if the process is being initialized or deinitialized.  If
	**	it is, jump to ERR_END and return out of this function.
	*/
	if(CONTROL_DEINITIALIZATION_BIT & InformationBits)
		goto ERR_END;
	if(CONTROL_INITIALIZATION_BIT & InformationBits)
		goto ERR_END;
		
		
	DL_HttpSendSuccessfulResponse(hndl, DL_HttpReturnString(HTTP_CONTENT_TYPE_HTML));
	
	
	// Output Page ...	
	DL_BuildAndSendHeader( hndl, "MailFilter Status", "MailFilter Status", /* REFRESH */ 1, /* RFRSHTIMER */30, /* FLAGS */ 0, NULL, "", "");


	/* MailFilter Server/NLM */
	DL_HttpSendData(hndl,"  <b>MailFilter Server</b><br>");
	
	MailFilter_GetStats = NULL; //(long(*)(int,int))ImportSymbol( (int)GetNLMHandle() , "MailFilter_API_GetStats" );

	if (MailFilter_GetStats != NULL)
	{
		statRet = MailFilter_GetStats(0,0) / 60;
//		HttpSendDataSprintf(hndl," &nbsp;&nbsp; Run-Time: %d h %d min.<br>\n" , (statRet/60) , (statRet) - (((long)(statRet/60))*60) );
		DL_HttpSendData(hndl," &nbsp;&nbsp; <br>\n");
		DL_HttpSendData(hndl,"  <b>MailFilter Mail Delivery</b><br>\n");
//		HttpSendDataSprintf(hndl," &nbsp;&nbsp; Mails Input Total / Failed: %d / %d<br>\n",MailFilter_GetStats(1,1),MailFilter_GetStats(1,2));
//		HttpSendDataSprintf(hndl," &nbsp;&nbsp; Mails Output Total / Failed: %d / %d<br>\n",MailFilter_GetStats(2,1),MailFilter_GetStats(2,2));

		//UnimportSymbol( (int)GetNLMHandle() , "MailFilter_API_GetStats" );


	} else {
		DL_HttpSendData(hndl," &nbsp;&nbsp; MailFilter/ax Server/NLM is not loaded, not loaded in the OS space, or is incompatible with this Version of MailFilter/NRM.<br>\n");
	}
	
	DL_HttpSendData(hndl,"    <br>\n");
	
/*	HttpSendDataSprintf(hndl,"  <b>MailFilter/NRM</b><br>\n");
	// Set Thread Group so that we can read our own LifeTimer, not the portal one's ...
	SetThreadGroupID(NLM_mainThreadGroupID);
	// Read LifeTimer ...
	statRet = (long)(((clock() / 100) / 60));
	// Restore Thread Group
	SetThreadGroupID(handlerThreadGroupID);

	HttpSendDataSprintf(hndl," &nbsp;&nbsp; Run-Time: %d h %d min.<br>\n", (statRet/60) , (statRet) - (((long)(statRet/60))*60) );
	HttpSendDataSprintf(hndl,"    <br>\n");

*/
	/* Config Info */
	DL_HttpSendData(hndl,"  <b>Configuration:</b><br>");

//	HttpSendDataSprintf(hndl," &nbsp;&nbsp; Configuration File: %s<br>\n",MF_GlobalConfiguration.config_file);
	DL_HttpSendData(hndl,"    <br>\n");
	
//	HttpSendDataSprintf(hndl," &nbsp;&nbsp; MailFilter/ax Home: %s<br>\n",MF_GlobalConfiguration.MFLTRoot);
//	HttpSendDataSprintf(hndl," &nbsp;&nbsp; GWIA Home: %s<br>\n",MF_GlobalConfiguration.GWIARoot);
	
	
	// end
	DL_HttpSendData(hndl,"    <br>\n");
	DL_BuildAndSendFooter(hndl);
	
	DL_HttpEndDataResponse(hndl);
		
ERR_END:

	--MFT_NLM_ThreadCount;
	return(rCode);
}


int MF_NLM_RM_Init()
{
	BOOL bCode;
	UINT32 rCode = 0;


	/*------------------------------------------------------------------------
	**	Register with Httpstk on the Server
	*/
#ifndef __NOVELL_LIBC__
	rTag = AllocateResourceTag(
		/* I- NLMHandle 		*/	GetNLMHandle(),
		/* I- descriptionString	*/	(unsigned char*)MF_NLM_RM_rTagName,
		/* I- resourceType		*/	NetWareHttpStackResourceSignature
		);
#else
	rTag = AllocateResourceTag(
		/* I- NLMHandle 		*/	getnlmhandle(),
		/* I- descriptionString	*/	(const char*)MF_NLM_RM_rTagName,
		/* I- resourceType		*/	NetWareHttpStackResourceSignature
		);
#endif
	
	/*------------------------------------------------------------------------
	**	Set up the Table of Contents (TOC) structure before registering the
	**	second type of Service Method (ServiceMethodEx)
	*/
	memset(&menuEntry, 0, sizeof(struct TOCregistration));
	strcpy(menuEntry.TOCHeadingName, MAILFILTER_NRM_HEADER); 
	menuEntry.TOCHeadingNumber = 9;
	
	bCode = DL_RegisterServiceMethodEx(
		/* I- pzServiceName		*/	"Status",
		/* I- pServiceTag 		*/	MAILFILTER_NRM_SERVICETAG_STATUS,
		/* I- szServiceTagLen	*/	MAILFILTER_NRM_SERVICETAG_STATUS_LEN,
		/* I- TOC struct		*/	&menuEntry,
		/* I- requestedRights	*/	(INFO_LOGGED_IN_BIT/* && INFO_CONSOLE_OPERATOR_PRIVILEGES_BIT*/),
		/* I- Reserved			*/	NULL,
		/* I- pServMethdCallBk	*/	MF_NLM_RM_HttpHandler,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);

	if (MF_NRM_WantRestore)
	bCode = DL_RegisterServiceMethodEx(
		/* I- pzServiceName		*/	"Mail Restore",
		/* I- pServiceTag 		*/	MAILFILTER_NRM_SERVICETAG_RESTORE,
		/* I- szServiceTagLen	*/	MAILFILTER_NRM_SERVICETAG_RESTORE_LEN,
		/* I- TOC struct		*/	&menuEntry,
		/* I- requestedRights	*/	(INFO_LOGGED_IN_BIT/* && INFO_CONSOLE_OPERATOR_PRIVILEGES_BIT*/),
		/* I- Reserved			*/	NULL,
		/* I- pServMethdCallBk	*/	MF_NLM_RM_HttpHandler_Restore,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);
		
	MF_NRM_InitComplete = 1;
	return 1;
}

void MF_NLM_RM_DeInit()
{

	if (MF_NRM_InitComplete != 1)
		return;

	consoleprintf("MAILFILTER: Terminating NRM module.\n");

	MF_NRM_InitComplete = 2; 	/* deinit... */

	BOOL bCode;
	UINT32 rCode = 0;
	
	/*------------------------------------------------------------------------
	**	Deregister both service methods before the NLM unloads, in the 
	**	opposite order that they were registered.
	*/
	if (MF_NRM_WantRestore)
	bCode = DL_DeRegisterServiceMethod(
		/* I- pzServiceName		*/	"Mail Restore",
		/* I- pServiceTag 		*/	MAILFILTER_NRM_SERVICETAG_RESTORE,
		/* I- szServiceTagLen	*/	MAILFILTER_NRM_SERVICETAG_RESTORE_LEN,
		/* I- pServMethdCallBk	*/	MF_NLM_RM_HttpHandler_Restore,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);
	

	bCode = DL_DeRegisterServiceMethod(
		/* I- pzServiceName		*/	"Status",
		/* I- pServiceTag 		*/	MAILFILTER_NRM_SERVICETAG_STATUS,
		/* I- szServiceTagLen	*/	MAILFILTER_NRM_SERVICETAG_STATUS_LEN,
		/* I- pServMethdCallBk	*/	MF_NLM_RM_HttpHandler,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);
	
	MF_NRM_InitComplete = 0;	/* not init'ed */
}


void MailFilter_NRM_sigterm()
{
#ifdef __NOVELL_LIBC__
	cond_signal(&condMainThread);
	mutex_t mtx;
	mutex_init(&mtx, USYNC_THREAD, NULL);
	mutex_lock(&mtx);
	cond_wait(&condMainThread,&mtx);
#else
	MF_NLM_RM_DeInit();
	DLDeSetupNRM();
#endif

	return;
}

int MailFilter_Main_RunAppNRM()
{

	if (!DLSetupNRM())
	{
		printf("Could not import symbols. aborting startup.\n");
		abort();
	}

	if (!MF_NLM_RM_Init())
	{
		printf ("MailFilter/NRM Startup Failed.\n");
		abort();
	}
	// Pretend that we've already exited.
	--MFT_NLM_ThreadCount;

#ifndef __NOVELL_LIBC__
	ExitThread(TSR_THREAD, 0);
#else
	
	if (cond_init ( &condMainThread , USYNC_THREAD, NULL ) != 0)
	{
		printf("Could not initialize a mutex. aborting startup\n");
	}
	else 
	{
	
		mutex_t mtx;
		mutex_init(&mtx, USYNC_THREAD, NULL);
		mutex_lock(&mtx);
		cond_wait(&condMainThread,&mtx);
	}
   	
	
	MF_NLM_RM_DeInit();
	DLDeSetupNRM();

	cond_signal(&condMainThread);
	cond_destroy(&condMainThread);
#endif
	
	return 0;	
}
