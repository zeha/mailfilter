/****************************************************************************
**	File:	MFInterface.C
**
**	Desc:	MailFilter Remote Manager Plug-In Interface to MailFilter/ax
**
**  (C) Copyright 2001-2002 Christian Hofstädtler. All Rights Reserved.
**							
*/

#include "mfnrm.h"
#include <nwcntask.h>

#include "../../../Config/MFConfig.h"
#include <ix.h>
extern "C" { int checkGroupMembership(char* group, NWDSContextHandle dContext); }

extern int NLM_threadCnt;
extern int NLM_mainThreadGroupID;
extern int MF_NRM_InitComplete;
extern char MF_ProductName[];



int MF_ExamineFile(const char *fileIn, char *szMailFrom_, char *szMailRcpt_, char *szMailSubject_)
{

	FILE *mailFile				= NULL;
	int q						= 255;
	int curChr					= 0;
	int curPos					= 0;
	int curCmpPos				= 0;
	int iFirstNonSpaceCharacter = 0;
	char *pszFirstNonSpaceCharacter;
	
	char* szScanBuffer;
	
	bool  bFilterMatched		= FALSE;
	int	  iFilterHandle			= 0;

	bool  bHaveFrom				= false;
	char* szMailFrom;

	bool  bHaveRcpt				= false;
	char* szMailRcpt;

	bool  bHaveSubject			= false;
	char* szMailSubject;

	bool  bNextLineStartSpace	= false;
	char  cNextLineStart		= 0;

	mailFile = fopen(fileIn,"rb");
	if (mailFile == NULL)
	{
		return 240;
	}

	szScanBuffer  	=(char*)malloc(2002);
	szMailFrom 		=(char*)malloc( 252);
	szMailRcpt 		=(char*)malloc( 252);
	szMailSubject  	=(char*)malloc(2002);
	if (szScanBuffer == NULL)		return 239;
	if (szMailFrom == NULL)			return 239;
	if (szMailRcpt == NULL)			return 239;
	if (szMailSubject == NULL)		return 239;

	memset ( szScanBuffer	, 0 , 2000 );
	memset ( szMailFrom		, 0 ,  250 );
	memset ( szMailRcpt		, 0 ,  250 );
	memset ( szMailSubject	, 0 , 2000 );

	sprintf(szScanBuffer,"STRING INIT");
	// go to top of file
	fseek(mailFile,0,SEEK_SET);

	while (q == 255)
	{
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		if ( feof(mailFile) )
		{	q=0;	break;	}

		szScanBuffer[0]=0;
		iFirstNonSpaceCharacter = 0;

		// Get Line ...
		curPos = -1;
		do {
			curChr = 0;

			if (bNextLineStartSpace)
			{
				if (curPos > 0)
				{				
					// remove trailing \r or \n, that we may have carried over ...
					do {
						if ( (szScanBuffer[curPos] == '\0') || (szScanBuffer[curPos] == '\r') || (szScanBuffer[curPos] == '\n') )
							curPos--;
							else
							break;
					} while (curPos > 0);
				}
				
				curPos++;
				if (curPos >= 2000)	break;
				szScanBuffer[curPos]=0x20;		// Add Space to buffer (separator).

				bNextLineStartSpace = false;
			} else {
				if ( (cNextLineStart != -1) && (cNextLineStart != 0) && (cNextLineStart!='\n') && (cNextLineStart!='\r') && (cNextLineStart!=' ') && (cNextLineStart!=0x9) )
				{
					curPos++;
					if (curPos >= 2000)	break;
					szScanBuffer[curPos]=cNextLineStart;
				}
			}
			szScanBuffer[2000] = 0;
			
			while ((curChr != '\n') && (curChr != -1))
			{
				if (curPos >= 2000) break;
				curChr = fgetc(mailFile);

				curPos++;

				if ((curChr == '\n') || (curChr == -1))
					break;
				if ( curChr == 0x9 )				// Tab to Single Space
					curChr = 0x20;
				if ( (curChr != '\r') && (curChr != '\n'))
					szScanBuffer[curPos]=(char)curChr;
			}
			
			curPos--;
			szScanBuffer[curPos]=0;
			szScanBuffer[2000]=0;
			
			do
			{
				cNextLineStart = (char)fgetc(mailFile);
					
				if (cNextLineStart == -1)
					break;
					
					
			} while ( ( cNextLineStart == '\n' ) || ( cNextLineStart == '\r' ) );

			switch (cNextLineStart)
			{
			case 0x9:
				bNextLineStartSpace = true;
				break;
			case 0x20:
				bNextLineStartSpace = true;
				break;
			case -1:	// fall through
			default:
				bNextLineStartSpace = false;
				break;	// end
			}
			
			if (cNextLineStart == -1)
				bNextLineStartSpace = false;
				
		} while (bNextLineStartSpace && (curPos<2000));
		szScanBuffer[2000]=0;


		switch (szScanBuffer[0])
		{
			case 'm':
			case 'M':
				if (!bHaveFrom)		/* MAIL FROM: Filter - bHaveFrom wird nur =true gesetzt, wenn der Filter matcht, da dieses Feld nicht */
				{					/* immer vorhanden ist, bzw. gültigen Inhalt hat (MAX!->admin@domain.com) */
					if ( memicmp(szScanBuffer,"mail from:",10) == 0 )
					{
						szMailFrom[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+10);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						memcpy( szMailFrom , pszFirstNonSpaceCharacter , 245 );
						szMailFrom[245]=0;
						
					}
				}
				break;
			case 'f':
			case 'F':
				if (!bHaveFrom)
				{
					if ( memicmp(szScanBuffer,"from:",5) == 0 )
					{
						bHaveFrom = true;
						szMailFrom[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+5);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						memcpy( szMailFrom , pszFirstNonSpaceCharacter , 245 );
						szMailFrom[245]=0;
						
					}
				}
				break;
			case 'r':
			case 'R':
				if (!bHaveRcpt)		/* RCPT TO: Filter - bHaveRcpt wird nur =true gesetzt, wenn der Filter matcht, da dieses Feld nicht */
				{					/* immer vorhanden ist, bzw. gültigen Inhalt hat */
					if ( memicmp(szScanBuffer,"rcpt to:",8) == 0 )
					{
						szMailRcpt[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+8);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						memcpy( szMailRcpt , pszFirstNonSpaceCharacter , 245 );
						szMailRcpt[245]=0;

					}
				}
				break;
			case 't':
			case 'T':
				if (!bHaveRcpt)
				{
					if ( memicmp(szScanBuffer,"to:",3) == 0 )
					{
						bHaveRcpt = true;
						szMailRcpt[0]=0;
						
						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+3);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						memcpy( szMailRcpt , pszFirstNonSpaceCharacter , 245 );
						szMailRcpt[245]=0;
					}
				}
				break;
			case 's':
			case 'S':
				if (!bHaveSubject)
				{
					if ( memicmp(szScanBuffer,"subject:",8) == 0 )
					{
						bHaveSubject = true;
						szMailSubject[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = ( szScanBuffer+8);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						memcpy( szMailSubject , pszFirstNonSpaceCharacter , 1998 );
						szMailSubject[1999]=0;

					}
				}
				break;
			default:
				break;
			}
	
	
		if (bHaveSubject && bHaveRcpt && bHaveFrom)
		{
			q = 1;
			break;
		}

	}	// end of while

	fclose(mailFile);

	if (szMailFrom_ != NULL)	{	strncpy(szMailFrom_,szMailFrom,250); szMailFrom_[250]=0;	}
	if (szMailRcpt_ != NULL)	{	strncpy(szMailRcpt_,szMailRcpt,250); szMailRcpt_[250]=0;	}
	if (szMailSubject_ != NULL)	{	strncpy(szMailSubject_,szMailSubject,250); szMailSubject_[250]=0;	}

	free(szMailFrom);
	free(szMailRcpt);
	free(szMailSubject);
	free(szScanBuffer);

#ifdef N_PLAT_NLM
	ThreadSwitch();
#endif

	return q;
				
}

