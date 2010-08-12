/*****************************************************************************
**	File: HTTPSTKDemo.c
**  
**	Desc: 	This is a demo HTTPSTK application NLM.  When loaded on a NetWare
**			5 server this NLM registers 2 user defined services with the 
**			HTTPSTK.NLM that must already be running on the Server. "Demo1" is
**			the Service Tag of the first method, and "Demo2" is the second.
**			A user from any browser can type the name or IP address of the 
**			server followed by :8008/Demo1 to invoke the first service.  For
**			example:  http://xxx.xxx.xxx.xxx:8008/Demo1.  Demo2 is registered 
**			as an extension to the standard Table of Contents (TOC) and 
**			appears on the left side of the browser as the last TOC entry.
**	
**	Programmers:
**
**		Who	Firm
**		---	------------------------------------------------------------------
**		AES	Novell, Developer Support
**
**	History:
**
**		When		Who	What
**		-----------	---	------------------------------------------------------
**		04/06/01	AES	First attempt.
*/


/*****************************************************************************
**	compiler setup 
*/
	/*------------------------------------------------------------------------
	**	ANSI-POSIX
	*/
	#include <stdio.h>
	#include <signal.h>
	
	/*------------------------------------------------------------------------
	**	NetWare specific compiler setup
	*/
	#include <nwtypes.h>
	#include <httpexp.h>
	#include <nwadv.h>
	#include <nwthread.h>
	
	/*------------------------------------------------------------------------
	**	Define Macros
	*/
	/*** Boolean Values ***/
	#define NLM_FALSE 0
	#define NLM_TRUE (-1)
	
	/*** Return Codes ***/
	#define NLM_SUCCESS 0
	
/*****************************************************************************
**	Global Storage
*/
int	NLM_exiting			=	NLM_FALSE;
int NLM_threadCnt		=	0;
int NLM_mainThreadID 	=	(-1);
char *NLM_rTagName		=	"HTTPSTKDemo";


/*****************************************************************************
**	Http Request Handler
*/
UINT32 NLM_HttpHandler(
		/* I- httpHndl		*/	HINTERNET hndl,
		/* I- pExtraInfo	*/	void *pExtraInfo,
		/* I- szExtraInfo	*/	UINT32 szExtraInfo,
		/* I- InfoBits		*/	UINT32 InformationBits
		)
	{
	UINT32 rCode = NLM_SUCCESS;
	
	++NLM_threadCnt;
	
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
	
	HttpSendDataSprintf(hndl,"<HTML>\n");
	HttpSendDataSprintf(hndl," <HEAD>\n");
	HttpSendDataSprintf(hndl,"  <TITLE>HTTPSTKDemonstration</TITLE>\n");
	HttpSendDataSprintf(hndl," </HEAD>\n");
	HttpSendDataSprintf(hndl,"\n");
	HttpSendDataSprintf(hndl," <BODY>\n");
	HttpSendDataSprintf(hndl,"  <H1>HELLO WORLD</H1>\n");
	HttpSendDataSprintf(hndl," </BODY>\n");
	HttpSendDataSprintf(hndl,"</HTML>\n");

	HttpEndDataResponse(hndl);
		
ERR_END:

	--NLM_threadCnt;
	return(rCode);
	}

/*****************************************************************************
**	Program Signal Handling
*/
void NLM_SignalHandler(int sig)
	{
	switch(sig)
		{
		case SIGTERM:
			NLM_exiting = NLM_TRUE;
			ResumeThread(NLM_mainThreadID);
			while (NLM_threadCnt != 0)
				ThreadSwitchWithDelay();
			break;
		}
	return;
	}
	
/*****************************************************************************
**	Program Start
*/
void main (void)
	{
	BOOL bCode;
	UINT32 rCode = NLM_SUCCESS;
	LONG rTag;
	struct TOCregistration menuEntry;

	++NLM_threadCnt;
	NLM_mainThreadID = GetThreadID();
	
	signal(SIGTERM,NLM_SignalHandler);

	printf ("The HTTPSTKDemonstration NLM is running on the server\n\n");

	/*------------------------------------------------------------------------
	**	Register with Httpstk on the Server
	*/
	rTag = AllocateResourceTag(
		/* I- NLMHandle 		*/	GetNLMHandle(),
		/* I- descriptionString	*/	NLM_rTagName,
		/* I- resourceType		*/	NetWareHttpStackResourceSignature
		);
		
	/*------------------------------------------------------------------------
	**	Register the first type of Service Method (Standard ServiceMethod)
	*/
	bCode = RegisterServiceMethod(
		/* I- pzServiceName		*/	"HTTPSTKDemo1",
		/* I- pServiceTag 		*/	"Demo1",
		/* I- szServiceTagLen	*/	5,
		/* I- pServMethdCallBk	*/	NLM_HttpHandler,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);

	/*------------------------------------------------------------------------
	**	Set up the Table of Contents (TOC) structure before registering the
	**	second type of Service Method (ServiceMethodEx)
	*/
	memset(&menuEntry, 0, sizeof(struct TOCregistration));
	strcpy(menuEntry.TOCHeadingName, "HTTPSTK Demo");  /* not to exceed 35 chars*/
	menuEntry.TOCHeadingNumber = 0;

	bCode = RegisterServiceMethodEx(
		/* I- pzServiceName		*/	"HTTPSTKDemo2",
		/* I- pServiceTag 		*/	"Demo2",
		/* I- szServiceTagLen	*/	5,
		/* I- TOC struct		*/	&menuEntry,
		/* I- requestedRights	*/	/* INFO_LOGGED_IN_BIT*/ 0,
		/* I- Reserved			*/	NULL,
		/* I- pServMethdCallBk	*/	NLM_HttpHandler,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);  


	/*------------------------------------------------------------------------
	**	Suspend the main thread of this NLM.  It will only be resumed when the
	**	NLM is unloaded from the server console.
	*/
	SuspendThread(NLM_mainThreadID);
	NLM_exiting = NLM_TRUE;

	
	/*------------------------------------------------------------------------
	**	Deregister both service methods before the NLM unloads, in the 
	**	opposite order that they were registered.
	*/
	bCode = DeRegisterServiceMethod(
		/* I- pzServiceName		*/	"HTTPSTKDemo2",
		/* I- pServiceTag 		*/	"Demo2",
		/* I- szServiceTagLen	*/	5,
		/* I- pServMethdCallBk	*/	NLM_HttpHandler,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);
	
	bCode = DeRegisterServiceMethod(
		/* I- pzServiceName		*/	"HTTPSTKDemo1",
		/* I- pServiceTag 		*/	"Demo1",
		/* I- szServiceTagLen	*/	5,
		/* I- pServMethdCallBk	*/	NLM_HttpHandler,
		/* I- pRTag				*/	(void*)rTag,
		/* I- lpFailureRsnCode	*/	&rCode
		);
		
	--NLM_threadCnt;
	return;
	}
