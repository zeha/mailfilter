/****************************************************************************
**	File:	MFNRM.C
**
**	Desc:	MailFilter Remote Manager Plug-In
**
**  (C) Copyright 2001-2002 Christian Hofstädtler. All Rights Reserved.
**							
*/

#include "mfnrm.h"
#include <nwfileio.h> 

extern int MF_ConfigRead();
extern void MF_ConfigFree();
extern UINT32 MF_NLM_RM_HttpHandler_Restore(HINTERNET hndl,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits);
extern int MF_NRM_ParseCommandLine( int argc, char **argv );

LONG rTag;
struct TOCregistration menuEntry;
char *MF_NLM_RM_rTagName		=	"MailFilterRM";

int	NLM_exiting			=	NLM_FALSE;
int NLM_threadCnt		=	0;
int NLM_mainThreadID 	=	(-1);
int NLM_mainThreadGroupID = (-1);
int MF_NRM_InitComplete = 0;
bool MF_NRM_WantRestore =	FALSE;
char	MF_ProductName[]	= "MailFilter/ax 1.5 Web Restore [NLM]";

#include "../../../Config/MFConfig.h"
#include <ix.h>


/** Remote Manager Function Calls **/

UINT32 MF_NLM_RM_HttpHandler(
		/* I- httpHndl		*/	HINTERNET hndl,
		/* I- pExtraInfo	*/	void *pExtraInfo,
		/* I- szExtraInfo	*/	UINT32 szExtraInfo,
		/* I- InfoBits		*/	UINT32 InformationBits
		)
	{
	int handlerThreadGroupID;
	UINT32 rCode = 0;
	long statRet = -1;
	long (*MailFilter_GetStats)(int statID, int statSubID);
	
	if (MF_NRM_InitComplete != 1)
		return 0;

	++NLM_threadCnt;

	handlerThreadGroupID = GetThreadGroupID();
	
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
		
		
	HttpSendSuccessfulResponse(hndl, HttpReturnString(HTTP_CONTENT_TYPE_HTML));
	
	
	// Output Page ...	
	BuildAndSendHeader( hndl, "MailFilter/ax Status", "MailFilter/ax Status", /* REFRESH */ 1, /* RFRSHTIMER */30, /* FLAGS */ 0, NULL, "", "");


	/* MailFilter Server/NLM */
	HttpSendDataSprintf(hndl,"  <b>MailFilter/ax Server/NLM</b><br>");
	
	MailFilter_GetStats = (long(*)(int,int))ImportSymbol( (int)GetNLMHandle() , "MailFilter_API_GetStats" );

	if (MailFilter_GetStats != NULL)
	{
		statRet = MailFilter_GetStats(0,0) / 60;
		HttpSendDataSprintf(hndl," &nbsp;&nbsp; Run-Time: %d h %d min.<br>\n" , (statRet/60) , (statRet) - (((long)(statRet/60))*60) );
		HttpSendDataSprintf(hndl," &nbsp;&nbsp; <br>\n");
		HttpSendDataSprintf(hndl,"  <b>MailFilter Mail Delivery</b><br>\n");
		HttpSendDataSprintf(hndl," &nbsp;&nbsp; Mails Input Total / Failed: %d / %d<br>\n",MailFilter_GetStats(1,1),MailFilter_GetStats(1,2));
		HttpSendDataSprintf(hndl," &nbsp;&nbsp; Mails Output Total / Failed: %d / %d<br>\n",MailFilter_GetStats(2,1),MailFilter_GetStats(2,2));

		UnimportSymbol( (int)GetNLMHandle() , "MailFilter_API_GetStats" );


	} else {
		HttpSendDataSprintf(hndl," &nbsp;&nbsp; MailFilter/ax Server/NLM is not loaded, not loaded in the OS space, or is incompatible with this Version of MailFilter/NRM.<br>\n");
	}
	
	HttpSendDataSprintf(hndl,"    <br>\n");
	
	/* MailFilter/NRM */
	HttpSendDataSprintf(hndl,"  <b>MailFilter/NRM</b><br>\n");
	// Set Thread Group so that we can read our own LifeTimer, not the portal one's ...
	SetThreadGroupID(NLM_mainThreadGroupID);
	// Read LifeTimer ...
	statRet = (long)(((clock() / 100) / 60));
	// Restore Thread Group
	SetThreadGroupID(handlerThreadGroupID);

	HttpSendDataSprintf(hndl," &nbsp;&nbsp; Run-Time: %d h %d min.<br>\n", (statRet/60) , (statRet) - (((long)(statRet/60))*60) );
	HttpSendDataSprintf(hndl,"    <br>\n");


	/* Config Info */
	HttpSendDataSprintf(hndl,"  <b>Configuration:</b><br>");

	HttpSendDataSprintf(hndl," &nbsp;&nbsp; Configuration File: %s<br>\n",MFC_ConfigFile);
	HttpSendDataSprintf(hndl,"    <br>\n");
	
	HttpSendDataSprintf(hndl," &nbsp;&nbsp; MailFilter/ax Home: %s<br>\n",MFC_MFLT_ROOT);
	HttpSendDataSprintf(hndl," &nbsp;&nbsp; GWIA Home: %s<br>\n",MFC_GWIA_ROOT);
	
	
	// end
	HttpSendDataSprintf(hndl,"    <br>\n");
	BuildAndSendFooter(hndl);
	
	HttpEndDataResponse(hndl);
		
ERR_END:

	--NLM_threadCnt;
	return(rCode);
}


