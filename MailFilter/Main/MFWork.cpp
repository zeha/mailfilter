/*
 +
 +		MFWork.cpp
 +		
 +		Worker Thread Code
 +		
 +		Copyright 2001-2004 Christian Hofstädtler.
 +		
 +		
 +		
 */


#define _MFD_MODULE			"MFWork.CPP"
#include "MailFilter.h"
#include "MFMail.h++"
#include "MFRelayHost.h++"
#include "MFZip.h"
#include "MFConfig-defines.h"
#include "MFVersion.h"

static int MF_GoOn();

static int MF_Notification_Send(MailFilter_MailData* m);

// MailFilter Configuration Cache
static char MFT_GWIA_SendDirIn		[MAX_PATH];
static char MFT_GWIA_SendDirOut		[MAX_PATH];
static char MFT_GWIA_RecvDirIn		[MAX_PATH];
static char MFT_GWIA_RecvDirOut		[MAX_PATH];
static char MFT_GWIA_ResultDirIn	[MAX_PATH];
static char MFT_GWIA_ResultDirOut	[MAX_PATH];
static char MFT_MF_ScheduleDirSend	[MAX_PATH];
static char MFT_MF_ProbDir			[MAX_PATH];


#ifdef HAVE_WINSOCK
// Helper function to start WSA and associates ...
static bool MF_Worker_WSA_Startup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	 
	wVersionRequested = MAKEWORD( 2, 2 );
	 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
	    /* Tell the user that we could not find a usable */
	    /* WinSock DLL.                                  */
	    return false;
	}
	 
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	 
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
	        HIBYTE( wsaData.wVersion ) != 2 ) {
	    /* Tell the user that we could not find a usable */
	    /* WinSock DLL.                                  */
	    WSACleanup( );
	    return false; 
	}
	return true;
}

// Helper function to shut down WSA and associates ...
static bool MF_Worker_WSA_Shutdown()
{
	WSACleanup( );
	return true;
}
#else
#define MF_Worker_WSA_Startup()	
#define MF_Worker_WSA_Shutdown()
#endif /* HAVE_WINSOCK */




//static MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];
//static char MFT_ModifyInt_ListFrom[MFT_ModifyInt_ListFrom_MAX][50];

/* Sleep Time for the worker thread */
#define MF_WORKER_SLEEPTIME 3000

static char* MF_GetRoot();

#include "MFVirusScan.h"


//
//
static int MF_CopyEmail_Send(const char* szAttachFileName, MailFilter_MailData* mMailInfo, const char* szDestinationEMail)
{
	FILE *fMailFile	= NULL;
	char* fileName[13];
	char* fileOut = (char*)_mfd_malloc(strlen(MFT_GWIA_RecvDirOut)+14,"CopyEmail_Send");		// 13=(8+1+3)
	int addVal = 0;

	if (fileOut == NULL)	{ MF_OutOfMemoryHandler();	return 239; }

	if (!MFL_GetFlag(MAILFILTER_MC_M_MAILCOPY))
		return 0;

	do
	{
		MF_Util_MakeUniqueFileName( (char*)fileName , addVal );
		strcpy( fileOut , MFT_GWIA_RecvDirOut );
		strcat( fileOut , (const char*)fileName );
		addVal = addVal+56;			// Random file num for MF_Util_MakeUniqueFilename on retry ...
	}
	while ( access(fileOut,F_OK) == 0 );

	fMailFile = fopen(fileOut,"wt");
	_mfd_free(fileOut,"CopyEmail_Send");
	if (fMailFile == NULL)	{	MF_OutOfMemoryHandler(); return 239; }

	char szBoundary[50];
	MF_Util_MakeUniqueFileName(szBoundary,135);
	

	FILE *fTemplate			= NULL;
	char* fileTemplate		= (char*)_mfd_malloc(MAX_PATH,"MailCopy (Tmpl)");
	char* templateString	= NULL;
	int iTemplateChar		= 0;
	int iTemplateCharPrev	= 0;

	if (fileTemplate == NULL)	{	fclose(fMailFile);	MF_OutOfMemoryHandler(); return 239; }

	sprintf(fileTemplate,"%s\\%s",MF_GlobalConfiguration.config_directory.c_str(),"MAILCOPY.TPL");

	fTemplate = fopen(fileTemplate,"rt");
	
	_mfd_free(fileTemplate,"MailCopy (Tmpl)");
	
	if (fTemplate == NULL)
	{
		MF_StatusText("Error reading MailCopy template!");

		fprintf(fMailFile,"MAIL FROM:%s\r\nRCPT TO:%s\r\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),szDestinationEMail);
		
		fprintf(fMailFile,"From: %s\nTo: %s\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),szDestinationEMail);
		fprintf(fMailFile,"Subject: MailFilter MailCopy\r\n");
		fprintf(fMailFile,"X-Mailer: MailFilter professional "MAILFILTERVERNUM"\r\nMime-Version: 1.0\r\n");
		fprintf(fMailFile,"Content-Type: multipart/mixed; boundary=\"==MFMFMFMFMF%s\"\r\n\r\n",szBoundary);
		fprintf(fMailFile,"--==MFMFMFMFMF%s\r\n",szBoundary);
		fprintf(fMailFile,"Content-Type: text/plain; charset=US-ASCII\r\nContent-Transfer-Encoding: 7bit\r\nContent-Disposition: inline\r\n\r\n");

		fprintf(fMailFile,"ERROR: This is the default text. It is used, \r\n");
		fprintf(fMailFile,"because the template is not accessible!\r\n\r\n");

		fprintf(fMailFile,"\r\n\r\n");
	
		FILE* fAttFile = fopen(szAttachFileName,"rb");
		if (fAttFile == NULL)
		{
			fprintf(fMailFile,"ERROR: A file called \"%s\" should have\r\nbeen attached to this mail\r\nbut it couldn't be opened.\r\n",szAttachFileName);
		} else {
			fprintf(fMailFile,"--==MFMFMFMFMF%s\r\n",szBoundary);
			fprintf(fMailFile,"Content-Type: message/rfc822\r\n");
			fprintf(fMailFile,"\r\n");
			
			int curChr = 0;
			while ( ( curChr = fgetc(fAttFile) ) != EOF )
			{
				fputc(curChr,fMailFile);
			}
			fclose(fAttFile);
		}
	
		fprintf(fMailFile,"\r\n%s\r\n",szBoundary);

		
	} else {

		while ( (iTemplateChar = fgetc(fTemplate)) != EOF )
		{
			if (iTemplateChar == '#')
			{
				if ( (iTemplateChar == '\r') || (iTemplateChar == '\n') )
				{
					// a comment starts here ...
					while ( (iTemplateChar = fgetc(fTemplate)) != EOF )
					{	
						// as we only support line-start to line-end comments,
						// look for the line end here...
						if ( (iTemplateChar == '\r') || (iTemplateChar == '\n') )
							break;
					}
				}
			
			} else {

				if ( (iTemplateCharPrev == '%') && (iTemplateChar != '%') )
				{
					// allocate a buffer
					if (templateString == NULL)
						templateString = (char*)_mfd_malloc(100,"templateString");
				
					// overwrite the previously written %
					fseek(fMailFile,ftell(fMailFile)-1,SEEK_SET);
					templateString[0] = 0;
					
					switch (iTemplateChar)
					{
						case 'f':
							// original mail from
							sprintf(templateString,"%.79s",mMailInfo->szMailFrom);
							break;
						case 't':
							// original mail to
							sprintf(templateString,"%.79s",mMailInfo->szMailRcpt);
							break;
						case 's':
							// original mail subject 
							sprintf(templateString,"%.79s",mMailInfo->szMailSubject);
							break;
						case 'R':
							// drop reason
							sprintf(templateString,"%.79s",mMailInfo->szErrorMessage);
							break;
						case 'F':
							// Filename
							sprintf(templateString,"%.79s",mMailInfo->szFileIn);
							break;
						case 'P':
							// PostMaster E-Mail
							sprintf(templateString,"%.79s",MF_GlobalConfiguration.DomainEmailPostmaster.c_str());
							break;
						case 'M':
							// MailFilter E-Mail
							sprintf(templateString,"%.79s",MF_GlobalConfiguration.DomainEmailMailFilter.c_str());
							break;
						case 'S':
							// Servername
							sprintf(templateString,"%.79s",MF_GlobalConfiguration.ServerName.c_str());
							break;
						case 'V':
							// MailFilter Product Version
							sprintf(templateString,"%.79s",MAILFILTERVERNUM);
							break;
						case 'U':
							// some unique text
							sprintf(templateString,"%.79s",szBoundary);
							break;
						case 'C':
							// Copy recipient
							sprintf(templateString,"%.79s",szDestinationEMail);
							break;
						case 'X':
MFD_Out(MFD_SOURCE_VSCAN,"MailCopy: %s\n",szAttachFileName);
MFD_Out(MFD_SOURCE_VSCAN,">> Output: %s\n",fileName);
							FILE* fAttFile = fopen(szAttachFileName,"rb");
							if (fAttFile == NULL)
							{
								sprintf(templateString,"ERROR: A file called \"%s\" should have\r\nbeen attached to this mail\r\nbut it couldn't be opened.\r\n",szAttachFileName);
							} else {
								sprintf(templateString,"\r\n");
							
								int curChr = 0;
								long bytes = 0;
								while ( ( curChr = fgetc(fAttFile) ) != EOF )
								{
									fputc(curChr,fMailFile);
									++bytes;
									if(curChr==0) MFD_Out(MFD_SOURCE_MAIL,"**NULL** ");
								}
								fclose(fAttFile);
MFD_Out(MFD_SOURCE_VSCAN,">> Attached %d bytes.\n",bytes);
							}
							break;
						
						default:
							// Unknown
							sprintf(templateString,"** Escape Character %c not recognized. **",iTemplateChar);
							break;
					}
					
					fprintf(fMailFile,"%s",templateString);
				} else {
					// nothing special; write it out...
					fputc(iTemplateChar,fMailFile);
				}
			}

			iTemplateCharPrev = iTemplateChar;
		}
	
		fclose(fTemplate);
		
	}

	if (templateString != NULL)
		_mfd_free(templateString,"templateString");

	return 0;
}


int MFAPI_FilterCheck( char *szScan , int mailSource, int matchfield )
{
	int rc = 0;
	int scanLen = 0;
	unsigned int curItem = 0;
	pcre *re;
	int ovector[OVECCOUNT];
	const char *error;
	int erroffset;

	char *toScan = _mfd_strdup(szScan,"APIFilterCheckSBuf");
	scanLen = (int)strlen(toScan);

	strlwr(toScan);

	while(rc==0)
	{
//		if (curItem > MailFilter_MaxFilters) break;
		if (curItem > MF_GlobalConfiguration.filterList.size()) break;

//		if (MFC_Filters[curItem].expression[0] == 0)
		if (MF_GlobalConfiguration.filterList[curItem].expression == "")
			break;

		if (
			
			(MF_GlobalConfiguration.filterList[curItem].matchfield == matchfield) && 
			
			( 
				((mailSource == MAILFILTER_MAILSOURCE_INCOMING) && (MF_GlobalConfiguration.filterList[curItem].enabledIncoming == true))
				||
				((mailSource == MAILFILTER_MAILSOURCE_OUTGOING) && (MF_GlobalConfiguration.filterList[curItem].enabledOutgoing == true))
			)
			
		) {

			re = pcre_compile(
			  MF_GlobalConfiguration.filterList[curItem].expression.c_str(), /* the pattern */
			  0,                    /* default options */
			  &error,               /* for error message */
			  &erroffset,           /* for error offset */
			  NULL);                /* use default character tables */

			if (re == NULL)
				MFD_Out(MFD_SOURCE_REGEX,"API: RegExp compilation failed at offset %d: %s\n", erroffset, error);
				else {

				rc = pcre_exec(
				  re,                   /* the compiled pattern */
				  NULL,                 /* no extra data - we didn't study the pattern */
				  toScan,              	/* the subject string */
				  scanLen,				/* the length of the subject */
				  0,                    /* start at offset 0 in the subject */
				  0,                    /* default options */
				  ovector,              /* output vector for substring information */
				  OVECCOUNT);           /* number of elements in the output vector */
				pcre_free(re);

				// success, error?
				if (rc < 0)
				{
				// Match Failed. Check why...
				    if (rc == PCRE_ERROR_NOMATCH)
				    {	// no error, but no match
				    		rc = 0;
				    		// if this is a NO match rule, then the filter applies and we break the cycle...
							if (MF_GlobalConfiguration.filterList[curItem].type == MailFilter_Configuration::noMatch)
							{
								rc=(int)curItem+1;
								if (MF_GlobalConfiguration.filterList[curItem].action == MailFilter_Configuration::schedule)
									rc = 0;
								
								if (rc)	break;
							}
				    	}
				    	else
				    	MFD_Out(MFD_SOURCE_REGEX,"API: RegExp Matching error %d\n", rc); /* error... */
				    	
				} else {
				// Match Applies.

		    		// if this is a NO match rule, then the filter DOES NOT apply
					if (MF_GlobalConfiguration.filterList[curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
						{
							rc=0;
						} else {
							/* normal MATCH rule, break the cycle */
							rc=(int)curItem+1;
							if (MF_GlobalConfiguration.filterList[curItem].action == MAILFILTER_MATCHACTION_SCHEDULE)
								rc = 0;
							
							if (rc)	break;
						}
				}

			}
		}
		
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif

		curItem++;
	}

	if (rc > 0)
	{
		// check if we just should cancel rule processing if the
		// rule matches
		if (MF_GlobalConfiguration.filterList[curItem].action == MAILFILTER_MATCHACTION_PASS)
		{
			// yes, we have to!
			// clear our status code, this indicates no match ...
			rc = 0;
		}
	}
	
	_mfd_free(toScan,"APIFilterCheckSBuf");
	return rc;
}


// Checks the passed String for block ...
// * inAttachmentName: Original attachment name
// * mailSource
// > Returns filterNum+1 for "INFECTED" ;)
int MF_FilterCheck( MailFilter_MailData* m, char *szScan , int matchfield )
{
	int rc = 0;
	int scanLen = 0;
	unsigned int curItem = 0;
#ifdef _MAILFILTER_WITH_POSIX_REGEXP_H
	regexp myRegExp;
	regmatch_t *myRegMatch;
#endif //_MAILFILTER_WITH_POSIX_REGEXP_H
#ifdef _MAILFILTER_WITH_REGEXP_H
	pcre *re;
	int ovector[OVECCOUNT];
	const char *error;
	int erroffset;
#endif //_MAILFILTER_WITH_REGEXP_H

	char *toScan = _mfd_strdup(szScan,"FilterCheckSBuf");
	scanLen = (int)strlen(toScan);

#ifdef _MAILFILTER_WITH_REGEXP_H
	strlwr(toScan);
#endif

#ifdef _MAILFILTER_WITH_POSIX_REGEXP_H
	myRegMatch = (regmatch_t*)malloc(sizeof(regmatch_t) * strlen(toScan));
	if (myRegMatch == NULL)
	{
		MF_OutOfMemoryHandler();
		return -1;
	}
#endif // _MAILFILTER_WITH_POSIX_REGEXP_H

	while(rc==0)
	{
//		curItem++;
		if (curItem > MF_GlobalConfiguration.filterList.size()) break;

		if (MF_GlobalConfiguration.filterList[curItem].expression == "")
			break;

		if (
			
			(MF_GlobalConfiguration.filterList[curItem].matchfield == matchfield) && 
			
			( 
				((m->iMailSource == MAILFILTER_MAILSOURCE_INCOMING) && (MF_GlobalConfiguration.filterList[curItem].enabledIncoming == true))
				||
				((m->iMailSource == MAILFILTER_MAILSOURCE_OUTGOING) && (MF_GlobalConfiguration.filterList[curItem].enabledOutgoing == true))
			)
			
		) {

/*#ifdef _MAILFILTER_WITH_REGEXP_H*/
			re = pcre_compile(
			  MF_GlobalConfiguration.filterList[curItem].expression.c_str(), /* the pattern */
			  0,                    /* default options */
			  &error,               /* for error message */
			  &erroffset,           /* for error offset */
			  NULL);                /* use default character tables */

			if (re == NULL)
				MFD_Out(MFD_SOURCE_REGEX,"RegExp compilation failed at offset %d: %s\n", erroffset, error);
				else {

				rc = pcre_exec(
				  re,                   /* the compiled pattern */
				  NULL,                 /* no extra data - we didn't study the pattern */
				  toScan,              	/* the subject string */
				  scanLen,				/* the length of the subject */
				  0,                    /* start at offset 0 in the subject */
				  0,                    /* default options */
				  ovector,              /* output vector for substring information */
				  OVECCOUNT);           /* number of elements in the output vector */
				pcre_free(re);

				// success, error?
				if (rc < 0)
				{
				// Match Failed. Check why...
				    if (rc == PCRE_ERROR_NOMATCH)
				    {	// no error, but no match
				    		rc = 0;
				    		// if this is a NO match rule, then the filter applies and we break the cycle...
							if (MF_GlobalConfiguration.filterList[curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
							{
								rc=(int)curItem+1;
								if (MF_GlobalConfiguration.filterList[curItem].action == MAILFILTER_MATCHACTION_SCHEDULE)
								{
									m->bSchedule = true;
									rc = 0;
								}
								
								if (rc)	break;
							}
				    	}
				    	else
				    	MFD_Out(MFD_SOURCE_REGEX,"(RegExp) Matching error %d\n", rc); /* error... */
				    	
				} else {
				// Match Applies.

		    		// if this is a NO match rule, then the filter DOES NOT apply
					if (MF_GlobalConfiguration.filterList[curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
						{
							rc=0;
						} else {
							/* normal MATCH rule, break the cycle */
							rc=(int)curItem+1;
							if (MF_GlobalConfiguration.filterList[curItem].action == MAILFILTER_MATCHACTION_SCHEDULE)
							{
								m->bSchedule = true;
								rc = 0;
							}
							
							if (rc)	break;
						}
				}

			}
/*			
#else
#ifdef _MAILFILTER_WITH_POSIX_REGEXP_H
			if (regcomp(&myRegExp,MFC_Filters[curItem].expression) == 0)
			{
				if ( regexec(&myRegExp, toScan, scanLen, myRegMatch, 0) == 0 )
				{
					rc=curItem+1;
					regfree(&myRegExp);
					if (NULL != myRegMatch) _mfd_free(myRegMatch,"FilterCheck");
					myRegMatch = NULL;
					break;
				}
				regfree(&myRegExp);
			} else {
				MF_StatusText("Filter corrupt!");
			}
#endif //_MAILFILTER_WITH_POSIX_REGEXP_H [ELSE]
#endif //_MAILFILTER_WITH_REGEXP_H [ELSE]
*/
		}
		
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curItem++;
	}

#ifdef _MAILFILTER_WITH_POSIX_REGEXP_H
	if (NULL != myRegMatch)	_mfd_free(myRegMatch,"FilterCheck2");
	myRegMatch = NULL;
#endif //_MAILFILTER_WITH_POSIX_REGEXP_H


	if (rc > 0)
	{
		// check if we just should cancel rule processing if the
		// rule matches
		if (MF_GlobalConfiguration.filterList[curItem].action == MAILFILTER_MATCHACTION_PASS)
		{
			// yes, we have to!
			// clear our status code, this indicates no match ...
			rc = 0;
		}
	}
	if (rc)
	{	/* set these things appropaite */
		m->iFilterNotify = MF_GlobalConfiguration.filterList[(unsigned int)(m->iFilterHandle-1)].notify;
		m->iFilterAction = MF_GlobalConfiguration.filterList[(unsigned int)(m->iFilterHandle-1)].action;
	}
	
	_mfd_free(toScan,"FilterCheckSBuf");
	return rc;
}

int MF_RuleExec_RE(const char* expression, char* scan)
{
	int rc = 0;
	int scanLen = 0;
	pcre *re;
	int ovector[OVECCOUNT];
	const char *error;
	int erroffset;
	int iResult = 2;
	
	char *toScan = _mfd_nt_strdup(scan,"RuleExec_RESBuf");
	scanLen = (int)strlen(toScan);
	strlwr(toScan);

//	MFD_Out("compiling %s\n",expression);

	rc = -99;
	re = pcre_compile(
	  expression, /* the pattern */
	  0,                    /* default options */
	  &error,               /* for error message */
	  &erroffset,           /* for error offset */
	  NULL);                /* use default character tables */

	if (re == NULL)
	{
		
		MFD_Out(MFD_SOURCE_REGEX,"RegExp compilation failed at offset %d: %s\n", erroffset, error);
		
	} else {
		rc = pcre_exec(
		  re,                   /* the compiled pattern */
		  NULL,                 /* no extra data - we didn't study the pattern */
		  toScan,              	/* the subject string */
		  scanLen,				/* the length of the subject */
		  0,                    /* start at offset 0 in the subject */
		  0,                    /* default options */
		  ovector,              /* output vector for substring information */
		  OVECCOUNT);           /* number of elements in the output vector */
		  
		pcre_free(re);
	}

	if (rc < 0)
	{
	    if (rc == PCRE_ERROR_NOMATCH)
	    	iResult = 0;
	    	else
	    	MFD_Out(MFD_SOURCE_REGEX,"(RegExp) Matching error %d\n", rc); /* error... */
	} else {
		iResult = 1;
	}

	_mfd_nt_free(toScan,"RuleExec_RESBuf");

	return iResult;
}

// Checks the passed String for block ...
// * m: the mail data
// > Returns: 1 = success
int MF_RuleExec( MailFilter_MailData* m )
{

	int curItem = 0;
	bool bOverrideErrorMessage = false;
	
	int iResult = 0;
	long lVal;
	char* szFieldDescription;

	int iIgnoreFields = 0;

	MFRelayHost *bl;
	if (m->bHaveReceivedFrom)
	{	bl = new MFRelayHost(m->szReceivedFrom); }
	else
	{	bl = new MFRelayHost(""); }

	iXList_Storage* att;
	att = m->lstAttachments->GetFirst();
	while (att != NULL)
	{
		if (((char*)att->data)[0]==0)
			break;
			
//MFD_Out("att: %s\n",(char*)att->data);
		
		att = m->lstAttachments->GetNext(att);
	}

	for (curItem = 0; curItem < MF_GlobalConfiguration.filterList.size(); curItem++)
	{
		iResult = 2;

		if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression == "")
			break;
//MFD_Out("ck:%i",curItem);
	if (
			(MF_GlobalConfiguration.filterList[(unsigned int)curItem].enabled == true) &&
			(
			((m->iMailSource == MAILFILTER_MAILSOURCE_INCOMING) && (MF_GlobalConfiguration.filterList[(unsigned int)curItem].enabledIncoming == true))
			||
			((m->iMailSource == MAILFILTER_MAILSOURCE_OUTGOING) && (MF_GlobalConfiguration.filterList[(unsigned int)curItem].enabledOutgoing == true))
			)
	) {

		if ( (bOverrideErrorMessage == true) && (szFieldDescription != NULL) )
			_mfd_free(szFieldDescription,"szFieldDescription (Auto)");
		bOverrideErrorMessage = false;
		
//MFD_Out("..");
		switch (MF_GlobalConfiguration.filterList[(unsigned int)curItem].matchfield)
		{
		case MailFilter_Configuration::always:
			// this rule matches without any other filters
			szFieldDescription = "Always";
			iResult = 1;
			break;
		case MailFilter_Configuration::email:
//			if (chkFlag(iIgnoreFields,MAILFILTER_MATCHFIELD_EMAIL)) break;
			szFieldDescription = "E-Mail address";

			if (m->szMailFrom[0]!=0)						 {	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailFrom);		szFieldDescription = "Sender's E-Mail";	}
			if (m->szEnvelopeFrom[0]!=0)	if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szEnvelopeFrom);	szFieldDescription = "Sender's E-Mail";		}
			if (m->szMailRcpt[0]!=0)		if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailRcpt);		szFieldDescription = "Recipients's E-Mail";	}
			if (m->szEnvelopeRcpt[0]!=0)	if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szEnvelopeRcpt);	szFieldDescription = "Recipients's E-Mail";	}

			break;
		case MailFilter_Configuration::emailBothAndCC:
//			if (chkFlag(iIgnoreFields,MAILFILTER_MATCHFIELD_EMAIL)) break;
			szFieldDescription = "E-Mail address";

			if (m->szMailFrom[0]!=0)						 {	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailFrom);		szFieldDescription = "Sender's E-Mail";	}
			if (m->szEnvelopeFrom[0]!=0)	if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szEnvelopeFrom);	szFieldDescription = "Sender's E-Mail";		}
			if (m->szMailRcpt[0]!=0)		if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailRcpt);		szFieldDescription = "Recipients's E-Mail";	}
			if (m->szEnvelopeRcpt[0]!=0)	if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szEnvelopeRcpt);	szFieldDescription = "Recipients's E-Mail";	}
			if (m->szMailCC[0]!=0)			if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailCC);		szFieldDescription = "CC's E-Mail";			}

			break;
		case MailFilter_Configuration::emailFrom:
