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
#else
	static int MF_NRM_ThreadID = 0;
#endif

static rtag_t rTag;

static struct TOCregistration menuEntry;
static char *MF_NLM_RM_rTagName		=	"MailFilterRM";


int MF_NRM_InitComplete = 0;
//char	MF_ProductName[]	= "MailFilter 1.5 Web Restore [NLM]";


static bool outputMailList(HINTERNET hndl, int startPage)
{
	int entriesPerPage = 20;

	DL_HttpSendData(hndl, "<table cellpadding=1 cellspacing=0 width=800>\n"
		"<tr><th width=100>Direction</th><th width=100>Date</th><th width=100>Size</th><th>Original Filename</th></tr>\n" );
	
	/*------------------------------------------------------------------------
	** Look for dropped mails and put them in the list.
	*/

	std::string szHttpPath;
	std::string szScanPath;

	szScanPath = MF_GlobalConfiguration.MFLTRoot + "\\MFPROB\\DROP\\";
	szHttpPath = szScanPath;
	
	while (std::string::size_type iR = szHttpPath.find(":"))
		szHttpPath.replace(iR, 1, "/");
	while (std::string::size_type iR = szHttpPath.find("\\"))
		szHttpPath.replace(iR, 1, "/");


	iXDir dir(szScanPath.c_str());
	dir.SkipDotFiles = true;
	
	int thisEntry = 0;
	bool bHaveMoreEntries = false;
	int iDirection;
	
	char szDate[16];
	std::string szLink;
	std::string szListLine;
	time_t fileTime;
//	tm fileTm;
	std::string szFileOut;
	
	while ( dir.ReadNextEntry() )
	{
		thisEntry++;
		
		if (thisEntry > (startPage*entriesPerPage))
		{
			if (thisEntry > ((startPage+1)*entriesPerPage))
			{
				bHaveMoreEntries = true;
				break;
			}
			
			const char* e = dir.GetCurrentEntryName();
			iDirection = e[0] == 'S' ? 1 : 2;
			if (iDirection == 1)
				szFileOut = MF_GlobalConfiguration.MFLTRoot + "\\SEND\\" + e;
			else
				szFileOut = MF_GlobalConfiguration.GWIARoot + "\\RECEIVE\\" + e;
			
			fileTime = dir.GetCurrentEntryModificationTime();
			strcpy(szDate,"");
//			fileTm = localtime(fileTime);
//			sprintf(szDate,"%04d/%02d/%02d", fileTime)+1980, GET_MONTH_FROM_DATE(fileTime), GET_DAY_FROM_DATE(fileTime));
			
			DL_HttpSendData(hndl,strprintf(
					"<tr bgcolor=\"#efeee9\" ><td>%s</td><td>%s</td><td>%6d kB</td><td>%s</td></tr>\n",
					(iDirection == 1) ? "Outgoing" : " Incoming", 
					szDate, 
					((dir.GetCurrentEntrySize())/1024)+1, 
					szFileOut)
					);
					
			DL_HttpSendData(hndl,strprintf(
					"<tr bgcolor=\"#ffffff\"><td colspan=2></td><td colspan=2>%s &nbsp; "
					"<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/RESTORE/%s\">Restore</a> &nbsp; "
					"<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/RECHECK/%s\">Recheck</a> <br><br>"
					"</td></tr>\n\n", szLink, dir.GetCurrentEntryName(),dir.GetCurrentEntryName()
				));
			
		}
		
	}

	DL_HttpSendData(hndl,"\n<tr>\n");
	DL_HttpSendData(hndl,"\n<th align=left colspan=2> \n");
	DL_HttpSendData(hndl,strprintf("\n<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/0\">|&lt; first page</a> \n",startPage-1));
	if (startPage>0)
	{
		DL_HttpSendData(hndl,strprintf("\n<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/%d\">&lt;&lt; prev. page</a> \n",startPage-1));
		
	}
	DL_HttpSendData(hndl,"\n</th><th align=right colspan=2> \n");
	if (bHaveMoreEntries==true)
	{
		DL_HttpSendData(hndl,strprintf("\n<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/%d\">next page &gt;&gt;</a> \n",startPage+1));
		
	}
	DL_HttpSendData(hndl,strprintf("\n <a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/%i\">last page &gt;|</a> \n",thisEntry/entriesPerPage));
	DL_HttpSendData(hndl,strprintf(" </th></tr>\n"));

	DL_HttpSendData(hndl,strprintf("</table>    <br>\n"));

	return true;
}


