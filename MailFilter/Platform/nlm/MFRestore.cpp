/****************************************************************************
**	File:	MFRestore.C
**
**	Desc:	MailFilter Mail Restore Console
**
**  (C) Copyright 2001-2002 Christian Hofstädtler. All Rights Reserved.
**							
*/

/****************************************************************************
**	Compiler setup.
*/

	#include "MailFilter.h"
	#include "MFMail.h++"
	#include "MFVersion.h"
	#include "MFConfig-defines.h"

	/*------------------------------------------------------------------------
	**	Variables
	*/
//	char	MF_ProductName[]	= "MailFilter/ax 1.5 Restore [NLM]";

	static void MFRestore_UI_ShowKeys(int keys);


int MF_ExamineFileMini(const char *fileIn, char *szMailFrom_, char *szMailRcpt_, char *szMailSubject_)
{

	FILE *mailFile				= NULL;
	int q						= 255;
	int curChr					= 0;
	int curPos					= 0;
	int curCmpPos				= 0;
	int iFirstNonSpaceCharacter = 0;
	char *pszFirstNonSpaceCharacter;
	
	char* szScanBuffer;
	
	bool  bFilterMatched		= false;
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
	szMailFrom 		=(char*)malloc(2002);
	szMailRcpt 		=(char*)malloc(2002);
	szMailSubject  	=(char*)malloc(2002);
	if (szScanBuffer == NULL)		return 239;
	if (szMailFrom == NULL)			return 239;
	if (szMailRcpt == NULL)			return 239;
	if (szMailSubject == NULL)		return 239;

	memset ( szScanBuffer	, 0 , 2000 );
	memset ( szMailFrom		, 0 , 2000 );
	memset ( szMailRcpt		, 0 , 2000 );
	memset ( szMailSubject	, 0 , 2000 );

	sprintf(szScanBuffer,"STRING INIT");
	// go to top of file
	fseek(mailFile,0,SEEK_SET);

	char *szMimeDest;
#define _EXAMINEFILE_DOMIMEHDRDECODE(szBuffer,iLen) szMimeDest = MIME_DecodeMimeHeader(szBuffer); if (szMimeDest != NULL) { strncpy(szBuffer,szMimeDest,iLen); free(szMimeDest); szMimeDest = NULL; szBuffer[iLen-1]=0; }

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

						while ( isspace(*pszFirstNonSpaceCharacter) && ((pszFirstNonSpaceCharacter-szScanBuffer)<245) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( szMailFrom , pszFirstNonSpaceCharacter , 245 );
						szMailFrom[245]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(szMailFrom,249);
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

						while ( isspace(*pszFirstNonSpaceCharacter) && ((pszFirstNonSpaceCharacter-szScanBuffer)<245) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( szMailFrom , pszFirstNonSpaceCharacter , 245 );
						szMailFrom[245]=0;
						
						_EXAMINEFILE_DOMIMEHDRDECODE(szMailFrom,249);
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

						while ( isspace(*pszFirstNonSpaceCharacter) && ((pszFirstNonSpaceCharacter-szScanBuffer)<245) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( szMailRcpt , pszFirstNonSpaceCharacter , 245 );
						szMailRcpt[245]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(szMailRcpt,249);
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

						while ( isspace(*pszFirstNonSpaceCharacter) && ((pszFirstNonSpaceCharacter-szScanBuffer)<245) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( szMailRcpt , pszFirstNonSpaceCharacter , 245 );
						szMailRcpt[245]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(szMailRcpt,249);
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

						while ( isspace(*pszFirstNonSpaceCharacter) && ((pszFirstNonSpaceCharacter-szScanBuffer)<1998) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( szMailSubject , pszFirstNonSpaceCharacter , 1998 );
						szMailSubject[1999]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(szMailSubject,1999);
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


	q=1;	//override just in case EOF has reached before we got Subject, Rcpt, From

	fseek(mailFile,0,SEEK_END);
	int mailSize = ftell(mailFile);
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

int MFRestore_RestoreFile(std::string szInFile, std::string szOutFile)
{
	FILE* inputFile;
	FILE* outputFile;
	char* szScanBuffer	   = NULL;

	bool bWroteXRestore = false;

	inputFile = fopen(szInFile.c_str(),"rb");
	if (inputFile == NULL)
		return -1;

	outputFile = fopen(szOutFile.c_str(),"wb");
	if (outputFile == NULL)
		return -1;

	szScanBuffer =	(char*)malloc(2002);
	if (szScanBuffer == NULL)		return -1;
	memset ( szScanBuffer	, 0 , 2000 );

#define _MF_NRM_RestoreFile_WriteRestoreHeader()	\
									if (!bWroteXRestore) { \
										time_t currentTime = time(NULL); \
										strftime  (szScanBuffer, 1999, "%a, %d %b %Y %T UTC", gmtime(&currentTime)); \
										fprintf(outputFile,"X-Restore: (%s) RESTORE at %s\r\n",MF_ProductName,szScanBuffer); \
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
	unlink(szInFile.c_str());
	
	return 0;
}
/****************************************************************************
** Show Details
*/
int MFRestore_ShowDetails(const char* fileName, int enableRestoreField)
{

   	unsigned long	line;
	char	szFilename [MAX_PATH+2];
	char	szDirection[MAX_PATH+2];
	std::string	szFileIn;
	std::string	szFileRestore;
	std::string	szFileRecheck;
	int		iDirection = 0;
	BOOL	newRestore = 0;
	BOOL	newRecheck = 0;
	int		rc = 0;

	if (MFT_NLM_Exiting)
		return -2;

	NWSStartWait(0,0,MF_NutInfo);

	szFileIn = MF_GlobalConfiguration.MFLTRoot + "\\MFPROB\\DROP\\";
	szFileIn += fileName;

	MailFilter_MailData* m = MailFilter_MailInit(
				fileName,
				(fileName[0] == 'S') ? MAILFILTER_MAILSOURCE_OUTGOING : MAILFILTER_MAILSOURCE_INCOMING
			);
	strcpy(m->szFileWork,szFileIn.c_str());
	rc = MF_ParseMail(m, true);
	
	MFD_Out(MFD_SOURCE_GENERIC,"file: %s\n",szFileIn.c_str());
	MFD_Out(MFD_SOURCE_GENERIC,"rc -> %d\n",rc);

	//if (MF_ExamineFileMini(szFileIn,szMailFrom,szMailRcpt,szMailSubject) != 1)
//	{
//		NWSDisplayErrorText (
//				MSG_ERROR_FILE_NOT_FOUND,
//				NUT_SEVERITY_FATAL,
//				MF_NutInfo
//			);
//		
//		return -2;
  //  }

	MFRestore_UI_ShowKeys(MFUI_KEYS_SAVE | MFUI_KEYS_SELECT);

	NWSEndWait(MF_NutInfo);

	NWSPushList(MF_NutInfo);
	NWSInitForm(MF_NutInfo);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 0;

	strncpy(szFilename,fileName,80);
	iDirection = (fileName[0] == 'S') ? 1 : 2;
	strcpy(szDirection,iDirection == 1 ? "Outgoing" : "Incoming");

	if (iDirection == 1)
	{
		szFileRestore = MF_GlobalConfiguration.MFLTRoot + "\\SEND\\";
		szFileRestore += fileName;
		szFileRecheck = MF_GlobalConfiguration.GWIARoot + "\\SEND\\";
		szFileRecheck += fileName;
	}
		
	if (iDirection == 2)
	{
		szFileRestore = MF_GlobalConfiguration.GWIARoot + "\\RECEIVE\\";
		szFileRestore += fileName;
		szFileRecheck = MF_GlobalConfiguration.MFLTRoot + "\\RECEIVE\\";
		szFileRecheck += fileName;
	}

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Mail Basename:", MF_NutInfo);
	NWSAppendCommentField (line, 22, (_MF_NUTCHAR)szFilename, MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Mail Direction:", MF_NutInfo);
	NWSAppendCommentField (line, 22, (_MF_NUTCHAR)szDirection, MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"File and Path:", MF_NutInfo);
	NWSAppendCommentField (line, 22, (_MF_NUTCHAR)szFileIn.c_str(), MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Restore Destination:", MF_NutInfo);
	NWSAppendCommentField (line, 22, (_MF_NUTCHAR)szFileRestore.c_str(), MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Mail From:", MF_NutInfo);
	NWSAppendCommentField (line, 22, (_MF_NUTCHAR)m->szMailFrom, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Mail To:", MF_NutInfo);
	NWSAppendCommentField (line, 22, (_MF_NUTCHAR)m->szMailRcpt, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Mail Subject:", MF_NutInfo);
	NWSAppendCommentField (line, 22, (_MF_NUTCHAR)m->szMailSubject, MF_NutInfo);
	line++;

	{
		char attachments[80];
		int cnt = 0;

		iXList_Storage* att;
		att = m->lstAttachments->GetFirst();
		while (att != NULL)
		{
			if (((char*)att->data)[0]==0)
				break;

			++cnt;

			att = m->lstAttachments->GetNext(att);
		}
		sprintf(attachments,"%d files",cnt);
		
		NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Attachments:", MF_NutInfo);
		NWSAppendCommentField (line, 22, (_MF_NUTCHAR)attachments, MF_NutInfo);
		line++;

	}

	if (enableRestoreField)	
	{
		line++;
		
		NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Restore this Mail:", MF_NutInfo);
		NWSAppendBoolField    (line, 22, NORMAL_FIELD, &newRestore, NULL, MF_NutInfo);
		line++;
		
		NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Queue for Re-Check:", MF_NutInfo);
		NWSAppendBoolField    (line, 22, NORMAL_FIELD, &newRecheck, NULL, MF_NutInfo);
		line++;
	}	


	rc = NWSEditForm (
		MSG_PROGRAM_NAME,				//headernum
		25-line-4-3,				//line
		0,							//col
		line+4,						//portalHeight
		80,							//portalWidth
		line+1,						//virtualHeight,
		78,							//virtualWidth,
		false,						//ESCverify,
		false,						//forceverify,
		MSG_SAVE_CHANGES,			//confirmMessage,
		MF_NutInfo
	);

	NWSDestroyForm	(MF_NutInfo);
	NWSPopList		(MF_NutInfo);

	MailFilter_MailDestroy(m);

	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_SHOW);

	if (rc)
	{
		if ( newRestore && newRecheck )
		{
			NWSDisplayErrorText ( MSG_ERROR_ONLY_ONE_OF_RESTORE_RECHECK , NUT_SEVERITY_INFORM , MF_NutInfo );
			return -1;
		} else {

			if (newRestore)
			{
				MFRestore_RestoreFile(szFileIn,szFileRestore);
				return -2;
			}
			if (newRecheck)
			{
				MFRestore_RestoreFile(szFileIn,szFileRecheck);
				return -2;
			}
		}
	}
	
	return -1;

}

/****************************************************************************
** Show Mail File (4k max)
*/
int MFRestore_ShowFile(const char* fileName)
{
#ifdef __NOVELL_LIBC__
	char szBuffer[8001];
#else
	unsigned char szBuffer[8001];
#endif	
	FILE* fp;
	unsigned int size = 0;
	
	if (MFT_NLM_Exiting)
		return -2;

	fp = fopen(fileName,"rt");
	if (fp == NULL)
	{
			NWSDisplayInformation (
				NULL,
				5,
				0,
				0,
				ERROR_PALETTE,
				VREVERSE,
				(_MF_NUTCHAR)"Unable to open specified file!",
				MF_NutInfo
				);
		
		return -2;	
	}
	
	
	do {
	
		if (feof(fp))
			break;
		
		fgets((char*)szBuffer+size,(8000-(int)size),fp);
		
		
	} while ( (size=strlen((char*)szBuffer)) < 7999);
	fclose(fp);
	szBuffer[8000]=0;
	
	if(MFT_NLM_Exiting)
		return -2;

	MFRestore_UI_ShowKeys(MFUI_KEYS_SAVE | MFUI_KEYS_SELECT);

	NWSPushList	(MF_NutInfo);

	NWSViewText	(0,0,20,78,MSG_VIEW_MAILFILE_HEADER,szBuffer,8000,MF_NutInfo);
	
	NWSPopList	(MF_NutInfo);

	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_SHOW);

	return (-1);
}

/****************************************************************************
** Restore List Action
*/
int MFRestore_RstList_Act(LONG keyPressed, LIST **elementSelected,
		LONG *itemLineNumber, void *actionParameter)
	{

	if(MFT_NLM_Exiting)
		return 0;

	if (keyPressed == M_ESCAPE)
		return 0;
		
	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_SHOW);

	itemLineNumber=itemLineNumber;			/* Rid compiler warning */
	actionParameter=actionParameter;		/* Rid compiler warning */

	switch(keyPressed)
		{
		case M_ESCAPE:
			return 0;
			break;

		case M_CYCLE:
			if (*elementSelected != NULL)
				return MFRestore_ShowFile((char *)((*elementSelected)->otherInfo));
			break;

		case M_SELECT:
			if (*elementSelected != NULL)
				return MFRestore_ShowDetails((char *)((*elementSelected)->otherInfo),true);
			break;
		default:
			return(-1);
		}

	return(-2);
	}


/****************************************************************************
** Restore List
*/
void NLM_Main(void)
	{

	/*------------------------------------------------------------------------
	**	Don't do this if we should be exiting.
	*/
	if(MFT_NLM_Exiting)
		return;

	/*------------------------------------------------------------------------
	**	At this point, the current list is uninitialized (being that it is the
	** first list of the program.)  Before using the current list it must be
	**	initialized (set head and tail to NULL) by calling InitMenu().
	**	Note that Lists use NWInitList() and Menus use NWInitMenu().
	*/
	char						szFile[80+1];
	char						szDate[80+1];
	char						szList[80+1];
	long						rc = 0;
	char*						cI = NULL;
	

//	char scanDir[MAX_PATH];
	char scanPath[MAX_PATH];
	
	// Init Lists
	NWSPushList(MF_NutInfo);
	NWSInitList(MF_NutInfo, free);

	// init function keys ...
	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_SHOW);
	
	/*------------------------------------------------------------------------
	** Look for dropped mails and put them in the list.
	*/
	
	sprintf(scanPath,"%s\\MFPROB\\DROP",MF_GlobalConfiguration.MFLTRoot.c_str());
	chdir(scanPath);
	
	struct tm* time;
	long lTime;
	
	iXDir dir(scanPath);
	const char* e;
	
	while ( dir.ReadNextEntry() )
	{
		if (MFT_NLM_Exiting > 0)	break;

		e = dir.GetCurrentEntryName();
		lTime = dir.GetCurrentEntryModificationTime();

		cI = (char*)malloc(sizeof(e));
		strcpy(cI,(e));
		
		strcpy (szFile,e);
		szFile[13] = 0;
		
//		strftime (szDate, 60, "%04d/%02d/%02d %02d:%02d",  );   
		time = localtime(&lTime);

		sprintf ( szDate, "%04d/%02d/%02d %02d:%02d", time->tm_year , time->tm_mon , time->tm_mday,
														time->tm_hour , time->tm_min );
		
		sprintf ( szList, " %s | %s | %s | %6d kB ", (szFile[0] == 'S') ? "Out" : " In", szDate, szFile, ((dir.GetCurrentEntrySize())/1024)+1 );
		
		NWSAppendToList ( (_MF_NUTCHAR)szList, cI, MF_NutInfo );

		ThreadSwitch();
	}
	

	if (MFT_NLM_Exiting > 0)
		return;

	// Display the list and allow user interaction.
	rc = (long) NWSList(
		/* I-	header			*/	MSG_PROGRAM_NAME,
		/* I-	centerLine		*/	5,
		/* I-	centerColumn	*/	1,
		/* I-	height			*/	16,
		/* I-	width			*/  80,
		/* I-	validKeyFlags	*/	M_ESCAPE|M_SELECT|M_CYCLE, //|M_NO_SORT,
		/* IO	element			*/	0,
		/* I-	handle			*/  MF_NutInfo,
		/* I-	formatProcedure	*/	NULL,
		/* I-	actionProcedure	*/	MFRestore_RstList_Act,
		/* I-	actionParameter	*/	NULL
		);
	
	// free...
	NWSDestroyList(MF_NutInfo);
	NWSPopList(MF_NutInfo);

	// Really Exit? 
	if (rc == -2)
		NLM_Main();

	return;
	}


static void MFRestore_UI_ShowKeys(int keys)
{
	char szTemp[80+2];

	memset(szTemp,' ',80);
	szTemp[81]=0;

	strcpy(szTemp+1,"GWIA: ");
	strncpy(szTemp+7,MF_GlobalConfiguration.GWIARoot.c_str(),60);
	NWSShowLineAttribute ( 22 , 0 , (_MF_NUTCHAR)szTemp , VREVERSE , 80 , (ScreenStruct*)MF_NutInfo->screenID );

	strcpy(szTemp+1,"MFLT: ");
	strncpy(szTemp+7,MF_GlobalConfiguration.MFLTRoot.c_str(),60);
	NWSShowLineAttribute ( 23 , 0 , (_MF_NUTCHAR)szTemp , VREVERSE , 80 , (ScreenStruct*)MF_NutInfo->screenID );
	
	MF_UI_ShowKeys(keys);
}

/****************************************************************************
** Program start.
*/

int MailFilter_Main_RunAppRestore(bool bStandalone)
{
	char szTemp[82];
	
	if (!MF_NutInit())
		return false;


	memset(szTemp,' ',80);
	szTemp[81]=0;
	sprintf(szTemp,"  MailFilter Mail Restore (NLM) Version %s", MAILFILTERVERNUM);
	szTemp[81]=0;
	NWSShowLineAttribute ( 0 , 0 , (_MF_NUTCHAR)szTemp , VNORMAL , 80 , (struct ScreenStruct*)MF_NutInfo->screenID );


	NWSDisplayInformation (
		NULL,
		4,
		0,
		0,
		ERROR_PALETTE,
		VREVERSE,
		(_MF_NUTCHAR)"WARNING: This Program is a potential security risk!\n\nREFRAIN FROM USING THIS TOOL IF YOU DO NOT KNOW WHAT YOU ARE DOING!\n\nYou can use this tool to bypass MailFilter Checks! This can lead to viruses in the system!\n\n",
		MF_NutInfo
		);


	NLM_Main();

	if (!bStandalone)
		MF_NutDeinit();
		
	return true;
}