//			if (chkFlag(iIgnoreFields,MAILFILTER_MATCHFIELD_EMAIL_FROM)) break;
			szFieldDescription = "Sender's E-Mail";

			if (m->szMailFrom[0]!=0)						 {	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailFrom);		}
			if (m->szEnvelopeFrom[0]!=0)	if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szEnvelopeFrom);	}

			break;
		case MailFilter_Configuration::emailTo:
//			if (chkFlag(iIgnoreFields,MAILFILTER_MATCHFIELD_EMAIL_TO)) break;
			szFieldDescription = "Recipients's E-Mail";

			if (m->szMailRcpt[0]!=0)						 {	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailRcpt);		}
			if (m->szEnvelopeRcpt[0]!=0)	if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szEnvelopeRcpt);	}

			break;
		case MailFilter_Configuration::emailToAndCC:
//			if (chkFlag(iIgnoreFields,MAILFILTER_MATCHFIELD_EMAIL_TO)) break;
			szFieldDescription = "Recipients's E-Mail";

			if (m->szMailRcpt[0]!=0)						 {	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailRcpt);		}
			if (m->szEnvelopeRcpt[0]!=0)	if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szEnvelopeRcpt);	}
			if (m->szMailCC[0]!=0)			if (iResult != 1){	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailCC);			szFieldDescription = "CC's E-Mail";			}

			break;
		case MailFilter_Configuration::subject:
//MFD_Out("S");
//			if (chkFlag(iIgnoreFields,MAILFILTER_MATCHFIELD_SUBJECT)) break;
			szFieldDescription = "Subject";

			if (m->szMailSubject[0]!=0)						 {	iResult = 0;	iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),m->szMailSubject);		}
			
			break;
		case MailFilter_Configuration::size:
//MFD_Out("Z");
//			if (chkFlag(iIgnoreFields,MAILFILTER_MATCHFIELD_SIZE)) break;
			szFieldDescription = "Mail Size";
			lVal = 0;
			lVal = atol(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str()+1);
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression[0] == '>')
				if (lVal > m->iMailSize)	
					iResult = 1;

			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression[0] == '<')
				if (lVal < m->iMailSize)	
					iResult = 1;
				
			break;
		case MailFilter_Configuration::attachment:
			att = m->lstAttachments->GetFirst();
			while (att != NULL)
			{
				if (((char*)att->data)[0]==0)
					break;

				iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),(char*)att->data);	szFieldDescription = "Attachment Name";

				if (iResult == 1)
					break;

				att = m->lstAttachments->GetNext(att);
			}
			break;
		case MailFilter_Configuration::archiveContentName:
			bool bIsEncryptedCheck = (MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression == "<encrypted file>");
			att = m->lstArchiveContents->GetFirst();
			while (att != NULL)
			{
				if (((char*)att->name)[0]==0)
					break;

				if (!bIsEncryptedCheck)
				{
					iResult = MF_RuleExec_RE(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),att->name);	szFieldDescription = "Archive Attachment";
				} else {
					iResult = (((bool)att->data == true) ? 1 : 0);
				}

				if (iResult == 1)
					break;

				att = m->lstArchiveContents->GetNext(att);
			}
			break;
		case MailFilter_Configuration::archiveContentCount:
			int cmpCount = atoi(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str());
			int effectiveCount = 0;
			att = m->lstArchiveContents->GetFirst();
			while (att != NULL)
			{
				if (((char*)att->name)[0]==0)	break;
				++effectiveCount;
				att = m->lstArchiveContents->GetNext(att);
			}

			if (effectiveCount == cmpCount)
			{
				iResult = 1;
				break;
			} else
				iResult = 0;

			break;
		case MailFilter_Configuration::blacklist:
		
			char* holeZone = _mfd_strdup(MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str(),"holeZone");
			char* validResponse = strchr(holeZone,':');
			if (validResponse == NULL) 
			{   // this was the default in previous versions
				validResponse = "";
			} else {
				validResponse[0] = 0;
				validResponse++;
			}
					
			bOverrideErrorMessage = true;
			szFieldDescription = (char*)_mfd_malloc(1001,"szFieldDescription");
			sprintf(szFieldDescription,"Found a blacklisted mailserver address. (RBL: %s)",holeZone);
			
			if( bl->LookupRBL_DNS(holeZone,validResponse) == 1 )	{ iResult = 1; } else { iResult = 0; }
			_mfd_free(holeZone,"holeZone");

		    break;
		case MailFilter_Configuration::ipUnresolvable:
		
/*		
			} else {
				// this is a dirty hack!
				// if we cannot test, we have to pretend everything's fine - also for the reverse
				// matching type
				//
				iResult = 0;
				if (MF_GlobalConfiguration.filterList[curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
				{	iResult = 1;	}
			}						  */
			

			bOverrideErrorMessage = true;
			szFieldDescription = (char*)_mfd_malloc(1001,"szFieldDescription");
			sprintf(szFieldDescription,"Found an unknown/unresolvable relay host address.");
			
			if( bl->LookupDNS() == 2 )	{ iResult = 1; } else { iResult = 0; }
			
		    break;
		default:
			MFD_Out(MFD_SOURCE_RULE,"MF_RuleExec encountered unknown field %d!\n",MF_GlobalConfiguration.filterList[(unsigned int)curItem].matchfield);
			break;
		}
		
//MFD_Out("%i",iResult);
		if (iResult != 2)
		{
		
		if (iResult == 0)
		{	// no match
    		// if this is a NO match rule, then the filter applies and we break the cycle...
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
			{
				iResult = 1;
			}
				    	
		} else {
			// Match Applies.
    		// if this is a NO match rule, then the filter DOES NOT apply
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
			{
					iResult=0;
			}
		}

		if (iResult)
		{
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].action == MAILFILTER_MATCHACTION_COPY)
			{
				m->bCopy = true;
				iResult = 0;
				m->lstCopies->AddValueChar("", MF_GlobalConfiguration.filterList[(unsigned int)curItem].name.c_str());
				
			}
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].action == MAILFILTER_MATCHACTION_SCHEDULE)
			{
				m->bSchedule = true;
				iResult = 0;
			}
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].action == MAILFILTER_MATCHACTION_NOSCHEDULE)
			{
				m->bSchedule = false;
				iResult = 0;
			}
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].action == MAILFILTER_MATCHACTION_PASS)
			{
//				MFD_Out("Ignore: %x -> %x\n",iIgnoreFields,iIgnoreFields|MFC_Filters[(unsigned int)curItem].matchfield);
				iIgnoreFields = iIgnoreFields | MF_GlobalConfiguration.filterList[(unsigned int)curItem].matchfield;
				iResult = 0;
			}
		}
//MFD_Out("%i",iResult);
		
		if (iResult)
		{
//MFD_Out("..break\n");
			break;
		}
		
		}
//MFD_Out("\n");

	}
		
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
	}
	
	if (bl != NULL)
	{
		delete(bl);
	}

MFD_Out(MFD_SOURCE_RULE,"\n->%i %u",iResult,iResult);
	if (iResult == 2)
	{
		iResult = 0;
		curItem = -2;
	}
	
	if (iResult == 1)
	{	/* set these things approaite */
		m->bFilterMatched = true;
		m->iFilterHandle = curItem+1;
		m->iFilterNotify = MF_GlobalConfiguration.filterList[(unsigned int)curItem].notify;
		m->iFilterAction = MF_GlobalConfiguration.filterList[(unsigned int)curItem].action;
MFD_Out(MFD_SOURCE_RULE,"%s",MF_GlobalConfiguration.filterList[(unsigned int)curItem].name.c_str());

		if (!bOverrideErrorMessage)
		{
			if (MF_GlobalConfiguration.filterList[(unsigned int)curItem].name == "")
				sprintf(m->szErrorMessage," Match: %s: \"%s\"",szFieldDescription,MF_GlobalConfiguration.filterList[(unsigned int)curItem].expression.c_str());
				else
				sprintf(m->szErrorMessage," Match: %s: %s",szFieldDescription,MF_GlobalConfiguration.filterList[(unsigned int)curItem].name.c_str());
		} else {
			strncpy(m->szErrorMessage,szFieldDescription,999);
		}
			
		m->szErrorMessage[1000] = 0;
		MF_StatusText(m->szErrorMessage);
			
	}

	if (bOverrideErrorMessage)
	{
		// If the error message gets overriden, we define, that the memory has been malloc'd.
		// Therefore, we free it here.
		_mfd_free(szFieldDescription,"szFieldDescription");
	}

MFD_Out(MFD_SOURCE_RULE,"\n");	
	return 1;
}

// Moves a File
// * fileIn:			INPUT filename
// * workingFile:		OUTPUT filename
// * beVerbose:			1 = print messages to log/window, 0 = act silent
int MF_MoveFileToFile(const char *fileIn, const char *workingFile, int beVerbose)
{
	int rc;
#if defined(N_PLAT_NLM) && (!defined(__NOVELL_LIBC__))
	int fSource;
	int fDest;
	LONG numBytesCopied;
	LONG numBytesToCopy;

	fSource = -1;
	fDest = -1;
#endif
	rc = -1;

	unlink ( workingFile );


#ifdef N_PLAT_NLM
#ifdef __NOVELL_LIBC__
		errno=0;
		MFD_Out(MFD_SOURCE_WORKER,"%s -> %s\n",fileIn,workingFile);
		rc = rename ( fileIn , workingFile );
		MFD_Out(MFD_SOURCE_WORKER,"errcode: %d, %d %s\n",rc,errno,strerror(errno));
#else
	if (MFT_InOutSameServerVolume)
	{
		rc = rename ( fileIn , workingFile );
	} else {

		fSource	= open(		fileIn		, O_RDONLY | O_BINARY );
		if (fSource == -1)
		{
			if (beVerbose)	MF_StatusText("MoveFile: Error opening Source File!");
			return 240;
		}

		fDest	= creat(	workingFile	, 0 );
		if (fDest == -1)
		{
			close(fSource);
			if (beVerbose)	MF_StatusText("MoveFile: Error creating Destination File!");
			return 240;
		}

		numBytesToCopy = filelength (fSource);

		rc = FileServerFileCopy (
			fSource,
			fDest,
			0x00,
			0x00,
			numBytesToCopy,
			&numBytesCopied);

		close(fSource);
		close(fDest);

		if (numBytesCopied < numBytesToCopy)
		{
			if (beVerbose)	MF_StatusText("MoveFile: API did not copy all data!?");
			return 255;
		}
		
		if (rc == 0) rc=unlink(fileIn);
			else	 rc=unlink(workingFile);
		
	}

#endif
#endif

#ifdef WIN32		// Windows makes it easy to copy a file ;)  --  proceed if file already exists == FALSE
		CopyFile ( fileIn , workingFile , FALSE );
		unlink ( fileIn );
		rc = 0;
#endif

	if ( rc != 0 )
	{
		if (beVerbose)	MF_StatusText("MoveFile: Something went wrong. May be removing the source file?");
		rc = 240;
	}

	return rc;

}