/** Remote Manager Function Calls **/

UINT32 MF_NLM_RM_HttpHandler_Restore(
		/* I- httpHndl		*/	HINTERNET hndl,
		/* I- pExtraInfo	*/	void *pExtraInfo,
		/* I- szExtraInfo	*/	UINT32 szExtraInfo,
		/* I- InfoBits		*/	UINT32 InformationBits
		)
{
#pragma unused(pExtraInfo,szExtraInfo)
	UINT32 rCode = 0;

	if (MF_NRM_InitComplete != 1)
		return 0;

	++MFT_NLM_ThreadCount;

#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)
	int handlerThreadGroupID = GetThreadGroupID();
	SetThreadGroupID(MF_NRM_ThreadID);
#endif


	if(CONTROL_DEINITIALIZATION_BIT & InformationBits)
		goto ERR_END;
	if(CONTROL_INITIALIZATION_BIT & InformationBits)
		goto ERR_END;

	if ( (MF_GlobalConfiguration.MFLTRoot == "") || (MF_GlobalConfiguration.GWIARoot == "") )
	{
		DL_HttpSendErrorResponse(hndl, HTTP_INTERNAL_SERVER_ERROR);
		rCode = HTTP_INTERNAL_SERVER_ERROR;
		goto ERR_END;
	}
	
	{
		// check request method
		long methodType = 0;

		if (DL_HttpReturnRequestMethod(hndl, &methodType) != TRUE)
		{
			//hndl is invalid, return error
			DL_HttpSendErrorResponse(hndl, HTTP_INTERNAL_SERVER_ERROR);
			rCode = HTTP_INTERNAL_SERVER_ERROR;
			goto ERR_END;
		}		

		if (methodType != HTTP_REQUEST_METHOD_GET)
		{
			//request is not a get, return error
			DL_HttpSendErrorResponse(hndl, HTTP_STATUS_NOT_SUPPORTED);
			rCode = HTTP_STATUS_NOT_SUPPORTED;
			goto ERR_END;
		}
	}
	
	{
		// path buffer stuff
		
	}
	
	{
		// page output
		DL_HttpSendSuccessfulResponse(hndl, DL_HttpReturnString(HTTP_CONTENT_TYPE_HTML));

		// Output Page Header ...	
		DL_BuildAndSendHeader( hndl, "Mail Restore", "Mail Restore", /* REFRESH */ 0, /* RFRSHTIMER */0, /* FLAGS */ 0, NULL, "", "");


		DL_HttpSendData(hndl," <b><span style=\"color: red;\">WARNING: This tool is a potential security risk. If restored mails are not examined carefully, this could lead to viruses in the system!</span></b><br><br>\n");
		
		outputMailList(hndl,0);

	}

	{
		// end
		DL_HttpSendData(hndl,"    <br>\n");
		DL_BuildAndSendFooter(hndl);
		
		DL_HttpEndDataResponse(hndl);
	}
			
ERR_END:

#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)
	SetThreadGroupID(handlerThreadGroupID);	
#endif
	
	--MFT_NLM_ThreadCount;
	
	return(rCode);
}