int MF_NLM_RM_Init()
{
	BOOL bCode;
	UINT32 rCode = 0;

	if (!MF_ConfigRead())
	{
		return NLM_FALSE;
	}

	/*------------------------------------------------------------------------
	**	Register with Httpstk on the Server
	*/
	rTag = AllocateResourceTag(
		/* I- NLMHandle 		*/	GetNLMHandle(),
		/* I- descriptionString	*/	(unsigned char*)MF_NLM_RM_rTagName,
		/* I- resourceType		*/	NetWareHttpStackResourceSignature
		);
	
	/*------------------------------------------------------------------------
	**	Set up the Table of Contents (TOC) structure before registering the
	**	second type of Service Method (ServiceMethodEx)
	*/
	memset(&menuEntry, 0, sizeof(struct TOCregistration));
	strcpy(menuEntry.TOCHeadingName, MAILFILTER_NRM_HEADER); 
	menuEntry.TOCHeadingNumber = 9;
	
	bCode = RegisterServiceMethodEx(
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
	bCode = RegisterServiceMethodEx(
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
	return NLM_TRUE;
}

void MF_NLM_RM_DeInit()
{

	if (MF_NRM_InitComplete != 1)
		return;

	MF_NRM_InitComplete = 2; 	/* deinit... */

	BOOL bCode;
	UINT32 rCode = 0;
	
	/*------------------------------------------------------------------------
	**	Deregister both service methods before the NLM unloads, in the 
	**	opposite order that they were registered.
	*/
	if (MF_NRM_WantRestore)
	bCode = DeRegisterServiceMethod(
		/* I- pzServiceName		*/	"Mail Restore",
		/* I- pServiceTag 		*/	MAILFILTER_NRM_SERVICETAG_RESTORE,
		/* I- szServiceTagLen	*/	MAILFILTER_NRM_SERVICETAG_RESTORE_LEN,
		/* I- pServMethdCallBk	*/	MF_NLM_RM_HttpHandler_Restore,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);
	

	bCode = DeRegisterServiceMethod(
		/* I- pzServiceName		*/	"Status",
		/* I- pServiceTag 		*/	MAILFILTER_NRM_SERVICETAG_STATUS,
		/* I- szServiceTagLen	*/	MAILFILTER_NRM_SERVICETAG_STATUS_LEN,
		/* I- pServMethdCallBk	*/	MF_NLM_RM_HttpHandler,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);
	
	MF_ConfigFree();
	
	MF_NRM_InitComplete = 0;	/* not init'ed */
}

/** NLM Function Calls **/

void NLM_SignalHandler(int sig)
	{
//	int handlerThreadGroupID;
	switch(sig)
		{
		case SIGTERM:
			
//			handlerThreadGroupID = GetThreadGroupID();
//			SetThreadGroupID(NLM_mainThreadGroupID);
			
			NLM_exiting = NLM_TRUE;

//			ResumeThread(NLM_mainThreadID);
			//ungetch(27);
			
			MF_NLM_RM_DeInit();

//			SetThreadGroupID(handlerThreadGroupID);
/*			while (NLM_threadCnt > 0)
				ThreadSwitchWithDelay();
*/				
			break;
		}
	return;
	}


//extern "C" { extern NWDSCCODE NWCallsInit( nptr reserved1, nptr reserved2); }
void main (int argc, char *argv[])
{
	++NLM_threadCnt;
	NLM_mainThreadID = GetThreadID();
	NLM_mainThreadGroupID = GetThreadGroupID();
	NWCCODE 			dscCode;

	MF_NRM_ParseCommandLine(argc,argv);

	UnAugmentAsterisk(TRUE);

	dscCode=NWCallsInit(NULL,NULL);
	if(dscCode)
	{
		printf ("MailFilter/NRM: Unable to initialize NWCalls!\n");
		abort();
	}

	if (!MF_NLM_RM_Init())
	{
		printf ("MailFilter/NRM Startup Failed.\n");
		abort();
	}

	signal(SIGTERM,NLM_SignalHandler);

	/*------------------------------------------------------------------------
	**	Suspend the main thread of this NLM.  It will only be resumed when the
	**	NLM is unloaded from the server console.
	*/
	// Pretend that we've already exited.
	--NLM_threadCnt;

	//while (!NLM_exiting)
	//	ThreadSwitchWithDelay();
	
	ExitThread(TSR_THREAD, 0);

	return;	
}