//
// Moves a file to a directory.
//
// ***** Hatte Fehler - hat workingFile nicht freigegebeben.
// ***** Im MF_Run() hat dies allerdings ohne Fehler funktioniert ???
//
// fileIn:		Input File
// destDir:		Destination Directory
// beVerbose:	See MoveFileToFile
//
int MF_MoveFileToDir(const char *fileIn, const char *destDir, int beVerbose)
{
	int rc;

	char* workingFile;
	workingFile = (char*)_mfd_malloc(MAX_PATH,"MoveFile2Dir");

#ifndef __NOVELL_LIBC__
	char* pFilename;
	char* pExtension;

	pExtension = (char*)_mfd_malloc(MAX_PATH,"MoveFile2Dir");
	pFilename = (char*)_mfd_malloc(MAX_PATH,"MoveFile2Dir");

	if (workingFile == NULL)		{ MF_OutOfMemoryHandler();	return 239; }
	if (pExtension == NULL)			{ MF_OutOfMemoryHandler();	return 239; }
	if (pFilename == NULL)			{ MF_OutOfMemoryHandler();	return 239; }

	_splitpath(fileIn,NULL,NULL,pFilename,pExtension);
	sprintf(workingFile,"%s%s%s",destDir,pFilename,pExtension);

	rc = MF_MoveFileToFile(fileIn, workingFile, beVerbose);

	_mfd_free(pFilename,"MoveFile2Dir");
	_mfd_free(pExtension,"MoveFile2Dir");
#else
    char server[48+1], volume[256+1], directory[512+1],
    filename[256+1], extension[256+1];
    int elements, flags;
      
    deconstruct(fileIn, server, volume, directory, filename, extension, &elements, &flags);
	sprintf(workingFile,"%s%s",destDir,filename);

	rc = MF_MoveFileToFile(fileIn, workingFile, beVerbose);
#endif

	_mfd_free(workingFile,"MoveFile2Dir");
	return rc;
}
	   
static int MFVS_PassMailToVirusScan(MailFilter_MailData* m)//const char* fileName, const char* fileIn, const char* fileOut, char* szScanDir)
{
	char szScanFile[MAX_PATH];

	if (!MFC_MAILSCAN_Enabled)
		return 0;		// Success...
	
	//
	// Initiate Virus Scanning:

	sprintf(szScanFile,"%sMAILFLT.MFS",m->szScanDirectory);

	if (MailFilter_MailWrite(szScanFile,m) == 0)
		return 2;
		else
		return 0;

	// More is in the worker thread main routine ... ("Scan the SCAN directory...")

}

static char* MF_MakeValidHeaderEmail(char* szEmail, unsigned int iBufSize)
{
	char* szTemp = (char*)_mfd_malloc(iBufSize,"MakeValidHeaderEmail");
	unsigned int iBegin;
	unsigned int pos;

	for(iBegin=0;iBegin<strlen(szEmail);iBegin++)
	{	
		if (szEmail[iBegin] == '<') { iBegin++; break; }
	}
	
	if (iBegin == strlen(szEmail))
	{	// Nothing Found!
		iBegin = 0;
	}

	for(pos=iBegin+1;pos<=strlen(szEmail);pos++)
	{	if (szEmail[pos] == '>') break;
		if (szEmail[pos] == ',') break;
		if (szEmail[pos] == '\r') break;
		if (szEmail[pos] == '\n') break;
		if (szEmail[pos] == '\0') break;
	}

	szTemp[0]='<';
	szTemp[1]= 0 ;

	strncpy(szTemp+1,szEmail+iBegin,pos-iBegin);	//
	szTemp[pos-iBegin+1]=0;
	
	strcat(szTemp,">");	

MFD_Out(MFD_SOURCE_WORKER,"Made %s\n",szTemp);
	
	return szTemp;
}



//
// Examines Mail Files ...
//
static int MF_ExamineFile(MailFilter_MailData* m)
{

	FILE *mailFile				= NULL;
	int q						= 255;
	int curChr					= 0;
	int curPos					= 0;
	int curCmpPos				= 0;
	int iFirstNonSpaceCharacter = 0;
	long mynewfilterhandle		= 0;
	char *pszFirstNonSpaceCharacter;
	
	int filterNotify			= MAILFILTER_NOTIFICATION_NONE;
	int filterAction			= MAILFILTER_MATCHACTION_NONE;

	char* szThisAttachment		= NULL;	
	char* szScanBuffer			= NULL;
	char* szCmpBuffer			= NULL;
	bool  bNextLineStartSpace	= false;
	char  cNextLineStart		= 0;

	long iThisAttSize = 0;

	bool bDecodeDone = false;
	bool bDecodeExec = false;
	bool bInMimeAttachment = false;
	bool bRelayChecked = false;
//	MimeDecoderData* mimeDecoder = NULL;
	
	mailFile = fopen(m->szFileWork,"rb");
	if (mailFile == NULL)
	{
		MF_StatusText("Examine: Could not open file!");
		return 240;
	}

#define _EXAMINEFILE_INITSTRING(szString,size)		if (szString == NULL) { szString=(char*)_mfd_malloc(size+2,"ExaSBuf"); if (szString == NULL) return 239; } memset(szString,0,size);
	_EXAMINEFILE_INITSTRING (	szScanBuffer 		,	2002 );
	_EXAMINEFILE_INITSTRING (	szCmpBuffer			,	2502 );
	_EXAMINEFILE_INITSTRING (	szThisAttachment 	,	300  );

	sprintf(szScanBuffer,"STRING INIT");
	// go to top of file
	fseek(mailFile,0,SEEK_SET);

#undef _EXAMINEFILE_INITSTRING

	while (q == 255)
	{
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		if ( feof(mailFile) )
		{	q=0;	break;	}

		// reinit variables
		szScanBuffer[0]=0;
		iFirstNonSpaceCharacter = 0;

		// Get Line ...
		curPos = -1;
		do {
			curChr = 0;
			szScanBuffer[2000] = 0;
			
			if (bNextLineStartSpace)
			{
//			MFD_Out(MFD_SOURCE_MAIL,"bNLSS ");
				while ( (curChr = fgetc(mailFile) ) < 20 )
				{
					if (curChr == 0x9)
						curChr = 0x20;

					if (curChr == -1)
					{
						curChr = 0;
						break;
					}
						
					if (curChr != 0x20)
						curChr = 0;

#if defined(__NOVELL_LIBC__)
		NXThreadYield();
#else
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
#endif
				}
								
				if ( !feof(mailFile) )
				if (curChr != 0)
					ungetc(curChr,mailFile);
				
				ungetc(0x20,mailFile); // push back in a space!
					
//				MFD_Out(MFD_SOURCE_MAIL,"got %d\n",curChr);
			} else {
				if (bInMimeAttachment)
				{
					curChr = fgetc(mailFile);
					ungetc(curChr,mailFile);
					MFD_Out(MFD_SOURCE_MAIL,"bInMimeAttachment == TRUE; next char: %d (='%c')\n",curChr,curChr);
					if (curChr != 'C')
					{
					
						if (MF_GlobalConfiguration.EnableAttachmentDecoder)
						{
							if ( !feof(mailFile) )
								ungetc('\n',mailFile);	// go one back for decode()
							// Extract Attachment
							iThisAttSize = 0;
							m->iNumOfAttachments++;
							
							char szNamFile[MAX_PATH]; sprintf(szNamFile,"%s%i.mfn",m->szScanDirectory,m->iNumOfAttachments);
							char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments);
							char szZipFile[MAX_PATH]; sprintf(szZipFile,"%sFiles.zip",m->szScanDirectory);

							
							FILE* namFile = fopen(szNamFile,"wt");
							if (namFile != NULL)
							{
								fprintf(namFile,"%s\n",szThisAttachment);
								fclose(namFile);
							} else {
								MFD_Out(MFD_SOURCE_MAIL, "Unable (%d) to fOpen %s!\n",errno,szNamFile);
								MF_StatusText("Error while decompressing file.");
							}
							
							iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingBase64);
							
							if (iThisAttSize > 0)
							{
								m->iTotalAttachmentSize += iThisAttSize;
								MFVS_CheckAttachment(szAttFile,szThisAttachment,m);
							}
								
							MFD_Out(MFD_SOURCE_VSCAN,"New Total Size: %d (increased by %d)\n",m->iTotalAttachmentSize,iThisAttSize);

#ifdef MF_WITH_ZIP
							MFZip* zip = new MFZip( szZipFile , 9 , 0 );
							zip->AddFile( szThisAttachment , szAttFile );
							delete(zip);							
#endif
						}
								
						bInMimeAttachment = false;
					}
				}
			}
//MF_DisplayCriticalError((" XNLX");
			while ((curChr != '\n') && (curChr != -1))
			{
				if (curPos >= 2000) break;
				curChr = fgetc(mailFile);

//	MF_DisplayCriticalError((" %c[%d/%d] ",curChr,curChr,curPos);
				curPos++;

				if ((curChr == '\n') || (curChr == -1))
					break;
				if ( curChr == 0x9 )				// Tab to Single Space
					curChr = 0x20;
				if ( (curChr != '\r') && (curChr != '\n'))
					szScanBuffer[curPos]=(char)curChr;

#if defined(__NOVELL_LIBC__)
		NXThreadYield();
#else
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
#endif
			}
			
			if (curPos>0)
				curPos--;
				else
				curPos=0;

			szScanBuffer[curPos]=0;
			szScanBuffer[2000]=0;

			
			do
			{
				cNextLineStart = (char)fgetc(mailFile);
					
				if (cNextLineStart == -1)
					break;
					
					
			} while ( ( cNextLineStart == '\n' ) || ( cNextLineStart == '\r' ) );

if ( feof(mailFile) )
{	q=0;	break;	}

			// put char back...
			if (cNextLineStart != -1)
				ungetc(cNextLineStart,mailFile);

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

//MF_DisplayCriticalError((" XNLS:%d ",bNextLineStartSpace);
			
			if (bNextLineStartSpace)
				curPos--;
			
		} while (bNextLineStartSpace && (curPos<2000));
		szScanBuffer[2000]=0;
		//MFD_Out(MFD_SOURCE_MAIL,"(%d):%s\n",strlen(szScanBuffer),szScanBuffer);
		//for (int i = 0; i < 10; i++)
		//	MFD_Out(MFD_SOURCE_SMTP,"[%d/%d] ",szScanBuffer[i],i);


		// looks confusing, it is! some things below check the first character of the next line...
		cNextLineStart = (char)fgetc(mailFile);
		ungetc(cNextLineStart,mailFile);

		char *szMimeDest;
#define _EXAMINEFILE_DOMIMEHDRDECODE(szBuffer,iLen) szMimeDest = MIME_DecodeMimeHeader(szBuffer); if (szMimeDest != NULL) { strncpy(szBuffer,szMimeDest,iLen); _mfd_free(szMimeDest,"ExaMimeDecode"); szMimeDest = NULL; szBuffer[iLen-1]=0; }

		switch (szScanBuffer[0])
		{
			case '-':
					bInMimeAttachment = false;
				break;
			case 'm':
			case 'M':
//				if (!m->bHaveFrom)	/* MAIL FROM: Filter - bHaveFrom wird nur =true gesetzt, wenn der Filter matcht, da dieses Feld nicht */
//				{					/* immer vorhanden ist, bzw. gültigen Inhalt hat (MAX!->admin@domain.com) */
					if ( memicmp(szScanBuffer,"mail from:",10) == 0 )
					{
						m->szEnvelopeFrom[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+10);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szEnvelopeFrom		, pszFirstNonSpaceCharacter , 245 );
						m->szEnvelopeFrom[245]=0;
						
						_EXAMINEFILE_DOMIMEHDRDECODE(m->szEnvelopeFrom,249);

					}
//				}
				break;
			case 'f':
			case 'F':
				if (!m->bHaveFrom)
				{
					if ( memicmp(szScanBuffer,"from:",5) == 0 )
					{
						m->bHaveFrom = true;
						m->szMailFrom[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+5);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailFrom , pszFirstNonSpaceCharacter , 245 );
						m->szMailFrom[245]=0;
						
						_EXAMINEFILE_DOMIMEHDRDECODE(m->szMailFrom,249);

					}
				}
				break;
			case 'r':
			case 'R':
//				if (!m->bHaveRcpt)		/* RCPT TO: Filter - bHaveRcpt wird nur =true gesetzt, wenn der Filter matcht, da dieses Feld nicht */
//				{					/* immer vorhanden ist, bzw. gültigen Inhalt hat */
					if ( memicmp(szScanBuffer,"rcpt to:",8) == 0 )
					{
						m->szEnvelopeRcpt[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+8);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szEnvelopeRcpt		, pszFirstNonSpaceCharacter , 245 );
						m->szEnvelopeRcpt[245]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(m->szEnvelopeRcpt,249);

					}
//				}

				/* also copy the (last) received: from header over */
				if ( ( memicmp(szScanBuffer,"received: from ",15) == 0 ) && ((!m->bHaveReceivedFrom) || (MF_GlobalConfiguration.EnablePFAFunctionality)) )
				{
//TODO
					// look for ip first
					// "agw.kmsv.de (asdf [213.69.145.18])"
					bool bFound = false;
					int iPos = 15;	// 15 = len("received from")
					// find [
					for (; iPos < 245; iPos++)
					{
						if (szScanBuffer[iPos] == '[')
						{
							++iPos;
							bFound = true;
							break;
						}
					}
	
					if (!bFound)
						iPos = 15; 		// ok, we didn't find an ip in [brackets], but we still can try the
										// reported name.
					
					// copy next 60 chars
					strncpy(szCmpBuffer,szScanBuffer+iPos,60);
					szCmpBuffer[59] = 0;

					// find ] and terminate string there.
					iFirstNonSpaceCharacter = 0;
					while ( szCmpBuffer[iFirstNonSpaceCharacter] != 0 )
					{
						if ( 
							(szCmpBuffer[iFirstNonSpaceCharacter] == ']') ||
							(szCmpBuffer[iFirstNonSpaceCharacter] == ')') ||
							(iscntrl(szCmpBuffer[iFirstNonSpaceCharacter])) || 
							(isspace(szCmpBuffer[iFirstNonSpaceCharacter]))
							)
						{
							szCmpBuffer[iFirstNonSpaceCharacter] = 0;
							break;
						}
						iFirstNonSpaceCharacter++;
						if (iFirstNonSpaceCharacter>59)
							break;
					}

					if (szCmpBuffer[0])
					{
						
						if (MF_GlobalConfiguration.EnablePFAFunctionality)
						{	
							/*
							 * Ok, now this is just for the f*cking Novell PFA
							 * It always writes: Received from: pfAgent blah...
							 * into the mail. this just suxxs for our DNS-RBL checking
							 * ==> skip over it (well, skip the first received from)
							 */
							if (!m->bHaveReceivedFrom)
							{
								m->bHaveReceivedFrom = true;
								strcpy(m->szReceivedFrom,"PFA");	// mark that we are entering this the first time
							} else {
								if (strncmp("PFA",m->szReceivedFrom,3) == 0)
								{
									strncpy( m->szReceivedFrom		, szCmpBuffer , 245 );					
									m->bHaveReceivedFrom = true;
									
									MFD_Out(MFD_SOURCE_MAIL,"Received from '%s'\n",m->szReceivedFrom);
								}
							}
						} else {
						
							strncpy( m->szReceivedFrom		, szCmpBuffer , 245 );					
							m->bHaveReceivedFrom = true;
							
							MFD_Out(MFD_SOURCE_MAIL,"Received from '%s'\n",m->szReceivedFrom);
						}						
					}
				}
				break;
			case 't':
			case 'T':
				if (!m->bHaveRcpt)
				{
					if ( memicmp(szScanBuffer,"to:",3) == 0 )
					{
						m->bHaveRcpt = true;
						m->szMailRcpt[0]=0;
						
						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+3);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailRcpt , pszFirstNonSpaceCharacter , 245 );
						m->szMailRcpt[245]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(m->szMailRcpt,249);
					}
				}
				break;