static int MF_NRM_RestoreFile(const char* szInFile, const char* szOutFile, char* szType)
{
	FILE* inputFile;
	FILE* outputFile;
	char* szScanBuffer	   = NULL;

	bool bWroteXRestore = false;

	inputFile = fopen(szInFile,"rb");
	if (inputFile == NULL)
		return -1;

	outputFile = fopen(szOutFile,"wb");
	if (outputFile == NULL)
	{	fclose(inputFile);
		return -1;
	}
	
	szScanBuffer =	(char*)malloc(2002);
	if (szScanBuffer == NULL)	{ fclose(inputFile); fclose(outputFile);	return -1; }
	memset ( szScanBuffer	, 0 , 2000 );

#define _MF_NRM_RestoreFile_WriteRestoreHeader()	\
									if (!bWroteXRestore) { \
										time_t currentTime = time(NULL); \
										strftime  (szScanBuffer, 1999, "%a, %d %b %Y %T UTC", gmtime(&currentTime)); \
										fprintf(outputFile,"X-Restore: (%s) %s at %s\r\n",MF_ProductName,szType,szScanBuffer); \
										bWroteXRestore = true; \
									}

	while(!feof(inputFile) )
	{
		szScanBuffer[0]=0;
		fgets(szScanBuffer,2000,inputFile);

		switch(szScanBuffer[0])
		{
			case 'x':
			case 'X':
				{ 	_MF_NRM_RestoreFile_WriteRestoreHeader(); break; }
			case 's':
			case 'S':
				{ 	_MF_NRM_RestoreFile_WriteRestoreHeader(); break; }
			case 't':
			case 'T':
				{ 	_MF_NRM_RestoreFile_WriteRestoreHeader(); break; }
		}
		fwrite(szScanBuffer,sizeof(char),strlen(szScanBuffer),outputFile);
	}
	
	fclose(inputFile);
	fclose(outputFile);
	free(szScanBuffer);
	unlink(szInFile);
	
	return 0;
}

static int stripOffHtml(char* str)
{
	char* p = str;
	int cnt = 0;
	while ( (p = strchr(p,'<')) != NULL )
	{
	    *p = '(';
	    cnt++;
	}

	p = str;
	while ( (p = strchr(p,'>')) != NULL )
	{
	    *p = ')';
	    cnt++;
	}
	
	return cnt;
}