int MF_NRM_RestoreFile(const char* szInFile, const char* szOutFile, char* szType)
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
	
#ifndef WIN32
	flushall();
#endif
	fclose(inputFile);
	fclose(outputFile);
	free(szScanBuffer);
	unlink(szInFile);
	
	return 0;
}

int stripOffHtml(char* str)
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

UINT32 MF_NLM_RM_HttpHandler_Restore(
		/* I- httpHndl		*/	HINTERNET hndl,
		/* I- pExtraInfo	*/	void *pExtraInfo,
		/* I- szExtraInfo	*/	UINT32 szExtraInfo,
		/* I- InfoBits		*/	UINT32 InformationBits
		)
	{
	UINT32 rCode = 0;
	long statRet = -1;
	int handlerThreadGroupID;
	int err = 0; //stores returned error value
	int len = 0; //determine if request is valid length
	//char *bp; //stores path portion of URL
	//char* cnv; //un-escaped path buffer
	UINT32 urlcount = 0; //
	UINT32 methodType = 0; //stores request type
	nuint32           bufferSize;
	char*             pathBufferPtr;
	char*             unEscPathBufPtr;
	int				  membership = 0;
	NWDSContextHandle dContext;
//	NWDSCCODE			dscCode;
	UINT32				nwConnNumber;
	UINT32				nwTaskNumber;
	UINT32				origConnNumber;
	UINT32				origTaskNumber;
//	char             pathBufferPtr[257];
//	char             unEscPathBufPtr[257];

	if (MF_NRM_InitComplete != 1)
		return 0;
	if (MFC_MFLT_ROOT == NULL)
		return 0;
	if (MFC_GWIA_ROOT == NULL)
		return 0;

	++NLM_threadCnt;

	handlerThreadGroupID = GetThreadGroupID();
	SetThreadGroupID(NLM_mainThreadGroupID);

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


//	dscCode=NWDSCreateContextHandle(&dContext);
/*	if(dscCode)
	{
		printf ("MailFilter/NRM: Unable to create NDS Context Handle!\n");
		--NLM_threadCnt;
		return 0;
	}
*/
	serviceGetConnectionNumber(hndl,&nwConnNumber,&nwTaskNumber);

	origConnNumber = SetCurrentConnection(nwConnNumber);
	origTaskNumber = SetCurrentTask(nwTaskNumber);

	int iCtx = 0;
	serviceGetLocalNDSContext(hndl,&iCtx);
	dContext = (NWDSContextHandle)iCtx;


//	membership = checkGroupMembership("cn=MailFilter Restore Users.o=System",NULL);
	membership = checkGroupMembership("MailFilter Restore Users",dContext);
/*	
	if (!membership)
	{
		//hndl is invalid, return error
		HttpSendErrorResponse(hndl, HTTP_STATUS_DENIED );
		--NLM_threadCnt;
		return(HTTP_STATUS_DENIED);
	}
*/

	if (HttpReturnRequestMethod(hndl, &methodType) != TRUE)
	{
		//hndl is invalid, return error
		HttpSendErrorResponse(hndl, HTTP_INTERNAL_SERVER_ERROR);
		--NLM_threadCnt;
		return(HTTP_INTERNAL_SERVER_ERROR);
	}		

	if (methodType != HTTP_REQUEST_METHOD_GET)
	{
		//request is not a get, return error
		HttpSendErrorResponse(hndl, HTTP_STATUS_NOT_SUPPORTED);
		--NLM_threadCnt;
		return(HTTP_STATUS_NOT_SUPPORTED);
	}
	
/*	//get the requested path, remove our service name
	//and make sure that the request is valid
	err = HttpReturnPathBuffers(hndl, NULL, &bp, &cnv);
	if (err)
	{
		//hndl is invalid, return error
		HttpSendErrorResponse(hndl, HTTP_INTERNAL_SERVER_ERROR);
		return(HTTP_INTERNAL_SERVER_ERROR);
	}
	
*/	
	bufferSize = 256;
	rCode = (unsigned long) HttpReturnPathBuffers( hndl, &bufferSize, &pathBufferPtr, &unEscPathBufPtr);
	if (rCode)
	{
		HttpSendErrorResponse( hndl, HTTP_STATUS_NOT_FOUND );
		HttpEndDataResponse( hndl );  /* Flush bufs and close session */
		--NLM_threadCnt;
		return 0;
	}


	HttpSendSuccessfulResponse(hndl, HttpReturnString(HTTP_CONTENT_TYPE_HTML));
	
	// Output Page Header ...	
	BuildAndSendHeader( hndl, "Mail Restore", "Mail Restore", /* REFRESH */ 0, /* RFRSHTIMER */0, /* FLAGS */ 0, NULL, "", "");


	HttpSendDataSprintf(hndl," <b><span style=\"color: red;\">WARNING: This tool is a potential security risk. If restored mails are not examined carefully, this could lead to viruses in the system!</span></b><br><br>\n");
	
	HttpSendDataSprintf(hndl," Group Membership: %i<br>\n",membership);



	char *						scanDir = (char*)malloc(MAX_PATH);
	char *						scanPath = (char*)malloc(MAX_PATH);
	char *						szLink = (char*)malloc(MAX_PATH);
	char *						szHttpPath = (char*)malloc(MAX_PATH);
	DIR *						mailDir;
	DIR *						readentry;
	char *						szFile = (char*)malloc(MAX_PATH);
	char						szDate[81];
	char *						szList = (char*)malloc(MAX_PATH+1000);
	char *						szFileOut = (char*)malloc(MAX_PATH);
	char *						szMailFrom = (char*)malloc(2500);
	char *						szMailRcpt = (char*)malloc(2500);
	char *						szMailSubject = (char*)malloc(2500);
	int							iDirection = 0;

	sprintf(scanPath,"%s\\MFPROB\\DROP",MFC_MFLT_ROOT);
	chdir(scanPath);


	if (memicmp(pathBufferPtr, "/"MAILFILTER_NRM_SERVICETAG_RESTORE"/RESTORE/", MAILFILTER_NRM_SERVICETAG_RESTORE_LEN+10) == 0)
	{
		// Restore a file ...
		strncpy(szFile,pathBufferPtr + MAILFILTER_NRM_SERVICETAG_RESTORE_LEN+10,13);
		
		iDirection = (szFile[0] == 'S') ? 1 : 2;
		if (iDirection == 1)
			sprintf(szFileOut,"%s\\SEND\\%s",MFC_MFLT_ROOT,szFile);
		if (iDirection == 2)
			sprintf(szFileOut,"%s\\RECEIVE\\%s",MFC_GWIA_ROOT,szFile);
			
		sprintf(szList,"%s\\%s",scanPath,szFile);
		if (!MF_NRM_RestoreFile(szList,szFileOut,"RESTORE")) {
		
			HttpSendDataSprintf(hndl,"<br>  <b>The mail has been queued to GWIA.</b><br><br>\n");
			
		} else {

			HttpSendDataSprintf(hndl,"<br>  <b>Restore has FAILED.</b><br><br>\n");
		
		}	
	} else if (memicmp(pathBufferPtr, "/"MAILFILTER_NRM_SERVICETAG_RESTORE"/RECHECK/", MAILFILTER_NRM_SERVICETAG_RESTORE_LEN+10) == 0)
	{
		// Recheck a file ...
		strncpy(szFile,pathBufferPtr + MAILFILTER_NRM_SERVICETAG_RESTORE_LEN+10,13);

		iDirection = (szFile[0] == 'S') ? 1 : 2;
		if (iDirection == 1)
			sprintf(szFileOut,"%s\\SEND\\%s",MFC_GWIA_ROOT,szFile);
		if (iDirection == 2)
			sprintf(szFileOut,"%s\\RECEIVE\\%s",MFC_MFLT_ROOT,szFile);
			
		sprintf(szList,"%s\\%s",scanPath,szFile);
		if (!MF_NRM_RestoreFile(szList,szFileOut,"RECHECK")) {
		
			HttpSendDataSprintf(hndl,"<br>  <b>The mail has been queued for reprocessing.</b><br><br>\n");
			
		} else {

			HttpSendDataSprintf(hndl,"<br>  <b>Recheck has FAILED when trying to move the mail...</b><br><br>\n");
		
		}	
	
	} else {

		int page = 0;	// start at page 0, entry 0
		int entriesPerPage = 20;
		int thisEntry = 0;
		bool bHaveMoreEntries = false;
		
		if (memicmp(pathBufferPtr, "/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/", MAILFILTER_NRM_SERVICETAG_RESTORE_LEN+6) == 0)
		{
			char szPage[10];
			memset(szPage,0,8);
			strncpy(szPage,pathBufferPtr+MAILFILTER_NRM_SERVICETAG_RESTORE_LEN+6,8);
			szPage[8] = 0;
			szPage[9] = 0;
			page = atoi(szPage+1);
		} // else the defaults apply
			
		
		// Display List

		/* Available Mails */
//		HttpSendDataSprintf(hndl,"<br>  <b>Available Mails:</b><br>\n");
		HttpSendDataSprintf(hndl, "<table cellpadding=1 cellspacing=0 width=800>\n"
			"<tr><th width=100>Direction</th><th width=100>Date</th><th width=100>Size</th><th>Original Filename</th></tr>\n" );
		
		/*------------------------------------------------------------------------
		** Look for dropped mails and put them in the list.
		*/

		if (strchr(scanPath,'\\') != NULL)
			strcpy(szHttpPath,strchr(scanPath,'\\')+1);
			else
			strcpy(szHttpPath,scanPath);
			

/*		len = strlen(scanPath);
		szFile[0] = scanPath[len];
		if ( (szFile[0] != '\\') && (szFile[0] != '/') )
		{
			scanPath[len] = '\\';
			scanPath[len+1] = 0;
		}
*/			
		for (int curPos = 0; curPos < strlen(szHttpPath);curPos++)
		{
			if (szHttpPath[curPos] == ':')
			{
				szHttpPath[curPos] = '/';
				if ( (szHttpPath[curPos+1] == '\\') || (szHttpPath[curPos+1] == '/') )
					strcpy(szHttpPath+curPos,szHttpPath+curPos+1);
			}
			if (szHttpPath[curPos] == '\\')
				szHttpPath[curPos] = '/';
			
		}


		sprintf(scanDir,"%s\\*.*",scanPath);
		mailDir = opendir(scanDir);
		while ( mailDir != NULL )
		{
			readentry = readdir(mailDir);
			if (readentry == NULL) { break;	} // No more files.
			
			thisEntry++;
			if (thisEntry < (page*entriesPerPage))
				continue;
			if (thisEntry > ((page+1)*entriesPerPage))
			{
				bHaveMoreEntries = true;
				continue;
			}
			
			strcpy (szFile,readentry->d_nameDOS);
			szFile[13] = 0;

			iDirection = (szFile[0] == 'S') ? 1 : 2;
			if (iDirection == 1)
				sprintf(szFileOut,"%s\\SEND\\%s",MFC_MFLT_ROOT,szFile);
			if (iDirection == 2)
				sprintf(szFileOut,"%s\\RECEIVE\\%s",MFC_GWIA_ROOT,szFile);

			sprintf ( szDate, "%04d/%02d/%02d", GET_YEAR_FROM_DATE(readentry->d_date)+1980 , GET_MONTH_FROM_DATE(readentry->d_date) , GET_DAY_FROM_DATE(readentry->d_date));
			sprintf ( szLink, "<a href=\"/%s/%s\">Inspect Mail</a>",szHttpPath,szFile,szFile);
			sprintf ( szList, "<tr bgcolor=\"#efeee9\" ><td>%s</td><td>%s</td><td>%6d kB</td><td>%s</td></tr>\n", (iDirection == 1) ? "Outgoing" : " Incoming", szDate, ((readentry->d_size)/1024)+1, szFileOut );

			HttpSendDataSprintf(hndl,szList);

			MF_ExamineFile(szFile, szMailFrom, szMailRcpt, szMailSubject);
			stripOffHtml(szMailFrom);
			stripOffHtml(szMailRcpt);
			stripOffHtml(szMailSubject);

			HttpSendDataSprintf(hndl, "<tr bgcolor=\"#ffffff\"><td colspan=2></td><td>From:</td><td colspan=3>%s</td></tr>\n", szMailFrom);
			HttpSendDataSprintf(hndl, "<tr bgcolor=\"#ffffff\"><td colspan=2></td><td>To:</td><td colspan=3>%s</td></tr>\n", szMailRcpt);
			HttpSendDataSprintf(hndl, "<tr bgcolor=\"#ffffff\"><td colspan=2></td><td>Subject:</td><td colspan=3>%s</td></tr>\n\n", szMailSubject);
			HttpSendDataSprintf(hndl, "<tr bgcolor=\"#ffffff\"><td colspan=2></td><td colspan=2>%s &nbsp; "
				"<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/RESTORE/%s\">Restore</a> &nbsp; "
				"<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/RECHECK/%s\">Recheck</a> <br><br>"
				"</td></tr>\n\n", szLink, szFile, szFile);
			
		}
		closedir(mailDir);

		
		HttpSendDataSprintf(hndl,"\n<tr>\n");
		HttpSendDataSprintf(hndl,"\n<th align=left colspan=2> \n");
		HttpSendDataSprintf(hndl,"\n<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/0\">|&lt; first page</a> \n",page-1);
		if (page>0)
		{
			HttpSendDataSprintf(hndl,"\n<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/%d\">&lt;&lt; prev. page</a> \n",page-1);
			
		}
		HttpSendDataSprintf(hndl,"\n</th><th align=right colspan=2> \n");
		if (bHaveMoreEntries==true)
		{
			HttpSendDataSprintf(hndl,"\n<a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/%d\">next page &gt;&gt;</a> \n",page+1);
			
		}
		HttpSendDataSprintf(hndl,"\n <a href=\"/"MAILFILTER_NRM_SERVICETAG_RESTORE"/PAGE/%i\">last page &gt;|</a> \n",thisEntry/entriesPerPage);
		HttpSendDataSprintf(hndl," </th></tr>\n");

		HttpSendDataSprintf(hndl,"</table>    <br>\n");
	}
	
	// FOOTER ...
	
	BuildAndSendFooter(hndl);
	HttpEndDataResponse(hndl);


	free(szMailFrom);
	free(szMailRcpt);
	free(szMailSubject);
	free(scanDir);
	free(scanPath);
	free(szLink);
	free(szHttpPath);
	free(szFile);
	free(szList);
	free(szFileOut);


	SetCurrentConnection(origConnNumber);
	SetCurrentTask(origTaskNumber);
	
	// Restore Thread Group
	SetThreadGroupID(handlerThreadGroupID);

//	NWDSFreeContext(dContext);

ERR_END:

	--NLM_threadCnt;
	return(rCode);
}