/*			case 'c':
			case 'C':
				// CC: is handled below...
				break;*/
			case 's':
			case 'S':
				if (!m->bHaveSubject)
				{
					if ( memicmp(szScanBuffer,"subject:",8) == 0 )
					{
						m->bHaveSubject = true;
						m->szMailSubject[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+8);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailSubject , pszFirstNonSpaceCharacter , 1998 );
						m->szMailSubject[1999]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(m->szMailSubject,1999);
					}
				}
				break;
			case '*':
				if (memicmp(szScanBuffer,"** high priority **",19) == 0)
					m->iPriority = 1;
					else if (memicmp(szScanBuffer,"** low priority **",18) == 0)
						m->iPriority = -1;
				break;
			case 'b':
			case 'B':
				if ((memicmp(szScanBuffer,"begin",5) == 0) && (strlen(szScanBuffer) > 10))
				{
					szCmpBuffer[0]=0;
					
					strncpy(szCmpBuffer,szScanBuffer+9,245);
					szCmpBuffer[245]=0;
MFD_Out(MFD_SOURCE_MAIL,"UU Attachment: '%s'\n",szCmpBuffer);
		
					strncpy(szThisAttachment,szCmpBuffer,250);
					m->lstAttachments->AddValueChar("UUEncode", szCmpBuffer);

/*
	MFVS_WriteOutAttachment
	(int (*output_fn) (const char *buf,  int size,  void *closure),
									void *closure);	
*/								
					if (MF_GlobalConfiguration.EnableAttachmentDecoder)
					{
						// Extract Attachment
						m->iNumOfAttachments++;
						char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments);
						iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingUUEncode);
						if (iThisAttSize > 0)
						{
//CHANGE
							m->iTotalAttachmentSize += iThisAttSize;
							MFVS_CheckAttachment(szAttFile,szCmpBuffer,m);
						}
					}
				}
				break;
			case 'c':
			case 'C':
				if (!m->bHaveCC)
				{
					if ( memicmp(szScanBuffer,"cc:",3) == 0 )
					{
						m->bHaveCC = true;
						m->szMailCC[0]=0;
						
						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+3);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailCC , pszFirstNonSpaceCharacter , 245 );
						m->szMailCC[245]=0;

						break;
					}
				}
				if (memicmp(szScanBuffer,"content-",8) == 0)
				{
	
					curChr = 0;
					curPos = 8;
					if (memicmp(szScanBuffer+curPos,"disposition: attachment;",23) == 0)
					{

	MFD_Out(MFD_SOURCE_MAIL,"content-disposition: attachment\n");
							curPos = curPos+24;
					
							// example: Disposition: attachment; filename="xxxxxx.exe"
							// Liest die Zeichen bis 'name=' ( also bis '=' )...
							curChr = 1;
							while ((!feof(mailFile)) && (curChr != '=') && (curChr != '\r') && (curChr != '\n') && (curPos < 2000) && (curChr != '\0'))
							{
								curChr = szScanBuffer[curPos];
								curPos++;
							}

#ifdef N_PLAT_NLM
							ThreadSwitch();
#endif
							if ( (curChr == '\r') || (curChr == '\n') )
							{	
							MFD_Out(MFD_SOURCE_MAIL,"Break on \\r | \\n \n");
								break;
							}

							// is this is a filename ?
							if (memicmp(szScanBuffer+curPos-5,"name=",5) == 0)
							{

								szCmpBuffer[0] = 1;
								szCmpBuffer[1] = 0;
								curCmpPos = 0;

								if ( szScanBuffer[curPos] == '"' )	 //"
									curPos++;

#ifdef N_PLAT_NLM
								ThreadSwitch();
#endif
								do
								{

									szCmpBuffer[curCmpPos] = szScanBuffer[curPos];
									if (curPos > 2000)
									{
										// Abbruch wenn Attachment Name > Buffer
										strcpy(m->szErrorMessage,"Attachment Name was too long to fit into buffer.");
										m->iFilterNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
										m->iFilterAction = MAILFILTER_MATCHACTION_DROP_MAIL;
										m->bFilterMatched = true;
										break;
									}			
									
									if ((szCmpBuffer[curCmpPos] == '\r') || (szCmpBuffer[curCmpPos] == '\n') || ( szCmpBuffer[curCmpPos] == '=' ) || ( szCmpBuffer[curCmpPos] == '\0' ))
										break;

									curCmpPos++;
									curPos++;

									szCmpBuffer[curCmpPos] = '*';
									szCmpBuffer[curCmpPos+1] = 0;

								}
								while ((szCmpBuffer[curCmpPos] != '\n') && (szCmpBuffer[curCmpPos] != '\r') && (szCmpBuffer[curCmpPos] != '=') && (curPos < 2000) && (szCmpBuffer[curCmpPos] != '\0'));

								if (curCmpPos > 0)
								{
								
									curCmpPos--;
									curPos--;

									// Now we have a full attachment name.
									szCmpBuffer[curCmpPos+1]=0;
									if ( szCmpBuffer[curCmpPos] == '"' )	  //"
										szCmpBuffer[curCmpPos]=0;

	MFD_Out(MFD_SOURCE_MAIL,"--> ATT '%s'\n",szCmpBuffer);
									strncpy(szThisAttachment,szCmpBuffer,250);
									szThisAttachment[250] = 0;
	//DEBUG

									m->lstAttachments->AddValueChar("Base64", szCmpBuffer);
									//_EXAMINEFILE_DOFILTERCHECK(szCmpBuffer,"Attachment Name",MAILFILTER_MATCHFIELD_ATTACHMENT);

	/*
		MFVS_WriteOutAttachment
		(int (*output_fn) (const char *buf,  int size,  void *closure),
										void *closure);	
	*/								
									bInMimeAttachment = true;
	/*								if (MFC_EnableAttachmentDecoder)
									{
										// Extract Attachment
										iThisAttSize = 0;
										m->iNumOfAttachments++;
										char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments);
										iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingBase64);
										if (iThisAttSize > 0)
										{
											m->iTotalAttachmentSize += iThisAttSize;
											MF_CheckWinmailDat(szAttFile,szThisAttachment,m);
										}
									}*/
								} else {
									MFD_Out(MFD_SOURCE_GENERIC,"WAH: could not decode c-d:a filename= filename correctly. maybe empty or something like that.\n");
									MF_StatusText("  WARNING: could not decode attachment filename [A]");
								}
								
							} // name= matching

					} else {		// Disposition , Type

						if (memicmp(szScanBuffer+curPos,"type",4) == 0)
						{	
							// ex.: ": application/zip; name=\""
							curPos = 12;
							// we're looking for '='
							while ((!feof(mailFile)) && (curChr != '=') && (curChr != '\r') && (curChr != '\n') && (curPos < 2000))
							{
								curChr = szScanBuffer[curPos];
								curPos++;
							}
							ThreadSwitch();

							// is this is a filename ?
							if (memicmp(szScanBuffer+curPos-5,"name=",5) == 0)
							{
	MFD_Out(MFD_SOURCE_MAIL,"content-type name=\n");
	
								szCmpBuffer[0] = 0;
								curChr = 0;
								curCmpPos = 0;

								if ( szScanBuffer[curPos] == '"' )	// "
									curPos++;

								ThreadSwitch();
								while ((szCmpBuffer[curCmpPos] != '\n') && (szCmpBuffer[curCmpPos] != '\r') && (szCmpBuffer[curCmpPos] != '"')  && (szCmpBuffer[curCmpPos] != '=') && (curPos < 2000)) //"
								{
									szCmpBuffer[curCmpPos] = szScanBuffer[curPos];
									
									if (curPos > (MAX_PATH-2))
									{
										// Abbruch wenn Attachment Name > Buffer
										strcpy(m->szErrorMessage,"Attachment Name was too long to fit into buffer.");
										m->iFilterNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
										m->iFilterAction = MAILFILTER_MATCHACTION_DROP_MAIL;
										m->bFilterMatched = true;
										break;
									}
									
									if ((szCmpBuffer[curCmpPos] == '\0') || (szCmpBuffer[curCmpPos] == '\r') || (szCmpBuffer[curCmpPos] == '\n') || ( szCmpBuffer[curCmpPos] == '"' ))
										break;
// " //  <-- stupid highlighting in the IDE
									curCmpPos++;
									curPos++;

								}
								// Now we have a full attachment name.
								szCmpBuffer[curCmpPos]=0;
MFD_Out(MFD_SOURCE_MAIL,"--> TYPE '%s'\n",szCmpBuffer);

								strncpy(szThisAttachment,szCmpBuffer,250);
								szThisAttachment[250] = 0;
								_EXAMINEFILE_DOMIMEHDRDECODE(szThisAttachment,249);
MFD_Out(MFD_SOURCE_MAIL,"-=> TYPE '%s'\n",szThisAttachment);
								m->lstAttachments->AddValueChar("Base64", szCmpBuffer);

								bInMimeAttachment = true;
								
/*								if (MFC_EnableAttachmentDecoder)
								{
									// Extract Attachment
									m->iNumOfAttachments++;
									char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments);
									iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingBase64);
									if (iThisAttSize > 0)
									{
										m->iTotalAttachmentSize += iThisAttSize;
										MF_CheckWinmailDat(szAttFile,szThisAttachment,m);
									}
								}*/
								
							}	// name=
						}

						ThreadSwitch();
					}
				}

				// 'c' 'C'
				break;

			default:
				// Nothing ... 
				break;

		}	// end of switch()
		
		/* Break Codes (q):
		
		   q = 0 		...		EOF == Fertig!
		   q = 1		... 	Filter Match
		   q > 1 < 255  ...		Fehler !?
		
		*/
		
		// Terminate While Loop if q>0			** FEHLER **
		if ( ( q>0 ) && ( q<255 ) )
		{
			break;
		}
		
/*		if (m->bFilterMatched && m->bHaveSubject && m->bHaveRcpt && m->bHaveFrom)
		{
			q = 1;
			break;
		}*/

	}	// end of while

	fseek(mailFile,0,SEEK_END);
	m->iMailSize = ftell(mailFile);
	fclose(mailFile);

	MF_StatusLog("Applying rules.");
	MFD_Out(MFD_SOURCE_MAIL,"Applying rules.\n");
	MF_RuleExec(m);

	MF_StatusLog(" Mail Summary:");
	sprintf(szCmpBuffer,"  From: %s",m->szMailFrom);		MF_StatusLog(szCmpBuffer);
	sprintf(szCmpBuffer,"  Rcpt: %s",m->szMailRcpt);		MF_StatusLog(szCmpBuffer);
	sprintf(szCmpBuffer,"  Subject: %s",m->szMailSubject);	MF_StatusLog(szCmpBuffer);
	sprintf(szCmpBuffer,"  Mail Size: %d Byte",m->iMailSize);	MF_StatusLog(szCmpBuffer);

	if (m->szProblemMailDestination != NULL)
	{
		if ((m->iMailSource == 0) && (MF_GlobalConfiguration.DefaultNotification_InternalSender == 1))
			strncpy(m->szProblemMailDestination,m->szMailFrom,250);
		if ((m->iMailSource == 1) && (MF_GlobalConfiguration.DefaultNotification_InternalRecipient == 1))
			strncpy(m->szProblemMailDestination,m->szMailRcpt,250);
		
		m->szProblemMailDestination[250]=0;
	}

	if ( 
			( m->bFilterMatched == true ) /*|| 
			( m->iBlacklistResults == 1 ) || 
			( ( m->iBlacklistResults == 2 ) && ( MFC_DropUnresolvableRelayHosts ) && ( m->iMailSource == MAILFILTER_MAILSOURCE_INCOMING ) ) */
		)
	{

/*
	  	if (m->iBlacklistResults == 1)
    	{
    		MF_StatusText(" Dropping because of blacklisted IP.");
    		sprintf(m->szErrorMessage,"Found a blacklisted mailserver address. (RBL: %s)",MFC_DNSBL_Zonename);
    	}
    	if ( (m->iBlacklistResults == 2) && ( MFC_DropUnresolvableRelayHosts ) && ( m->iMailSource == MAILFILTER_MAILSOURCE_INCOMING ) )
    	{
    		MF_StatusText(" Dropping because of unresolvable relay host.");
    		strcpy(m->szErrorMessage,"Found an unknown/unresolvable relay host address.");
    	}
*/
		MF_StatusText(" Dropped Mail Summary:");
		sprintf(szCmpBuffer,"  From: %s",m->szMailFrom);				MF_StatusText(szCmpBuffer);
		sprintf(szCmpBuffer,"  Rcpt: %s",m->szMailRcpt);				MF_StatusText(szCmpBuffer);
		sprintf(szCmpBuffer,"  Subject: %s",m->szMailSubject);			MF_StatusText(szCmpBuffer);
		sprintf(szCmpBuffer,"  Mail Size: %d Byte(s)",m->iMailSize);	MF_StatusText(szCmpBuffer);

		if (
			((m->iMailSource == MAILFILTER_MAILSOURCE_INCOMING) && chkFlag(m->iFilterNotify,MAILFILTER_NOTIFICATION_ADMIN_INCOMING))
			||
			((m->iMailSource == MAILFILTER_MAILSOURCE_OUTGOING) && chkFlag(m->iFilterNotify,MAILFILTER_NOTIFICATION_ADMIN_OUTGOING))
		 ) {

/*			MF_MailProblemReport(
				m->szFileIn,
				(m->szErrorMessage[0]==0) ? "A filter rule denied the further delivery." : m->szErrorMessage,
				m->szMailFrom,
				m->szMailRcpt,
				m->szMailSubject
				);*/

			MF_MailProblemReport(m);
				
		}

		MF_Notification_Send(m);
		q=1;
	}

#ifdef MF_WITH_ZIP
	// Base64 encoding of attachment zip file
	{
		char szZipFile[MAX_PATH];	sprintf(szZipFile,"%sFiles.zip",m->szScanDirectory);
		char szB64File[MAX_PATH];	sprintf(szB64File,"%sFiles.b64",m->szScanDirectory);
		
		MFUtil_EncodeFile(szZipFile,szB64File,mimeEncodingBase64);
	}
#endif
	
	if (q)
	{
		char szAttFile[MAX_PATH];
		
		if (m->iNumOfAttachments)
		
		for (int i = 1; i < (m->iNumOfAttachments+1); i++)
		{
			sprintf(szAttFile,"%s%i.att",m->szScanDirectory,i);
			unlink(szAttFile);
			// we have a second file containing the original name of the attachment
			sprintf(szAttFile,"%s%i.mfn",m->szScanDirectory,i);
			unlink(szAttFile);
		}
	}
	
	if (m->iPriority > 0)
		m->bSchedule = false;
		
	MFD_Out(MFD_SOURCE_WORKER,"Mail priority is %d, => Schedule: %d\n",m->iPriority,m->bSchedule);
	
	if (q == 0)
		if (MFVS_PassMailToVirusScan(m))
			q = 2;
			else
			{
				/*
				 * well, I decoded all the files, and now the AV module
				 * doesn't want them. what a pity! kill everything ;>
				 */
				char szAttFile[MAX_PATH];
				
				if (m->iNumOfAttachments)
				
				for (int i = 1; i < (m->iNumOfAttachments+1); i++)
				{
					sprintf(szAttFile,"%s%i.att",m->szScanDirectory,i);
					unlink(szAttFile);
					sprintf(szAttFile,"%s%i.mfn",m->szScanDirectory,i);
					unlink(szAttFile);
				}
				
/*				if (m->bCopy)
				{
					iXList_Storage* stCopy;
					stCopy = m->lstCopies->GetFirst();
					while (stCopy != NULL)
					{
						if (((char*)stCopy->data)[0]==0)
							break;
				
						MF_CopyEmail_Send(m->szFileIn, m, ((char*)stCopy->data));
							
						stCopy = m->lstAttachments->GetNext(stCopy);
					}
				}

				if (m->bSchedule && (m->iMailSource == MAILFILTER_MAILSOURCE_SEND))
					if (!MF_MoveFileToDir(m->szFileIn,MFT_MF_ScheduleDirSend,true))
						q=2;
*/			}

	if (szThisAttachment != NULL) _mfd_free(szThisAttachment,"ExaSBuf1");
	if (szScanBuffer != NULL) _mfd_free(szScanBuffer,"ExaSBuf2");
	if (szCmpBuffer  != NULL) _mfd_free(szCmpBuffer, "ExaSBuf3");

#ifdef N_PLAT_NLM
	ThreadSwitch();
#endif

	return q;

	// 0  =  success, nothing (suspect) found!
	// 1  =  !!!!!!!, send to admin!
	// 2  =  VirusScanning active! fileIn has been removed!
	//240 =  Not Ready.

}


static int MF_ReadAddressFromDomain(const char* szDomain,const char* szAddresses,char* szAddress, int iSize)
{
	char* addrBegin;
	unsigned int pos;
	char szTemp[2002];

	addrBegin = strstr(szAddresses,szDomain);
	if (addrBegin != NULL)
	{	
		for(pos=(unsigned int)(addrBegin-szAddresses);pos>0;pos--)
		{	
			if (szAddresses[pos] == '<') break;
			if (szAddresses[pos] == ' ') break;
			if (szAddresses[pos] == ',') break;
		}
		
//		MFD_Out(MFD_SOURCE_MAIL,"beginpos: %d [%c], ",pos,szAddresses[pos]);
		if ( (pos == 0) && (szAddresses[0] != '<') )
		{	pos--; }									// special hack for ch@... instead of <ch@...
		
		pos = (-1)*((pos+1)-(addrBegin-szAddresses));
		
//		MFD_Out(MFD_SOURCE_MAIL,"newbeginpos: %d [%c]\n",pos,szAddresses[pos]);
		szTemp[0]='<';
		
		strncpy(szTemp+1,addrBegin-pos,pos);
		szTemp[pos+1]=0;
		
		for(pos=0;pos<=strlen(addrBegin);pos++)
		{	if (addrBegin[pos] == '>') break;
			if (addrBegin[pos] == ',') break;
			if (addrBegin[pos] == ' ') break;
			if (addrBegin[pos] == '"') break;	  // " // <-- stupid IDE highlighting
			if (addrBegin[pos] == '\r') break;
			if (addrBegin[pos] == '\n') break;
			if (addrBegin[pos] == '\0') break;
		}
		strncat(szTemp,addrBegin,pos);
		strcat(szTemp,">");
		
		if (szAddress != NULL)
		{
			if (strlen(szAddress) + strlen(szTemp) + 2 < iSize)
			{
				if (szAddress[0]!=0)
					strcat(szAddress,", ");
				strcat(szAddress,szTemp);
			}
		}
		
		return ((int)strlen(szTemp))+2;
	} else {
		return 0;
	}

}

static int MF_Notification_Send(MailFilter_MailData* m)	//const int notify, const int mailSource, const char* fileIn, const char* szErrorMessage, char* szMailFrom, char* szMailRcpt, const char* szMailSubject)
{

	char szInternalAddress[100];
	int iSize = 100;
	szInternalAddress[0]=0;
	const char* firstMatch;
	char* secondMatch;
	char szTemp[82];
	char* szAddresses;

	char* szMailFrom = MF_MakeValidHeaderEmail(m->szMailFrom,252);
	char* szMailRcpt = MF_MakeValidHeaderEmail(m->szMailRcpt,252);
//TODO
/*	szPointer = MF_MakeValidHeaderEmail(m->szMailRcpt,252);
	_mfd_free(m->szMailRcpt,"ExaFileOrigBuffer");
	m->szMailRcpt=szPointer;
*/	
	firstMatch = NULL;
	secondMatch = NULL;
	
	firstMatch = MF_GlobalConfiguration.DomainName.c_str();
	secondMatch = strchr(firstMatch, ',');

	if (m->iMailSource == MAILFILTER_MAILSOURCE_SEND)
		szAddresses = szMailFrom;
		else
		szAddresses = szMailRcpt;

	if (secondMatch != NULL)
	{
		strncpy(szTemp,firstMatch,(unsigned int)(secondMatch-firstMatch));
		szTemp[secondMatch-firstMatch]=0;
	
		MF_ReadAddressFromDomain(szTemp,szAddresses,szInternalAddress,iSize);
		
		while (firstMatch != NULL)
		{
			firstMatch = strchr(secondMatch, ',');
		
			if (firstMatch != NULL)
			{
				firstMatch++;
				secondMatch = strchr(firstMatch, ',');
		
				if (secondMatch != NULL)
				{
					strncpy(szTemp,firstMatch,(unsigned int)(secondMatch-firstMatch));
					szTemp[secondMatch-firstMatch]=0;
				}
				else
				{
					strncpy(szTemp,firstMatch, 79 );
					szTemp[79]=0;
				}

				MF_ReadAddressFromDomain(szTemp,szAddresses,szInternalAddress,iSize);

				if (secondMatch == NULL)
					break;
			}
			else
				break;
		
		} 
		
	} else {
	
		MF_ReadAddressFromDomain(MF_GlobalConfiguration.DomainName.c_str(),szAddresses,szInternalAddress,iSize);

	}


	// Notify Internal Sender
	// 
	if ( (szInternalAddress[0]!=0)  &&  (m->iMailSource == MAILFILTER_MAILSOURCE_SEND) && /*(MFC_Notification_EnableInternalSndr == 1)*/ chkFlag(m->iFilterNotify,MAILFILTER_NOTIFICATION_SENDER_OUTGOING))
	{
		MFD_Out(MFD_SOURCE_GENERIC," Internal Sender (ms=SEND) && notify has SENDER_OUTGOING\n");
		MF_StatusText("  -> Sending Internal Sender Notification.");

		MF_Notification_Send2 (
			 2,
			 szInternalAddress ,
			 m
			 );
	}
	
	// Notify External Sender
	if ((m->iMailSource == MAILFILTER_MAILSOURCE_RECEIVE) && /*(MFC_Notification_EnableExternalSndr == 1)*/chkFlag(m->iFilterNotify,MAILFILTER_NOTIFICATION_SENDER_INCOMING))
	{
		MFD_Out(MFD_SOURCE_GENERIC," External Sender (ms=INCOMING) && notify has SENDER_INCOMING\n");
		MF_StatusText("  -> Sending External Sender Notification.");

		MF_Notification_Send2 (
			 0,
			 szMailFrom,
			 m
			 );
	}
	
	// Notify Internal Recipient
	if ( (szInternalAddress[0]!=0)  &&  (m->iMailSource == MAILFILTER_MAILSOURCE_RECEIVE) && /*(MFC_Notification_EnableInternalRcpt == 1)*/chkFlag(m->iFilterNotify,MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING))
	{
		MFD_Out(MFD_SOURCE_GENERIC," Internal Recipient (ms=INCOMING) && notify has RECIPIENT_INCOMING\n");
		MF_StatusText("  -> Sending Internal Recipient Notification.");

		MF_Notification_Send2 (
			 3,
			 szInternalAddress ,
			 m
			 );
	}
	
	// Notify External Recipient
	if ((m->iMailSource == MAILFILTER_MAILSOURCE_SEND) && /*(MFC_Notification_EnableExternalRcpt == 1)*/chkFlag(m->iFilterNotify,MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING))
	{
		MFD_Out(MFD_SOURCE_GENERIC," Internal Recipient (ms=SEND) && notify has RECIPIENT_OUTGOING\n");
		MF_StatusText("  -> Sending External Recipient Notification.");

		MF_Notification_Send2 (
			 1,
			 szMailRcpt ,
			 m
			 );
	}
	
	
	if (m->szErrorMessage[0]!=0) { strcpy(szTemp,"  "); strncat(szTemp,m->szErrorMessage,77); szTemp[80]=0; MF_StatusLog(szTemp); }

	_mfd_free(szMailFrom,"NotifySendNewBuf1");
	_mfd_free(szMailRcpt,"NotifySendNewBuf2");
	
	return 0;
}