UINT32 MF_NLM_RM_HttpHandler(
		/* I- httpHndl		*/	HINTERNET hndl,
		/* I- pExtraInfo	*/	void *pExtraInfo,
		/* I- szExtraInfo	*/	UINT32 szExtraInfo,
		/* I- InfoBits		*/	UINT32 InformationBits
		)
{
#pragma unused(pExtraInfo,szExtraInfo)
	UINT32 rCode = 0;
	long statRet = -1;
	long (*MailFilter_GetStats)(int statID, int statSubID);
	
	if (MF_NRM_InitComplete != 1)
		return 0;

	++MFT_NLM_ThreadCount;

#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)
	int handlerThreadGroupID = GetThreadGroupID();
	SetThreadGroupID(MF_NRM_ThreadID);
#endif
	
	if(CONTROL_DEINITIALIZATION_BIT & InformationBits)
		goto ERR_END;
	if(CONTROL_INITIALIZATION_BIT & InformationBits)
		goto ERR_END;
		
		
	DL_HttpSendSuccessfulResponse(hndl, DL_HttpReturnString(HTTP_CONTENT_TYPE_HTML));

	// Output Page ...	
	DL_BuildAndSendHeader( hndl, "MailFilter Status", "MailFilter Status", /* REFRESH */ 1, /* RFRSHTIMER */30, /* FLAGS */ 0, NULL, "", "");


	/* MailFilter Server/NLM */
	DL_HttpSendData(hndl,"  <b>MailFilter Server</b><br>");
	

	if (MF_GlobalConfiguration.NRMInitialized == true)
	{
/*		std::string sztemp;
		char buffer [sizeof(long)*8+1];
		
		sztemp = "  <b>MailFilter Mail Delivery</b><br>\n";
		sztemp += " &nbsp;&nbsp; Mails Input Total / Failed: ";
		ultoa(MFS_MF_MailsInputTotal,buffer,10); sztemp += buffer;
		sztemp += " / ";
		ultoa(MFS_MF_MailsInputFailed,buffer,10); sztemp += buffer;
		sztemp += "<br>\n";
*/
//		DL_HttpSendData(hndl,strprintf(" &nbsp;&nbsp; Run-Time: %d h %d min.<br>\n" , (statRet/60) , (statRet) - (((long)(statRet/60))*60)));
		DL_HttpSendData(hndl,strprintf(" &nbsp;&nbsp; Mails Input Total / Failed: %d / %d<br>\n",MFS_MF_MailsInputTotal,MFS_MF_MailsInputFailed));
		DL_HttpSendData(hndl,strprintf(" &nbsp;&nbsp; Mails Output Total / Failed: %d / %d<br>\n",MFS_MF_MailsOutputTotal,MFS_MF_MailsOutputFailed));


	//		HttpSendDataSprintf(hndl," &nbsp;&nbsp; Run-Time: %d h %d min.<br>\n" , (statRet/60) , (statRet) - (((long)(statRet/60))*60) );
	//		HttpSendDataSprintf(hndl,);
	//		HttpSendDataSprintf(hndl,);
		
	} else {
	
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
			DL_HttpSendData(hndl," &nbsp;&nbsp; MailFilter Server/NLM is not loaded, not loaded in the OS space, or is incompatible with this Version of MailFilter/NRM.<br>\n");
		}
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

	DL_HttpSendData(hndl,strprintf(" &nbsp;&nbsp; Configuration File: %s<br>\n",MF_GlobalConfiguration.config_file));
	DL_HttpSendData(hndl,"    <br>\n");
	
	DL_HttpSendData(hndl,strprintf(" &nbsp;&nbsp; MailFilter Home: %s<br>\n",MF_GlobalConfiguration.MFLTRoot));
	DL_HttpSendData(hndl,strprintf(" &nbsp;&nbsp; GWIA Home: %s<br>\n",MF_GlobalConfiguration.GWIARoot));
	
	
	// end
	DL_HttpSendData(hndl,"    <br>\n");
	DL_BuildAndSendFooter(hndl);
	
	DL_HttpEndDataResponse(hndl);
		
ERR_END:

#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)
	SetThreadGroupID(handlerThreadGroupID);	
#endif
	
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
		/* I- descriptionString	*/	(const unsigned char*)MF_NLM_RM_rTagName,
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

	if (MF_GlobalConfiguration.EnableNRMRestore)
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

	MF_DisplayCriticalError("MAILFILTER: Terminating NRM module.\n");

	MF_NRM_InitComplete = 2; 	/* deinit... */

	BOOL bCode;
	UINT32 rCode = 0;
	
	/*------------------------------------------------------------------------
	**	Deregister both service methods before the NLM unloads, in the 
	**	opposite order that they were registered.
	*/
	if (MF_GlobalConfiguration.EnableNRMRestore)
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
	printf ("MailFilter NRM: Initializing...\n");

	// Rename thread
	NXContextSetName(NXContextGet(),"MailFilterNRM");
	RenameScreen(getscreenhandle(),"MailFilter NRM");

	if (!DLSetupNRM())
	{
		printf("Could not import PORTAL/HTTPSTK symbols. Aborting Startup.\n");
		return -1;
	}

	if (!MF_NLM_RM_Init())
	{
		printf ("Portal Startup Failed.\n");
		return -1;
	}
	// Pretend that we've already exited.
	--MFT_NLM_ThreadCount;

	
	if (cond_init ( &condMainThread , USYNC_THREAD, NULL ) != 0)
	{
		printf("Could not initialize mutex. Startup Failed\n");
	}
	else 
	{
	
		printf("MailFilter NRM Running.\n");
	
		mutex_t mtx;
		mutex_init(&mtx, USYNC_THREAD, NULL);
		mutex_lock(&mtx);
		cond_wait(&condMainThread,&mtx);
	}
   	
	printf("Shutdown...\n");
	
	MF_NLM_RM_DeInit();
	DLDeSetupNRM();

	cond_signal(&condMainThread);
	cond_destroy(&condMainThread);
	
	return 0;	
}