//
// Sends a bounce mail to the postmaster
//
// * fileIn:			input mail name
// * errorText:			optional descriptive error message which will be included in the bounce mail
// * mailRcpt:			Original Mail Recipient
// * mailFrom:			Original Mail Sender
// * mailSubject:		Original Mail Subject
//
int MF_ParseTemplate(const char* szTemplateName, FILE* fMailFile, MailFilter_MailData* mMailInfo)
{
	FILE *fTemplate			= NULL;
	char* fileTemplate		= (char*)_mfd_malloc(MAX_PATH,"Report (Tmpl)");
	char* templateString	= NULL;
	int iTemplateChar		= 0;
	int iTemplateCharPrev	= 0;

	if (szTemplateName == NULL)	return 220;
	if (fMailFile == NULL)		return 220;
	if (mMailInfo == NULL)		return 220;
	if (fileTemplate == NULL)	{	MF_OutOfMemoryHandler(); return 239; }

	sprintf(fileTemplate,"%s\\%s",MF_GlobalConfiguration.config_directory.c_str(),szTemplateName);

	fTemplate = fopen(fileTemplate,"rt");
	
	_mfd_free(fileTemplate,"Report (Tmpl)");
	
//	fprintf(fMailFile,"MAIL FROM:%s\r\nRCPT TO:%s\r\n",MFC_DOMAIN_EMail_MailFilter,MFC_DOMAIN_EMail_PostMaster);
//	fprintf(fMailFile,"From: %s\r\nTo: %s\n",MFC_DOMAIN_EMail_MailFilter,MFC_DOMAIN_EMail_PostMaster);
	
	fprintf(fMailFile,"X-Mailer: MailFilter professional "MAILFILTERVERNUM"\r\n");

	if (fTemplate == NULL)
	{
		MF_StatusText("Error reading postmaster report template!");

		fprintf(fMailFile,"Subject: MailFilter Problem Report\r\n\r\n");

		fprintf(fMailFile,"MailFilter Verification failed on the mail specified below.\r\n");
		fprintf(fMailFile,"%s\r\n\r\n",mMailInfo->szErrorMessage);
		fprintf(fMailFile,"From: \"%s\"\r\nRecipient: \"%s\"\r\nSubject: \"%s\"\r\n\r\n",mMailInfo->szMailFrom,mMailInfo->szMailRcpt,mMailInfo->szMailSubject);
//		fprintf(fMailFile,"Probably you are the only one, who has been notified! Please take approaite actions,\r\ne.g. inform the sender and/or the recipient of the original mail.\r\n\r\n");
//		fprintf(fMailFile,"The message (file) handle was:\r\n%s\r\n\r\n\nRegards,\r\nMailFilter/%s.\r\n",fileIn,MF_GlobalConfiguration.ServerName.c_str());
		fprintf(fMailFile,"ERROR: This is the default text. It is used, \r\n");
		fprintf(fMailFile,"because the template is not accessible!\r\n\r\n");
		
	} else {

		while ( (iTemplateChar = fgetc(fTemplate)) != EOF )
		{
			if (iTemplateChar == '#')
			{
				if ( (iTemplateChar == '\r') || (iTemplateChar == '\n') )
				{
					// a comment starts here ...
					while ( (iTemplateChar = fgetc(fTemplate)) != EOF )
					{	
						// as we only support line-start to line-end comments,
						// look for the line end here...
						if ( (iTemplateChar == '\r') || (iTemplateChar == '\n') )
							break;
					}
				}
			
			} else {

				if ( (iTemplateCharPrev == '%') && (iTemplateChar != '%') )
				{
					// allocate a buffer
					if (templateString == NULL)
						templateString = (char*)_mfd_malloc(100,"templateString");
				
					// overwrite the previously written %
					fseek(fMailFile,ftell(fMailFile)-1,SEEK_SET);
					templateString[0] = 0;
					
					switch (iTemplateChar)
					{
						case 'f':
							// original mail from
							sprintf(templateString,"%.79s",mMailInfo->szMailFrom);
							break;
						case 't':
							// original mail to
							sprintf(templateString,"%.79s",mMailInfo->szMailRcpt);
							break;
						case 's':
							// original mail subject 
							sprintf(templateString,"%.79s",mMailInfo->szMailSubject);
							break;
						case 'R':
							// drop reason
							sprintf(templateString,"%.79s",mMailInfo->szErrorMessage);
							break;
						case 'F':
							// Filename
							sprintf(templateString,"%.79s",mMailInfo->szFileIn);
							break;
						case 'P':
							// PostMaster E-Mail
							sprintf(templateString,"%.79s",MF_GlobalConfiguration.DomainEmailPostmaster.c_str());
							break;
						case 'M':
							// MailFilter E-Mail
							sprintf(templateString,"%.79s",MF_GlobalConfiguration.DomainEmailMailFilter.c_str());
							break;
						case 'S':
							// Servername
							sprintf(templateString,"%.79s",MF_GlobalConfiguration.ServerName.c_str());
							break;
						case 'V':
							// MailFilter Product Version
							sprintf(templateString,"%.79s",MAILFILTERVERNUM);
							break;
						default:
							// Unknown
							sprintf(templateString,"** Escape Characted %c not recognized. **",iTemplateChar);
							break;
					}
					
					fprintf(fMailFile,"%s",templateString);
				} else {
					// nothing special; write it out...
					fputc(iTemplateChar,fMailFile);
				}
			}

			iTemplateCharPrev = iTemplateChar;
		}
	
		fclose(fTemplate);
		
	}

	if (templateString != NULL)
	{
		_mfd_free(templateString,"templateString");
		templateString = NULL;
	}
	
	return 0;

}




//
// Sends a bounce mail to a user
//
// * bounceRcpt:		User who will get the bounce mail
// * fileIn:			input mail name
// * mMailInfo:			the MailFilter Mail Info container
//
int MF_Notification_Send2(const char messageType, const char* bounceRcpt, MailFilter_MailData* mMailInfo)
{
	FILE *mail	= NULL;
	char fileName[13];
	char* fileOut = NULL;
	int addVal = 0;
	int spos;
	int pos;
	char szTemp[4000];
	
	
	if ((messageType < 0) || (messageType > 3))
	{
		MF_StatusText("Notification Failed - Invalid Notification Selected.");
		MFD_Out(MFD_SOURCE_GENERIC,"Notification Text selected was %d\n",messageType);
	}
	
	if (strlen(mMailInfo->szMailRcpt)==0)	return 0;

	if (messageType > 1)
		fileOut = (char*)_mfd_malloc(strlen(MFT_GWIA_RecvDirOut)+14,"Send2");		// 13=(8+1+3)

	if (messageType < 2)
		fileOut = (char*)_mfd_malloc(strlen(MFT_GWIA_SendDirOut)+14,"Send2");		// 13=(8+1+3)
		

	if (fileOut == NULL)	{ MF_OutOfMemoryHandler();	return 239; }

	
	do
	{
		MF_Util_MakeUniqueFileName( fileName , addVal );
		if (messageType < 2)
			fileName[0]='S';
		
		if (messageType < 2)
			strcpy( fileOut , MFT_GWIA_SendDirOut );
			else
			strcpy( fileOut , MFT_GWIA_RecvDirOut );
			
		strcat( fileOut , (const char*)fileName );
		addVal = addVal+32;			// Random file num for MF_Util_MakeUniqueFilename on retry ...
	}
	while ( access(fileOut,F_OK) == 0 );

	mail = fopen(fileOut,"wt");
	_mfd_free(fileOut,"Send2");
	if (mail == NULL)
	{
		MF_StatusText("Error while opening new Mail file!");
		return 240;

	} else {
		szTemp[0]=0;
		if (messageType < 2)
		{
			pos=0;spos=0;
			for(pos=0;pos<=strlen(bounceRcpt);pos++)
			{	if (bounceRcpt[pos] == '@') break;
				if (bounceRcpt[pos] == '\n') { fclose(mail); return 238; }
				if (bounceRcpt[pos] == '\0') { fclose(mail); return 238; }
			}
			pos++;
			spos = pos;
			for(pos=spos;pos<=strlen(bounceRcpt);pos++)
			{	if (bounceRcpt[pos] == '>') break;
				if (bounceRcpt[pos] == ',') break;
				if (bounceRcpt[pos] == ' ') break;
				if (bounceRcpt[pos] == '\r') break;
				if (bounceRcpt[pos] == '\n') break;
				if (bounceRcpt[pos] == '\0') break;
				szTemp[pos-spos] = bounceRcpt[pos];
			}
			szTemp[pos-spos]=0;		
			
			if (MF_GlobalConfiguration.GWIAVersion == 600)
				fprintf(mail,"%s\r\nEHLO %s\r\n",szTemp,MF_GlobalConfiguration.DomainHostname.c_str());

			if (MF_GlobalConfiguration.GWIAVersion == 550)
				fprintf(mail,"%s\r\nHELO %s\r\n",szTemp,MF_GlobalConfiguration.DomainHostname.c_str());

		}
				
		pos=0;spos=0;
		for(pos=0;pos<=strlen(bounceRcpt);pos++)
		{	if (bounceRcpt[pos] == '<') break;
			if (bounceRcpt[pos] == '\n') 
			{
				for(pos=0;pos<=strlen(bounceRcpt);pos++)
					if ((bounceRcpt[pos] != '\n') && (bounceRcpt[pos] != '\0') && (bounceRcpt[pos] != ' ') && (bounceRcpt[pos] != '\t') && (bounceRcpt[pos] != '\r'))
					break;
				break;
			}
			if (bounceRcpt[pos] == '\0') 
			{
				for(pos=0;pos<=strlen(bounceRcpt);pos++)
					if ((bounceRcpt[pos] != '\n') && (bounceRcpt[pos] != '\0') && (bounceRcpt[pos] != ' ') && (bounceRcpt[pos] != '\t') && (bounceRcpt[pos] != '\r'))
					break;
				break;
			}
		}
		szTemp[0]=0;
		strcpy(szTemp,bounceRcpt+pos);
		fprintf(mail,"MAIL FROM:%s\r\nRCPT TO:%s\r\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),szTemp);
		if (messageType < 2)
			fprintf(mail,"DATA\n");
		
		fprintf(mail,"From: %s\r\nTo: %s\r\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),bounceRcpt);

		if (messageType > 1)
		{
			MF_ParseTemplate("RINSIDE.TPL", mail, mMailInfo);
		}
		
		if (messageType == 0)
		{
			MF_ParseTemplate("ROUTSNDR.TPL", mail, mMailInfo);
			
			fprintf(mail,"\r\n.\r\nQUIT\r\n");
		}
		if (messageType == 1)
		{
			MF_ParseTemplate("ROUTRCPT.TPL", mail, mMailInfo);
			fprintf(mail,"\r\n.\r\nQUIT\r\n");
		}
		fclose(mail);
		return 0;
	}
}

//
// Sends a bounce mail to the postmaster
//
// * fileIn:			input mail name
// * errorText:			optional descriptive error message which will be included in the bounce mail
// * mailRcpt:			Original Mail Recipient
// * mailFrom:			Original Mail Sender
// * mailSubject:		Original Mail Subject
//
int MF_MailProblemReport(MailFilter_MailData* mMailInfo) //const char* errorText, const char* mailFrom, const char* mailRcpt, const char* mailSubject)
{
	FILE *mail				= NULL;
//	FILE *mTemplate			= NULL;
	char* fileName[13];
	char* fileOut			= (char*)_mfd_malloc(strlen(MFT_GWIA_RecvDirOut)+14,"Report");		// 13=(8+1+3)
//	char* fileTemplate		= (char*)_mfd_malloc(MAX_PATH,"Report (Tmpl)");
//	char* templateString	= NULL;
	int addVal				= 0;
//	int szTemplateChar		= 0;
//	int szTemplateCharPrev	= 0;

	if (fileOut == NULL)	{ MF_OutOfMemoryHandler();	return 239; }

	do
	{
		MF_Util_MakeUniqueFileName( (char*)fileName , addVal );
		strcpy( fileOut , MFT_GWIA_RecvDirOut );
		strcat( fileOut , (const char*)fileName );
		addVal = addVal+56;			// Random file num for MF_Util_MakeUniqueFilename on retry ...
	}
	while ( access(fileOut,F_OK) == 0 );

//	sprintf(fileTemplate,"%s%s",MFC_ConfigDirectory,"REPORT.TPL");

//	mTemplate = fopen(fileTemplate,"rt");
	mail = fopen(fileOut,"wt");
	
	_mfd_free(fileOut,"Report");
//	_mfd_free(fileTemplate,"Report (Tmpl)");
	
	if (mail == NULL)
	{
//		if (mTemplate != NULL) fclose(mTemplate);
		MF_StatusText("Error while opening new Mail file!");
//** TODO **: printf("Could not write to report mail file %s.\n",fileOut);
		return 240;
	}
	
	fprintf(mail,"MAIL FROM:%s\r\nRCPT TO:%s\r\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),MF_GlobalConfiguration.DomainEmailPostmaster.c_str());
	fprintf(mail,"From: %s\r\nTo: %s\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),MF_GlobalConfiguration.DomainEmailPostmaster.c_str());
	
	MF_ParseTemplate("REPORT.TPL", mail, mMailInfo);
	
	
	fclose(mail);
	return 0;

}


//
// Sends a generic mail to the postmaster
//
// * Subject:			Subject of the e-mail
// * Text:				Text of the e-mail
//
int MF_EMailPostmasterGeneric(const char* Subject, const char* Text, const char* szAttachFileName, const char* szAttachDisplayName)
{
	FILE *mail	= NULL;
	char* fileName[13];
	char* fileOut = (char*)_mfd_malloc(strlen(MFT_GWIA_RecvDirOut)+14,"GenReport");		// 13=(8+1+3)
	int addVal = 0;

	if (fileOut == NULL)	{ MF_OutOfMemoryHandler();	return 239; }

	do
	{
		MF_Util_MakeUniqueFileName( (char*)fileName , addVal );
		strcpy( fileOut , MFT_GWIA_RecvDirOut );
		strcat( fileOut , (const char*)fileName );
		addVal = addVal+56;			// Random file num for MF_Util_MakeUniqueFilename on retry ...
	}
	while ( access(fileOut,F_OK) == 0 );

	mail = fopen(fileOut,"wt");
	_mfd_free(fileOut,"GenReport");
	if (mail == NULL)
	{
		MF_StatusText("Error while opening new Mail file!");
//** TODO **: printf("Could not write to report mail file %s.\n",fileOut);
		return 240;

	} else {
		bool bAttachFile = false;
		char szBoundary[50];
		char szB64File[MAX_PATH];
		MF_Util_MakeUniqueFileName(szBoundary,135);

		if ( strlen(szAttachDisplayName) > 6 )
		{
			bAttachFile = true;
		}

		
		if ( strlen(szAttachFileName) > 6 )
		{
			if (bAttachFile) {
			
			strncpy(szB64File,szAttachFileName,MAX_PATH);
			strcpy(szB64File+strlen(szB64File)-3,"b64");
			unlink(szB64File);
		
			MFUtil_EncodeFile(szAttachFileName,szB64File,mimeEncodingBase64);
			}
		} else {
			bAttachFile = false;
		}
		
		fprintf(mail,"MAIL FROM:%s\r\nRCPT TO:%s\r\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),MF_GlobalConfiguration.DomainEmailPostmaster.c_str());
		
		fprintf(mail,"From: %s\nTo: %s\n",MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),MF_GlobalConfiguration.DomainEmailPostmaster.c_str());
		fprintf(mail,"Subject: MailFilter Notification - %s\r\n",Subject);
		fprintf(mail,"X-Mailer: MailFilter professional "MAILFILTERVERNUM"\r\nMime-Version: 1.0\r\n");
		fprintf(mail,"Content-Type: multipart/mixed; boundary=\"==MFMFMFMFMF%s\"\r\n\r\n",szBoundary);
		fprintf(mail,"--==MFMFMFMFMF%s\r\n",szBoundary);
		fprintf(mail,"Content-Type: text/plain; charset=US-ASCII\r\nContent-Transfer-Encoding: 7bit\r\nContent-Disposition: inline\r\n\r\n");

		fprintf(mail,"%s\r\nRegards,\r\nMailFilter/%s.\r\n\r\n",Text,MF_GlobalConfiguration.ServerName.c_str());
		
		if (bAttachFile)
		{
			FILE* fAttFile = fopen(szB64File,"rb");
			if (fAttFile == NULL)
			{
				fprintf(mail,"ERROR: A file called \"%s\" should have\r\nbeen attached to this mail\r\nbut it couldn't be opened.\r\n");	
			} else {
				fprintf(mail,"--==MFMFMFMFMF%s\r\n",szBoundary);
				fprintf(mail,"Content-Type: application/zip; \r\n\tname=\"%s\"\r\n",szAttachDisplayName);
				fprintf(mail,"Content-Transfer-Encoding: Base64\r\nContent-Disposition: attachment;\r\n\tfilename=\"%s\"\r\n",szAttachDisplayName);
				fprintf(mail,"\r\n");
				
				int curChr = 0;
				while ( ( curChr = fgetc(fAttFile) ) != EOF )
				{
					fputc(curChr,mail);
				}
				fclose(fAttFile);
			}
		}
		
		fprintf(mail,"\r\n");

		fclose(mail);
		return 0;
	}
}

static int MF_PostScan_Modify( MailFilter_MailData* m )
{
	FILE* mailFile;
	FILE* fOutput;
	char* szScanBuffer = NULL;
	char* szTemp	   = NULL;
	char* szMimeBoundary = NULL;

	char fileName[13];
	char completeFileName[MAX_PATH];
	int addVal = 2;
	
	bool bModifiedFromAddress = false;
	bool bWroteXSieve = false;
	bool bWroteFooter = false;
	bool bWroteZip	  = false;
	int q					= 255;
	int curChr				= 0;
	int lstChr				= 0;
	int curPos				= 0;
	int curCmpPos			= 0;
	char szCmpBuffer[MAX_PATH];				// Alloc ~255 chars ...

	// **MODIFY** was "rt"
	mailFile = fopen(m->szFileWork,"rb");			/* Read, Text, Update */
	if (mailFile == NULL)
	{
		MF_StatusText("PostScan/Update: Could not open file for reading!");
		return 240;
	}

	do
	{
		MF_Util_MakeUniqueFileName( (char*)fileName , addVal );
		addVal = addVal+37;			// Random file num for MF_Util_MakeUniqueFilename on retry ...
		strcpy(completeFileName,m->szScanDirectory);
		strcat(completeFileName,fileName);
	}
	while ( access(completeFileName,F_OK) == 0 );

	// **MODIFY** was "wt"
	fOutput = fopen(completeFileName,"wb");			/* Write, Text */
	if (fOutput == NULL)
	{
		MF_StatusText("PostScan/Update: Could not open file for writing!");
		return 240;
	}

	szScanBuffer	 =	(char*)_mfd_malloc(2002,"PostSBuf");
	szTemp		 	 =	(char*)_mfd_malloc(2002,"PostTemp");
	szMimeBoundary	 =	(char*)_mfd_malloc(2002,"PostMime");
	if (szScanBuffer	 == NULL)		return 239;
	if (szTemp			 == NULL)		return 239;
	if (szMimeBoundary	 == NULL)		return 239;		/* out of memory */

	/* make sure our buffers are zero-ed */
	memset ( szScanBuffer	, 0 , 2000 );
	memset ( szTemp			, 0 , 2000 );
	memset ( szMimeBoundary	, 0 , 2000 );

	/* go to top of file */
	fseek(mailFile,0,SEEK_SET);

	while (q == 255)
	{
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif

	/* put in x-sieve header */
#define _POSTSCANMOD_WRITEXSIEVE() 	if (!bWroteXSieve) \
									{	\
										time_t currentTime = time(NULL); \
										strftime  (szTemp, 1999, "%a, %d %b %Y %T UTC", gmtime(&currentTime)); \
										fprintf(fOutput,"X-Sieve: (MailFilter "MAILFILTERVERNUM") PASS at %s\r\n",szTemp); \
										bWroteXSieve = true; \
									}

		if ( feof(mailFile) )
		{	q=0;	break;	}

		szScanBuffer[0]=0;

		/* Get Line ... */
		fgets(szScanBuffer,2000,mailFile);

		switch (szScanBuffer[0])
		{
			case 'x':
			case 'X':
				if ( szScanBuffer[1] == '-' )
				{
					_POSTSCANMOD_WRITEXSIEVE()
				}
				break;
			case 'r':
			case 'R':
				if ( ( m->iMailSource == MAILFILTER_MAILSOURCE_RECEIVE ) && ( MF_GlobalConfiguration.EnablePFAFunctionality ) )
				{
					/*
					 * "PFA Functionality"
					 *
					 * just fixes the bugs like RCPT TO:"Christian" <ch@ionus.at>
					 * while it should read RCPT TO:<ch@ionus.at>
					 * at least for GWIA.
					 *
					 * some lines below (many ;) we also skip the first
					 * received from: header, because the PFA always writes
					 * himself into that line... 
					 */
					if (memicmp(szScanBuffer,"rcpt to:",8) == 0)
					{
						char szInternalAddress[100];
						int iSize = 100;
						szInternalAddress[0]=0;
						const char* firstMatch;
						char* secondMatch;
						char szTemp[82];
						char* szAddresses = m->szMailRcpt;

						
					//TODO
					/*	szPointer = MF_MakeValidHeaderEmail(m->szMailRcpt,252);
						_mfd_free(m->szMailRcpt,"ExaFileOrigBuffer");
						m->szMailRcpt=szPointer;
					*/	
						firstMatch = NULL;
						secondMatch = NULL;
						
						firstMatch = MF_GlobalConfiguration.DomainName.c_str();
						secondMatch = strchr(firstMatch, ',');

						if (secondMatch != NULL)
						{
							strncpy(szTemp,firstMatch,(unsigned int)(secondMatch-firstMatch));
							szTemp[secondMatch-firstMatch]=0;
						
							MF_ReadAddressFromDomain(szTemp,szAddresses,szInternalAddress,iSize);
							
							while (firstMatch != NULL)
							{
								firstMatch = strchr(secondMatch, ',');
							
								if (firstMatch != NULL)
								{
									firstMatch++;
									secondMatch = strchr(firstMatch, ',');
							
									if (secondMatch != NULL)
									{
										strncpy(szTemp,firstMatch,(unsigned int)(secondMatch-firstMatch));
										szTemp[secondMatch-firstMatch]=0;
									}
									else
									{
										strncpy(szTemp,firstMatch, 79 );
										szTemp[79]=0;
									}

									MF_ReadAddressFromDomain(szTemp,szAddresses,szInternalAddress,iSize);

									if (secondMatch == NULL)
										break;
								}
								else
									break;
							
							} 
							
						} else {
						
							MF_ReadAddressFromDomain(MF_GlobalConfiguration.DomainName.c_str(),szAddresses,szInternalAddress,iSize);

						}
						
						char* szNewRecipient = MF_MakeValidHeaderEmail(szInternalAddress,252);
						MFD_Out(MFD_SOURCE_MAIL,"got %s - ",szNewRecipient);
						if (strcmp(szNewRecipient,"<>") != 0)
						{
							sprintf(szScanBuffer,"RCPT TO:%s\r\n",szNewRecipient);
							MFD_Out(MFD_SOURCE_MAIL,"patched.\n");
						} else {
							MFD_Out(MFD_SOURCE_MAIL,"_NOT_ patched.\n");
						}
						free(szNewRecipient);
					}
				}
				break;
			case 'f':
			case 'F':
				if ( memicmp(szScanBuffer,"from:",5) == 0 )
				{
					_POSTSCANMOD_WRITEXSIEVE()
					
					if ((m->iMailSource == 0) && (bModifiedFromAddress == false) && (MF_GlobalConfiguration.Multi2One != ""))
					{
						szTemp[0]=0;
						strncpy( szTemp , szScanBuffer+5, 2000 );
						szTemp[2000]=0;
						strlwr(szTemp);

						for (curPos = 0; curPos <= strlen(szTemp);curPos++)
						{
							if (szTemp[curPos]=='@')
								{	szTemp[curPos] = 0;
									break;
								}
						}
						
						curCmpPos = (int)strlen(MF_GlobalConfiguration.Multi2One.c_str());
						
						szCmpBuffer[0]=szTemp[strlen(szTemp)-1];
						szCmpBuffer[1]=szTemp[strlen(szTemp)-0];
						szCmpBuffer[2]=0;
						curChr = atoi(szCmpBuffer);
						if (curChr != 0)
						{
							szCmpBuffer[0]=0;
							
							if (memicmp(MF_GlobalConfiguration.Multi2One.c_str(),szTemp+strlen(szTemp)-strlen(MF_GlobalConfiguration.Multi2One.c_str()),strlen(MF_GlobalConfiguration.Multi2One.c_str())-2)==0)
							{
								szScanBuffer[curPos-2+5] = MF_GlobalConfiguration.Multi2One[(unsigned int)(curCmpPos-2)];
								szScanBuffer[curPos-1+5] = MF_GlobalConfiguration.Multi2One[(unsigned int)(curCmpPos-1)];
							}
						}
						bModifiedFromAddress = true;
					}
				}
				break;
			case '.':
				if (!bWroteFooter)
					fwrite(MF_GlobalConfiguration.MessageFooterText.c_str(),sizeof(char),strlen(MF_GlobalConfiguration.MessageFooterText.c_str()),fOutput);
				bWroteFooter = true;
				break;
			case 'C':
#ifdef MF_WITH_ZIP
				if (szMimeBoundary[0] == 0)
				{
					if ( memicmp(szScanBuffer,"Content-Type: multipart/mixed; boundary=\"",40) == 0 ||
					     memicmp(szScanBuffer,"Content-Type: multipart/alternative; boundary=\"",46) == 0
					)
					{
						int len = 0;
						strncpy(szMimeBoundary,szScanBuffer+41,1999);
						len = strlen(szMimeBoundary);
						if (len>2) { szMimeBoundary[len-1] = 0; }
						
						MFD_Out(MFD_SOURCE_ZIP,"MimeBoundary: -->%s\n",szMimeBoundary);
					}
				}
#endif			
				break;
			case '-':
#ifdef MF_WITH_ZIP
				if (szScanBuffer[1] == '-')
				{	if (memicmp(szScanBuffer+2,szMimeBoundary,strlen(szMimeBoundary)) == 0)
					{
						if (!bWroteZip)
						{
							char szB64File[MAX_PATH];	sprintf(szB64File,"%sFiles.b64",m->szScanDirectory);
						
							strcpy(szTemp,"--");
							strncat(szTemp,szMimeBoundary,1990);
						 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);

							strcpy(szTemp,"Content-Type: application/zip; name=\"Attachments.zip\"\r\n");
						 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);

							strcpy(szTemp,"Content-Transfer-Encoding: base64\r\n");
						 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);

							strcpy(szTemp,"Content-Disposition: attachment; filename=\"Attachments.zip\"\r\n");
						 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);
							
							strcpy(szTemp,"\r\n");
						 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);
						 	
						 	/* data */
						 	FILE* fAttFile = fopen(szB64File,"r");
						 	if (fAttFile != NULL)
						 	{	
						 		while (fgets(szTemp, 1998, fAttFile) != NULL)
						 		{
						 			fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);
						 			
									strcpy(szTemp,"\r\n");
								 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);
						 		}
						 		fclose(fAttFile);
						 	} else MF_StatusText("Error opening B64 Att File!");

							strcpy(szTemp,"\r\n");
						 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);
						 	
							strcpy(szTemp,"\r\n");
						 	fwrite(szTemp,sizeof(char),strlen(szTemp),fOutput);
						 	
						 	bWroteZip = true;
						 	
						}
					}
				}			
#endif			
				break;
				
/*			case 's':
			case 'S':
					if ( memicmp(szScanBuffer,"Subject:",8) == 0 )
					{
					}
				break;
			case 'r':
			case 'R':
					if ( memicmp(szScanBuffer,"RCPT TO:",8) == 0 )
					{
						bHaveRcpt = true;
						szMailRcpt[0]=0;

						strncpy( szMailRcpt , szScanBuffer+8 , 245 );
						szMailRcpt[249]=0;
					}
				break;
			case 's':
			case 'S':
					if ( memicmp(szScanBuffer,"Subject:",8) == 0 )
					{
						bHaveSubject = true;

						szMailSubject[curCmpPos]=0;

						strncpy( szMailSubject , szScanBuffer+8 , 1998 );

						szMailSubject[1999]=0;

						q = MF_ExamineSubject(szMailSubject);
						if ( q )
						{
							sprintf(szErrorMessage,"A part of the Subject matched with \"%s\"",MFT_Filter_ListSubjects[q-1]);
							MF_StatusText(szErrorMessage);
							q=1;
						} else { q=255; }

					}
				break;
*/
			
		}
	
		fwrite(szScanBuffer,sizeof(char),strlen(szScanBuffer),fOutput);
	}

	/* write out a footer text */
	if (!bWroteFooter)
		fwrite(MF_GlobalConfiguration.MessageFooterText.c_str(),sizeof(char),strlen(MF_GlobalConfiguration.MessageFooterText.c_str()),fOutput);

	fclose(mailFile);
	fclose(fOutput);
	
//	unlink(m->szFileWork);
//	MFD_Out("postscan rename: %s->%s\n",fileName,fileIn);
//	int rc = rename(fileName,fileIn);
//	MFD_Out("rc: %d, %d %s\n",rc,errno,strerror(errno));
	
	MF_MoveFileToFile(completeFileName,m->szFileWork,true);

	_mfd_free(szMimeBoundary,"PostMBBf");	
	_mfd_free(szScanBuffer,"PostSBuf");
	_mfd_free(szTemp,"PostTemp");
	
	return 0;
	
}

static int MF_PostScan_HandleFile( MailFilter_MailData* m )
{
	/*
	 * finishes handling of a mail file.
	 * the mail is still in the m->szFileWork location.
	 *
	 */

	//
	// write x-sieve and modify some other shit -- see above this func.
	MF_PostScan_Modify( m );

	//
	// should I copy the mail?
	if (m->bCopy)
	{
		MF_StatusText("  Sending copies...");
		iXList_Storage* stCopy;
		stCopy = m->lstCopies->GetFirst();
		while (stCopy != NULL)
		{
			if (((const char*)stCopy->data)[0]==0)
				break;
			MFD_Out(MFD_SOURCE_MAIL,"Sending copy to %s\n",((const char*)stCopy->data));
			MF_CopyEmail_Send(m->szFileWork, m, ((const char*)stCopy->data));
				
			stCopy = stCopy->next;
		}
	}
	
	//
	// move the file where it should be delivered (->gwia dir)
	if ((m->bSchedule) && (m->iMailSource == MAILFILTER_MAILSOURCE_SEND))
	{
		// or do scheduling and that.
		if (MF_MoveFileToDir(m->szFileWork,MFT_MF_ScheduleDirSend,true) == 0)
		{
			return ERROR_SUCCESS;
		} else {
			MF_StatusText("** ERROR: Left mail in MFSCAN, error moving to MFSCHED.");
			return 1;
		}
	} else {
		if (MF_MoveFileToFile(m->szFileWork,m->szFileOut,true) == 0)
		{
			return ERROR_SUCCESS;
		} else {
			MF_StatusText("** ERROR: Left mail in MFSCAN, error moving.");
			return 1;
		}
	}

	return ERROR_SUCCESS;
}



// 
// Process mail Files: copies file to MFWORK and starts MF_ExamineFile, and move it to output dir
//
// * fileName:		MailFile Name
// * fileIn:		Input Mail File Name (with Path)
// * fileOut:		Output Mail File Name (with Path)
// * mailSource:		which directory
//							0	from the SEND directory
//							1	from the RECV directory
//
static void MF_ProcessFile(const char* fileName, const char* fileIn, const char* fileOut, int mailSource)
{
	int waitCount = -1;				// loop counter for examinig file open problems 
	int rc;
	rc = -1;
	struct stat statInfo;
	char szLockFile[MAX_PATH];
	FILE* fLockFile = NULL;
	unsigned long iDateTime = 0;
	char statusText[82];

	if (fileName[0] == 'X')
	{
	 	MFD_Out(MFD_SOURCE_WORKER, "NOT processing %s (it's the X baby)\n", fileName );
	 	return;
	}

MFD_Out(MFD_SOURCE_WORKER,"Processing %s ...\n", fileName);
	
	MailFilter_MailData* m = MailFilter_MailInit((char*)fileName,mailSource);
	strncpy(	m->szFileIn		, fileIn	,	MAX_PATH	);
	strncpy(	m->szFileOut	, fileOut	,	MAX_PATH	);

//	_mfd_free(m->szScanDirectory,"ProcessFile");	m->szScanDirectory = NULL;
	/*m->szScanDirectory = */
	if (MFVS_MakeScanPath(m->szScanDirectory,MAX_PATH))
	{
//		MF_StatusText("No Free Scan Directories found - Could not process E-Mail.");
		MailFilter_MailDestroy(m);
		return;
	}
//	if (m->szScanDirectory == NULL)
//		{ MailFilter_MailDestroy(m); return; }

	strncpy(szLockFile,m->szScanDirectory,MAX_PATH);
	strncat(szLockFile,"LOCK.MFS",MAX_PATH);
	fLockFile = fopen(szLockFile,"wb");
	if (fLockFile != NULL)	
		fclose(fLockFile);
		else 
		{ MailFilter_MailDestroy(m); return; }
		
	sprintf(m->szFileWork, "%s%s", m->szScanDirectory, fileName );

	if (stat(fileIn,&statInfo) == -1)
	{	MailFilter_MailDestroy(m);	return;	}

	m->iMailSize = (long)statInfo.st_size;
	if (m->iMailSize == 0)
	{	MailFilter_MailDestroy(m);	return;	}

	m->iMailTimestamp = (unsigned long)statInfo.st_mtime;

	sprintf(statusText,"Mail: %s/%-12s, Size: %d kB", mailSource == 0 ? "SEND" : "RECEIVE", m->szFileName == NULL ? "*ERROR*" : m->szFileName, (m->iMailSize < 1024) ? 1 : (m->iMailSize/1024));
	statusText[78]='\0';
	MF_StatusText(statusText);

	rc = MF_MoveFileToFile( m->szFileIn, m->szFileWork, false );
	if (rc == 0)
	{	/* OK */

		while(waitCount < 5)
		{
			rc = MF_ExamineFile ( m );
			switch (rc)
			{
				case 0:		/* SUCCESS ; VirusScanning is disabled */
				
					// Move file to GWIA OK directory.
					// Mail PASS!
					
					// *** Warning: we will come in here also if Scheduling is ON! ***
					
					MF_StatusText("  Mail passed.");

					MF_PostScan_HandleFile( m );
					break;
				
				case 1:
				
					/* Mail a problem report.
					 * Move file to PROBLEM directory. */
					if (m->iMailSource == 0)
						MFS_MF_MailsOutputFailed++;
					else
						MFS_MF_MailsInputFailed++;

					if ( MF_MoveFileToDir( m->szFileWork, MFT_MF_ProbDir, true ) )
						MF_StatusText("The Original E-Mail could not be moved to MFPROB.");

					break;
					
				case 2:		/* SUCCESS ; VirusScanning is enabled. */

					// DO NOT do anything with fileIn - it won't exist anymore...
					break;
					
				case 237:
				
//					MF_StatusText("No Free Scan Directories found - Could not process E-Mail.");
					
					if ( MF_MoveFileToFile( m->szFileWork, m->szFileIn, true ) )
						MF_StatusText("Could not move E-Mail back.");
					
					break;
				
				default:	/* 240 = err. opening */

					sprintf(statusText,"Error Code from ExamineFile(): %d",rc);
					statusText[79]='\0';
					MF_StatusText(statusText);

					/* Not Ready to access file. */
					MF_StatusText("Waiting for Access to file ...");
					delay(1000);
					waitCount++;
					if (MFT_NLM_Exiting > 0)
						waitCount = 6;

					break;
				
			}

			if (waitCount == -1)
				break;

		} 
		if (waitCount > 5)
		{
			MailFilter_MailData* emptyMailInfo = new MailFilter_MailData;
			
			MF_MailProblemReport(emptyMailInfo);
			strcpy(emptyMailInfo->szFileIn,m->szFileIn);
			strcpy(emptyMailInfo->szMailFrom,"Unknown");
			strcpy(emptyMailInfo->szMailRcpt,"Unknown");
			strcpy(emptyMailInfo->szMailCC,"Unknown");
			strcpy(emptyMailInfo->szErrorMessage,"The mail could not be moved/opened from the MFWORK directory.");
			delete(emptyMailInfo);
		}

		/* Increase Counters ... */
		if (m->iMailSource == 0)
			MFS_MF_MailsOutputTotal++;
		else
			MFS_MF_MailsInputTotal++;

	} else {
MFD_Out(MFD_SOURCE_ERROR,"Worker: ProcessFile: ERROR: %d\n",rc);
		if (rc != 239)
		{
			// * Do Nothing, so this will be queued for next loop.
			MF_StatusLog("Skipped Mail, Couldn't read it ... Will Retry Later.");
		} else {
			MF_OutOfMemoryHandler();
		}
	}

MFD_Out(MFD_SOURCE_WORKER,"MFProcessFile: Exiting...\n");

	unlink(szLockFile);
	MailFilter_MailDestroy(m);

	return;
}

static void MFVS_CheckQueue()
{
	char szScanFile[MAX_PATH];
	char szScanDir[MAX_PATH];
	char szAttachmentFile[MAX_PATH];
	char szTemp[MAX_PATH];

	int i;
	MailFilter_MailData* m;

	long iTotalAttSize = 0;
	int rc = 0;

	/* Scan the SCAN directory (our queue) ... */
	for (int iScanDir = 0; iScanDir < MF_GlobalConfiguration.MailscanDirNum; iScanDir++)
	{
		if (MFT_NLM_Exiting > 0)	break;
		
		sprintf(szScanDir,"%s"IX_DIRECTORY_SEPARATOR_STR"MFSCAN"IX_DIRECTORY_SEPARATOR_STR"%04i"IX_DIRECTORY_SEPARATOR_STR,MF_GlobalConfiguration.MFLTRoot.c_str(),iScanDir);
		chdir(szScanDir);

		sprintf(szScanFile,"%sMAILFLT.MFS",szScanDir);
		if (access(szScanFile,W_OK) == 0)
		{
			struct stat statBuf;
			
			stat ( szScanFile , &statBuf );

			time_t currentTime = time(NULL);

//			MFD_Out(" F %d - %d\n",iScanDir,(unsigned int) difftime(currentTime,statBuf.st_ctime));

			if (((unsigned int) difftime(currentTime,statBuf.st_ctime)) > MF_GlobalConfiguration.MailscanTimeout)
			{
				m = MailFilter_MailRead(szScanFile);
				if (m == NULL)
					{
						MF_StatusText("** An error occoured while reading MFVS Status!");
						continue;
					}

				/* Check for existance of the mail. */
				if (access(m->szFileWork,F_OK) != 0)
				{
					rc = 4;
					MF_StatusText(" AV Scan failed. (File deleted.)");
				}					

				if (rc == 0)
				{
					struct stat statInfo;
					if (stat(m->szFileWork,&statInfo) == -1)
					{
						rc = -1;
						MF_StatusText("Left mail in MFSCAN. Reason: stat() returned -1.");
					} else {
					
						if (m->iMailSize != statInfo.st_size)
						{
							rc = 2;
							MF_StatusText(" AV Scan failed. (File changed.)");
						}
						
/*						if (iDateTime != (int)statInfo.st_mtime)
						{
							rc = 3;
							MF_StatusText("Mail was modified. Dropping ...");
						}*/
						
						iTotalAttSize = 0;
						if (m->iNumOfAttachments > 0)
						{
							int myrc = 0;
							for (i = 1; i < ( m->iNumOfAttachments +1 ); i++)
							{
								sprintf(szAttachmentFile,"%s%i.att",szScanDir,i);
								if (access(m->szFileWork,F_OK) != 0)
								{
									myrc = 5;
									MF_StatusText(" Infected Attachment detected.");
								} else {
									statBuf.st_size = 0;
									if (stat ( szAttachmentFile , &statBuf ) != 0)
									{
										myrc = 5;
										MF_StatusText(" Infected Attachment detected. (File size changed.)");
									} else {
										iTotalAttSize += (long)statBuf.st_size;
									}
								}
								// cleanup ;)
								unlink(szAttachmentFile);
								// we have a second file containing the original name of the attachment
								sprintf(szAttachmentFile,"%s%i.mfn",szScanDir,i);
								unlink(szAttachmentFile);
								
								if (myrc != 0)
									break;
							}
							if ( (rc == 0) && (myrc != 0) )
								rc = myrc;
						}

MFD_Out(MFD_SOURCE_VSCAN," totals: detected size %d, should be %d; result: %d \n",iTotalAttSize,m->iTotalAttachmentSize,(iTotalAttSize != m->iTotalAttachmentSize));
						if ((rc==0) && (iTotalAttSize != m->iTotalAttachmentSize))
						{
							rc = 5;
							MF_StatusText(" Infected Attachment detected.");
						}
					}
				}

				if (rc == 0)
				{
					/* Virus Scan Passed */
				
					if (access(m->szFileWork,W_OK) == 0)
					{
						if (MF_PostScan_HandleFile( m ) == ERROR_SUCCESS)
						{
							unlink(szScanFile);
						}

/*						MF_PostScan_Modify( m );
						if (m->bCopy)
							MF_CopyEmail_Send(m->szFileWork, m);
						
						if ((m->bSchedule) && (m->iMailSource == MAILFILTER_MAILSOURCE_SEND))
						{
							if (MF_MoveFileToDir(m->szFileWork,MFT_MF_ScheduleDirSend,true) == 0)
							{
								unlink(szScanFile);
							} else {
								MF_StatusText("** ERROR: Left mail in MFSCAN, error moving to MFSCHED.");
							}
						} else {
							if (MF_MoveFileToFile(m->szFileWork,m->szFileOut,true) == 0)
							{
								unlink(szScanFile);
							} else {
								MF_StatusText("** ERROR: Left mail in MFSCAN, error moving.");
							}
						}
*/
					} else {
						MF_StatusText("** ERROR: Left mail in MFSCAN.");
					}
					
				} else {
MFD_Out(MFD_SOURCE_VSCAN,"  VSCAN FAIL\n");

					/* Virus Scan Failed */
					MF_StatusText(" Dropped Mail Summary:");
					sprintf(szTemp,"  From: %s",m->szMailFrom);				MF_StatusText(szTemp);
					sprintf(szTemp,"  Rcpt: %s",m->szMailRcpt);				MF_StatusText(szTemp);
					sprintf(szTemp,"  Subject: %s",m->szMailSubject);		MF_StatusText(szTemp);
					sprintf(szTemp,"  Mail Size: %d Byte(s)",m->iMailSize);	MF_StatusText(szTemp);
					MF_StatusText("  Reason: Virus Scan failed.");

					if (m->iMailSource == 0)
						MFS_MF_MailsOutputFailed++;
					else
						MFS_MF_MailsInputFailed++;

					strcpy(m->szErrorMessage,"Virus Scan failed.");
					m->iFilterNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
					if (MF_GlobalConfiguration.DefaultNotification_InternalRecipient)	{ m->iFilterNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING; }
					if (MF_GlobalConfiguration.DefaultNotification_InternalSender)		{ m->iFilterNotify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING; }
					if (MF_GlobalConfiguration.DefaultNotification_ExternalRecipient)	{ m->iFilterNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING; }
					if (MF_GlobalConfiguration.DefaultNotification_ExternalSender)		{ m->iFilterNotify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING; }

					MF_Notification_Send(m);
					
					if (MF_MoveFileToDir(m->szFileWork,MFT_MF_ProbDir,true))
						MF_StatusText("** The Original E-Mail could not be moved to MFPROB.");

					unlink(szScanFile);
				}
			
				MailFilter_MailDestroy(m);
				//
			}
			
		}
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
	}		
}

//
// * Checks the Problem Directory for its Size and the Age of the contained files
//
void MF_CheckProblemDirAgeSize()
{
	struct stat 	statInfo;
	unsigned long	killAge = 0;
	char 			probDir[MAX_PATH];
	char 			thisFile[MAX_PATH];
	DIR				*mailDir;
	unsigned long	cntTotalNumber = 0;
	unsigned long	cntKilledNumber = 0;
#ifdef __NOVELL_LIBC__
	long long		cntTotalSize = 0;
	long long		cntKilledSize = 0;
#else
	unsigned long	cntTotalSize = 0;
	unsigned long	cntKilledSize = 0;
#endif
	char			messageText[8000];
#ifdef WIN32
	struct dirent	*dirEntry;
#endif

	MFD_Out(MFD_SOURCE_GENERIC,"Checking MFPROB size...");

	if ((MF_GlobalConfiguration.ProblemDirMaxSize == 0) && (MF_GlobalConfiguration.ProblemDirMaxAge == 0))
		return;
	
	if (MF_GlobalConfiguration.ProblemDirMaxAge)
		killAge = (unsigned long)(time(NULL) - (MF_GlobalConfiguration.ProblemDirMaxAge*86400));
	
#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)
	sprintf(probDir,"%s*.*",MFT_MF_ProbDir);
#else
	strcpy(probDir,MFT_MF_ProbDir);
#endif
MFD_Out(MFD_SOURCE_GENERIC,"prb: %s\n",probDir);
	mailDir = opendir(probDir);
	while ( mailDir != NULL )
	{
#ifdef N_PLAT_NLM
#ifdef __NOVELL_LIBC__
		mailDir = readdir(mailDir);
		if (mailDir == NULL) 		break;
		
		sprintf(thisFile,"%s%s",MFT_MF_ProbDir,mailDir->d_name);
#else
		mailDir = readdir(mailDir);
		if (mailDir == NULL) 		break;
		
		sprintf(thisFile,"%s%s",MFT_MF_ProbDir,mailDir->d_nameDOS);
#endif
#endif
#ifdef WIN32
		dirEntry = readdir(mailDir);
		if (dirEntry == NULL)		break;
		
		sprintf(thisFile,"%s%s",MFT_MF_ProbDir,dirEntry->d_name);
#endif

		if (stat(thisFile,&statInfo) == -1)
		{
			MF_StatusText("ChkProblemDirAgeSize: Error getting stat() info for file!");
			break;
		}
		else if (statInfo.st_size != 0)
		{
			cntTotalNumber++;

			cntTotalSize = cntTotalSize + statInfo.st_size;
			
			if ((!MF_GlobalConfiguration.ProblemDirMaxAge == 0) && (killAge > statInfo.st_mtime))
			{
				cntKilledNumber++;

				cntKilledSize = cntKilledSize + statInfo.st_size;

				unlink(thisFile);
			}
		}
	}
	if (mailDir != NULL) closedir(mailDir);

	MFD_Out(MFD_SOURCE_GENERIC,"size compare says: %lld vs %d\n", ((cntTotalSize - cntKilledSize)/1024), MF_GlobalConfiguration.ProblemDirMaxSize);

	if (( ((cntTotalSize - cntKilledSize)/1024) > 
#ifdef __NOVELL_LIBC__
		(long long)MF_GlobalConfiguration.ProblemDirMaxSize
#else
		MF_GlobalConfiguration.ProblemDirMaxSize
#endif
		) && (bool)(MF_GlobalConfiguration.ProblemDirMaxSize)
	)
	{
		MFD_Out(MFD_SOURCE_GENERIC,"==> trying size reduction\n");
	
		mailDir = opendir(probDir);
		while ( mailDir != NULL )
		{
/*			mailDir = readdir(mailDir);
			if (mailDir == NULL) 		break;
			
			sprintf(thisFile,"%s%s",MFT_MF_ProbDir,mailDir->d_nameDOS);
*/
#ifdef N_PLAT_NLM
#ifdef __NOVELL_LIBC__
			mailDir = readdir(mailDir);
			if (mailDir == NULL) 		break;
			
			sprintf(thisFile,"%s%s",MFT_MF_ProbDir,mailDir->d_name);
#else
			mailDir = readdir(mailDir);
			if (mailDir == NULL) 		break;
			
			sprintf(thisFile,"%s%s",MFT_MF_ProbDir,mailDir->d_nameDOS);
#endif
#endif
#ifdef WIN32
			dirEntry = readdir(mailDir);
			if (dirEntry == NULL)		break;
			
			sprintf(thisFile,"%s%s",MFT_MF_ProbDir,dirEntry->d_name);
#endif
			
			stat(thisFile,&statInfo);
			if (statInfo.st_size != 0)
			{
				cntKilledNumber++;
				cntKilledSize = cntKilledSize + statInfo.st_size;
				
				unlink(thisFile);
			}
			
		}
		closedir(mailDir);
	} else
		MFD_Out(MFD_SOURCE_GENERIC,"==> not trying size reduction\n");
	
	MFD_Out(MFD_SOURCE_GENERIC,	
#ifdef __NOVELL_LIBC__ // 64bit
			" > Total Files: %d, Size: %lld kBytes\n > Total Killed: %d, Size: %lld kBytes\n > New Totals: %d, Size: %lld kBytes\n",
#else
			" > Total Files: %d, Size: %d kBytes\n > Total Killed: %d, Size: %d kBytes\n > New Totals: %d, Size: %d kBytes\n",
#endif
			cntTotalNumber,cntTotalSize,cntKilledNumber,cntKilledSize,cntTotalNumber-cntKilledNumber,cntTotalSize - cntKilledSize);

	if (MF_GlobalConfiguration.NotificationAdminMailsKilled && cntKilledNumber)
	{
		cntTotalSize = cntTotalSize / 1024;
		cntKilledSize = cntKilledSize / 1024;

		sprintf(messageText,
			"The Problem Directory has been cleaned up.\r\n\r\nStatistics:\r\n"
#ifdef __NOVELL_LIBC__ // 64bit
			" > Total Files: %d, Size: %lld kBytes\r\n > Total Killed: %d, Size: %lld kBytes\r\n > New Totals: %d, Size: %lld kBytes\r\n",
#else
			" > Total Files: %d, Size: %d kBytes\r\n > Total Killed: %d, Size: %d kBytes\r\n > New Totals: %d, Size: %d kBytes\r\n",
#endif
			cntTotalNumber,cntTotalSize,cntKilledNumber,cntKilledSize,cntTotalNumber-cntKilledNumber,cntTotalSize - cntKilledSize);
		MF_EMailPostmasterGeneric("Problem Directory Cleanup",messageText,"","");
	}
}

static void MFBW_CheckQueue(char* szFile,char* szIn,char* szOut)
{
#pragma unused(szFile)
	if (MFBW_CheckCurrentScheduleState())
	{
		MF_MoveFileToFile(szIn,szOut,true);
	}
}


/*
 * Create the directory szDirectoryName if it doesnt exist.
 * If bCleanup, delete old files from the directory.
 */
static bool CheckDirectory(const char* szDirectoryName, bool bCleanUp)
{
	// Create directory if not existant
	if (chdir(szDirectoryName))
	{
		MFD_Out(MFD_SOURCE_GENERIC,"Create: %s\n",szDirectoryName);
		mkdir(szDirectoryName,S_IRWXU);
	}

	// delete stale files? (age > 2days && 0 bytes)
	if (bCleanUp == true)
	{
		time_t mintime = time(NULL) - (2*(24*60*60));

		iXDir dir(szDirectoryName);
		
		char szTemp[250];

		while ( dir.ReadNextEntry() )
		{
			const char* e = dir.GetCurrentEntryName();
			long long s = dir.GetCurrentEntrySize();
			time_t t = dir.GetCurrentEntryModificationTime();

			if ( (t < mintime) && (s == 0) )
			{
				sprintf(szTemp,"Found old file '%s' in a queue. Deleting.",e);
				MF_StatusText(szTemp);
				dir.UnlinkCurrentEntry();
			}
		}
	}
	
	return true;
}

/*
 * Loop through the directory szDirectoryName 
 * and call MF_ProcessFile for all files in it.
 */
static bool HandleGwiaDirectory(const char* szDirectoryName, const char* szDirectoryName2, int passOn, int tlc)
{
	char fileIn[MAX_PATH];
	char fileOut[MAX_PATH];
	
	#ifdef _TRACE
	MFD_Out(MFD_SOURCE_WORKER,"Check %s\n", szDirectoryName);
	#endif
	
	iXDir dir(szDirectoryName);
	dir.SkipDotFiles = true;
	
	while ( dir.ReadNextEntry() )
	{
		const char* e = dir.GetCurrentEntryName();
		
		if (MFT_NLM_Exiting > 0)
			break;

		// Build filenames...
		sprintf(fileIn,  "%s%s", szDirectoryName,  e);
		sprintf(fileOut, "%s%s", szDirectoryName2, e);

		if (tlc>10)
			MF_ProcessFile(e,fileIn,fileOut,passOn);

		ThreadSwitch();
	}
	
	MF_StatusNothing();

	return true;
}

void MFWorker_SetupPaths()
{
	char scanDir[MAX_PATH];

#define PS IX_DIRECTORY_SEPARATOR_STR

	// Init Directories
	sprintf(MFT_GWIA_SendDirIn,				"%s"PS"SEND"PS,			MF_GlobalConfiguration.GWIARoot.c_str());
	sprintf(MFT_GWIA_SendDirOut,			"%s"PS"SEND"PS,			MF_GlobalConfiguration.MFLTRoot.c_str());
	sprintf(MFT_GWIA_RecvDirIn,				"%s"PS"RECEIVE"PS,		MF_GlobalConfiguration.MFLTRoot.c_str());
	sprintf(MFT_GWIA_RecvDirOut,			"%s"PS"RECEIVE"PS,		MF_GlobalConfiguration.GWIARoot.c_str());
	sprintf(MFT_GWIA_ResultDirIn,			"%s"PS"RESULT"PS,		MF_GlobalConfiguration.MFLTRoot.c_str());
	sprintf(MFT_GWIA_ResultDirOut,			"%s"PS"RESULT"PS,		MF_GlobalConfiguration.GWIARoot.c_str());
	sprintf(MFT_MF_ScheduleDirSend,			"%s"PS"MFSCHED"PS,		MF_GlobalConfiguration.MFLTRoot.c_str());
	sprintf(MFT_MF_ProbDir,					"%s"PS"MFPROB"PS,		MF_GlobalConfiguration.MFLTRoot.c_str());

	CheckDirectory(MFT_GWIA_SendDirIn		,true	);
	CheckDirectory(MFT_GWIA_SendDirOut		,false	);
	CheckDirectory(MFT_GWIA_RecvDirIn		,true	);
	CheckDirectory(MFT_GWIA_RecvDirOut		,false	);
	CheckDirectory(MFT_GWIA_ResultDirIn		,true	);
	CheckDirectory(MFT_GWIA_ResultDirOut	,false	);

	CheckDirectory(MFT_MF_ScheduleDirSend	,false	);
	CheckDirectory(MFT_MF_ProbDir			,false	);

	sprintf(scanDir,"%sCRASH"PS,MFT_MF_ProbDir);	CheckDirectory(scanDir					,false	);
	sprintf(scanDir,"%sDROP"PS,MFT_MF_ProbDir);		CheckDirectory(scanDir					,false	);
	/* Remap MFPROB to MFPROB/DROP */
	sprintf(scanDir,"%sDROP"PS,MFT_MF_ProbDir);
	strcpy(MFT_MF_ProbDir,scanDir);

	// Create SCAN directory.
	sprintf(scanDir,"%s"IX_DIRECTORY_SEPARATOR_STR"MFSCAN"PS,MF_GlobalConfiguration.MFLTRoot.c_str());
	CheckDirectory(scanDir					,false	);

	// Create SCAN %i subdirectories...
	int iScanDir = 0;
	for (iScanDir = 0; iScanDir < MF_GlobalConfiguration.MailscanDirNum; iScanDir++)
	{
		sprintf(scanDir,"%s"PS"MFSCAN"PS"%04i"PS,MF_GlobalConfiguration.MFLTRoot.c_str(),iScanDir);
		CheckDirectory(scanDir					,true	);
	}
}

// 
// **** Worker Thread ****
//
#ifndef WIN32
void MF_Work_Startup(void *dummy)
#else
DWORD WINAPI MF_Work_Startup(void *dummy)
#endif // WIN32
{
#pragma unused(dummy)

	DIR* mailDir;
	DIR* readentry;

	char fileIn[MAX_PATH];
	char fileOut[MAX_PATH];
	char scanDir[MAX_PATH];
	char scanPath[MAX_PATH];
	int tlc = 0;
	int lc = 255;
	int shallTerminate = 0;
	int iScanDir;

#pragma unused(scanPath)

	MFT_NLM_ThreadCount++;
	
	// Rename this Thread
#if defined( N_PLAT_NLM ) && (!defined(__NOVELL_LIBC__))
	RenameThread(GetThreadID(),programMesgTable[MSG_THREADNAME_WORK]);		// 15 (16?) Chars max.
#endif
#if defined( N_PLAT_NLM ) && (defined(__NOVELL_LIBC__))
	NXContextSetName(NXContextGet(),"MailFilterWorker");
#endif


	// OLD BROKEN CODE
	// PLEASE REPLACE WITH iXDir c++ api!
#ifdef N_PLAT_NLM
#ifdef __NOVELL_LIBC__
#define _MF_DIRECTORY_FILENAME readentry->d_name
#define _MF_DIRECTORY_OPENDIR(dirPath) mailDir = NULL; errno = 0; chdir(dirPath); strcpy(scanPath,dirPath); mailDir = opendir(scanPath); \
		if (mailDir == NULL) { MFD_Out(MFD_SOURCE_ERROR,"opendir error!\n"); if (errno) { MFD_Out(MFD_SOURCE_ERROR,"mailDir (%d): %s\n",errno,strerror(errno)); } }
#define _MF_DIRECTORY_READDIR errno = 0; readentry = readdir(mailDir); if (readentry == NULL) { if (errno) { MFD_Out(MFD_SOURCE_ERROR,"readdir (%d): %s\n",errno,strerror(errno)); } break; }
#else
#define _MF_DIRECTORY_FILENAME readentry->d_nameDOS
#define _MF_DIRECTORY_OPENDIR(dirPath) errno = 0; chdir(dirPath); sprintf(scanPath,"%s*.*",dirPath); mailDir = opendir(scanPath);
#define _MF_DIRECTORY_READDIR readentry = readdir(mailDir);  if (readentry == NULL) break;
#endif
#endif

	// check SCAN %i subdirectories for left-over files ...
	for (iScanDir = 0; iScanDir < MF_GlobalConfiguration.MailscanDirNum; iScanDir++)
	{
		sprintf(scanDir,"%s"PS"MFSCAN"PS"%04i"PS,MF_GlobalConfiguration.MFLTRoot.c_str(),iScanDir);
		_MF_DIRECTORY_OPENDIR(scanDir);

		while ( mailDir != NULL )
		{
			_MF_DIRECTORY_READDIR;
			if (_MF_DIRECTORY_FILENAME[0] == '.')
				continue;
			
			if ( stricmp(_MF_DIRECTORY_FILENAME,"lock.mfs") == 0 )
			{
				MFD_Out(MFD_SOURCE_VSCAN,"Found stale lockfile for %d\n",iScanDir);
				
				sprintf(fileIn,"%s"PS"MFSCAN"PS"%04i"PS"%s",MF_GlobalConfiguration.MFLTRoot.c_str(),iScanDir,"lock.mfs");
				
				MFD_Out(MFD_SOURCE_VSCAN,"Delete: %s\n",fileIn);
				unlink(fileIn);
			}
			else
			{
				struct stat st;
				char szTemp[1024];
				time_t mintime;
				mintime = time(NULL) - (2*(24*60*60));

				stat(_MF_DIRECTORY_FILENAME,&st);
				if (st.st_mtime < mintime)
				{

					sprintf(szTemp,"Found old file '%s' in a queue. Deleting.",_MF_DIRECTORY_FILENAME);
					MF_StatusText(szTemp);
				
					sprintf(fileIn,"%s"PS"MFSCAN"PS"%04i"PS"%s",MF_GlobalConfiguration.MFLTRoot.c_str(),iScanDir,_MF_DIRECTORY_FILENAME);
					MFD_Out(MFD_SOURCE_VSCAN,"Delete: %s\n",fileIn);
					unlink(fileIn);
				}
			}
		}
		if (mailDir != NULL)		closedir(mailDir);
	}

	MF_Worker_WSA_Startup();

/*	// MFSCAN Crash Recovery
	sprintf(scanDir,"%s"PS,MFT_MF_WorkDir);
	_MF_DIRECTORY_OPENDIR(scanDir);
	while ( mailDir != NULL )
	{
		_MF_DIRECTORY_READDIR;
		
		if ( _MF_DIRECTORY_FILENAME[0] != '.' )
		{
			sprintf(szTemp,"Found '%s' in MFWORK. -> MFPROB/CRASH.",_MF_DIRECTORY_FILENAME);
			MF_StatusText(szTemp);
			
			sprintf(fileIn,"%s%s",MFT_MF_WorkDir,_MF_DIRECTORY_FILENAME);
			sprintf(fileOut,"%sCRASH"PS"%s",MFT_MF_ProbDir,_MF_DIRECTORY_FILENAME);
			MFD_Out("moving:\n");
			MFD_Out(" from %s\n",fileIn);
			MFD_Out("   to %s\n",fileOut);
			MFD_Out(" rename returned: %d\n",rename(fileIn,fileOut));
		}
	}
	if (mailDir != NULL)		closedir(mailDir);
	// -- MFWORK
*/



	// Endless Loop ...
	while (MFT_NLM_Exiting == 0)
	{

		//* Send Directory ... *
		HandleGwiaDirectory(MFT_GWIA_SendDirIn,MFT_GWIA_SendDirOut,0,tlc);
		if (MFT_NLM_Exiting > 0)	break;

		//* Receive Directory ... *
		HandleGwiaDirectory(MFT_GWIA_RecvDirIn,MFT_GWIA_RecvDirOut,1,tlc);
		if (MFT_NLM_Exiting > 0)	break;
		
		tlc++;
		if (MFT_NLM_Exiting > 0)	break;

		// Results Directory ...
		{
		
			iXDir dir(MFT_GWIA_ResultDirIn);
			dir.SkipDotFiles = true;
			
			while ( dir.ReadNextEntry() )
			{
				const char* e = dir.GetCurrentEntryName();
				
				if (MFT_NLM_Exiting > 0)
					break;

				// Build filenames...
				sprintf(fileIn,  "%s%s", MFT_GWIA_ResultDirIn,  e);

				if (access(fileIn,W_OK) == 0)
					MF_MoveFileToDir(fileIn,MFT_GWIA_ResultDirOut,false);

				ThreadSwitch();
			}
			MF_StatusNothing();
		}

		if (MFT_NLM_Exiting > 0)	break;

		// Cycle Daily Log?
		MF_StatusCycleLog();
		if (tlc < 11)
		{
			// check if the user has a blocked lic. dont run then!
			if (!MF_GoOn()) {
					lc=0;
				} else {
					// check license
					lc = MFL_Init(tlc-1);
			}
			// mode checks...
			if ((tlc == 10) && (lc != 1))
			{
				if (lc == 0)
				{
					MF_DisplayCriticalError("\nMailFilter: Unexpected Error (Code: WTLLNE)\n");
					shallTerminate=255;
					break;
				} else {
					char* root = MF_GetRoot();
					FILE *fEv = NULL;
					int rCode = (( fEv = fopen(root,"r")) == NULL);
					free(root);

					if (rCode) {
						root = MF_GetRoot();
						fEv = fopen(root,"w");
						int rCode = ( fEv == NULL);
						int errcode = errno;
						free(root);
						
						if (rCode)
						{
							MF_DisplayCriticalError("\nMailFilter: Unexpected Error (Code: CEIFF)\n");
							shallTerminate=255;
							break;
						}
						putc(1,fEv);putc(0,fEv);putc(0,fEv);
						time_t ctime;
						time(&ctime);
						fprintf(fEv,"%d\n",ctime);
						fclose(fEv);
					} else {
						char ev[31];
						time_t etime; time_t ctime;
						time(&ctime);
						fread(ev,1,30,fEv);
						ev[0]=' ';ev[1]=' ';ev[2]=' ';
						ev[25]=0;
						etime = (time_t)atol(ev);
						if ( (ctime-etime) > 2592000 )
						{
							MF_DisplayCriticalError("\nMailFilter: *** Evaluation Period Expired! ***\n");
							shallTerminate=255;
							break;
						}
					}

					MFL_VerInfo();	// print eval info
				}
			} else {
				if (tlc == 10)
				{
					char* mfroot = MF_GetRoot();
					unlink(mfroot);
					free(mfroot);
				}
			}
		}

		// Defragment Memory.
#ifdef N_PLAT_NLM
		NWGarbageCollect ( MF_NLMHandle );
		NWGarbageCollect ( MF_NLMHandle );
#endif


		if (MFC_MAILSCAN_Enabled)
		{
			MFVS_CheckQueue();
		}

		if ((tlc > 10) && (lc == 1))	// Ensure Certification is Done.
		{
			if (!MFL_GetFlag(MAILFILTER_MC_M_VIRUSSCAN))
				MFC_MAILSCAN_Enabled = false;
				else
				{
					if ( (MF_GlobalConfiguration.MailscanDirNum == 0) || (MF_GlobalConfiguration.MailscanTimeout == 0) )
						MFC_MAILSCAN_Enabled = false;
						else
						MFC_MAILSCAN_Enabled = true;
				
				}
				
			if (!MFL_GetFlag(MAILFILTER_MC_M_BWTHCNTRL))
				MF_GlobalConfiguration.BWLScheduleTime = "";
		}

		//* Scheduling Directory: Send *
		_MF_DIRECTORY_OPENDIR(MFT_MF_ScheduleDirSend);
		while ( mailDir != NULL )
		{
			if (MFT_NLM_Exiting > 0)	break;

			_MF_DIRECTORY_READDIR;

			if ( _MF_DIRECTORY_FILENAME[0] != '.' )
			{
				MFD_Out(MFD_SOURCE_WORKER,"Schedule: Found %s for Sched.\n",_MF_DIRECTORY_FILENAME);
				// Build filenames...
				sprintf(fileIn,  "%s%s", MFT_MF_ScheduleDirSend, _MF_DIRECTORY_FILENAME);
				sprintf(fileOut, "%s%s", MFT_GWIA_SendDirOut,	 _MF_DIRECTORY_FILENAME);
				
				MFBW_CheckQueue(_MF_DIRECTORY_FILENAME,fileIn,fileOut);
			}
#ifdef N_PLAT_NLM
			ThreadSwitch();
#endif

		}
		if (mailDir != NULL) 		closedir(mailDir);
		MF_StatusNothing();

		if (MFT_NLM_Exiting > 0)	break;
		

		delay(MF_WORKER_SLEEPTIME);
#ifdef N_PLAT_NLM
		NXThreadYield();
#endif

		// Check for Invalid License.
		if ((tlc > 500) && (lc == -1))
		{
			MF_DisplayCriticalError("\nMailFilter: Unexpected Error (Code: TLWOLC)\n");
			shallTerminate=255;
			break;
		}

		//
		// Check for Tampering
		//
		if (tlc > 21600)		// ~ 4 hours ...
		{
			tlc = 0;
			if (lc != MFL_Certified)
			{
				// Break the Cycle.
				MF_DisplayCriticalError("\nMailFilter: Unexpected Error (Code: WTLDLC)\n");
				shallTerminate=255;
				break;
			}
		}
		
		// Do the next two things to hide code.
		MF_StatusCycleLog();
#ifdef N_PLAT_NLM
		NWGarbageCollect ( MF_NLMHandle );
#endif

		if (shallTerminate)
			break;		
	}

	MF_Worker_WSA_Shutdown();

	// Tell NLM that we've exited...
	MFT_NLM_ThreadCount--;

	if (shallTerminate > 200)
		raise(SIGTERM);

#ifdef WIN32
	return GetLastError();
#endif // WIN32

}

static char* MF_GetRoot()
{
	// points to ... eh yeah... somewhere in the filesystem ...
#ifdef N_PLAT_NLM
	#define _MF_GETROOT_SIZE 26
	char root[] = { ']',  'V', 'C', '+', 'N', 'L', 'Z', 'P', 'B', '@',
				    'Y',  'K', '_', 'G', 'Q', '[', ':', 'W', 'i', 'd',
				   0x06, 0x0D, 'm', 'k', '`',  0 };
#endif
#ifdef WIN32
	#define _MF_GETROOT_SIZE 37
	char root[] = {
		'M', '5', 'L', 'B', 'k', '`', '`', 'p', '{', '7', 'N', 'v', 'v',
		'n', 'q', 'x', '>', 'V', 'N', 'G', 'M', 'Q', 'I', 'D', 'R', 'N', 
		'G', 'G', 'v', 't', 'D', 'D', 'K', 1, 'C', 'H',  
		};
	
#endif	
	char out[_MF_GETROOT_SIZE];
	int cpos;

	for (cpos=0;cpos<(_MF_GETROOT_SIZE-1);cpos++)
	{
		out[cpos] = (char)((root[cpos]) ^ (cpos+14));
		out[cpos+1] = 0;
	}

	return strdup(out);

}

static int MF_GoOn()
{
#define keycount 1
#define keylen 64

	int cpos = 0;
	int cur = 0;
	char out[keylen+2];
	static int ret = 1;
	static int table[keycount][keylen] = {
/* N6BUNNAYXOJjGzNjNA0yNGrZFvUfMpUV5Wbe47mmd6ivkvhckd2ciG5u0JIx00H2 :: old laborde and neuner */
	 { 0x40, 0x39, 0x52, 0x44, 0x5c, 0x5d, 0x55, 0x4c, 0x4e, 0x58, 0x52, 0x73, 0x5d, 0x61, 0x52, 0x77, 0x50, 0x5e, 0x10, 0x58, 0x6c, 0x64, 0x56, 0x7f, 0x60, 0x51, 0x7d, 0x4f, 0x67, 0x5b, 0x79, 0x7b, 0x1b, 0x78, 0x52, 0x54, 0x06, 0x04, 0x59, 0x58, 0x52, 0x01, 0x51, 0x4f, 0x51, 0x4d, 0x54, 0x5e, 0x55, 0x5b, 0x72, 0x22, 0x2b, 0x04, 0x71, 0x30, 0x76, 0x0d, 0x01, 0x31, 0x7a, 0x7b, 0x04, 0x7f } //,
	};

	for (cur=0;cur<keycount;cur++)
	{	
		if (table[cur][0] == 0)
			break;
			
		// xor
		for (cpos=0;cpos<keylen;cpos++)
			out[cpos] = (char)((table[cur][cpos]) ^ (cpos+14));
			
		out[keylen+1]=0;
		out[keylen+2]=0;
	
		// check
		if (memicmp(MF_GlobalConfiguration.LicenseKey.c_str(),out,0))
			ret = 0;
			
		// zap it out
			// the xored keys
			for (cpos=0;cpos<keylen;cpos++)
				table[cur][cpos]=0;
			// the dexored output key
			for (cpos=0;cpos<(keylen+1);cpos++)
				out[cpos]=0;
	
	}
	
	return ret;
	
#undef keycount
#undef keylen
//	return 0;	/** outdated license **/
//	return 1;	/** not-blacklisted license **/
}

/*
 *
 *
 *  --- eof ---
 *
 *
 */
