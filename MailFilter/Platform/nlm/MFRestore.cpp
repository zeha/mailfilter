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
	static void MFRestore_UI_ShowKeys(int keys);


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

	ThreadSwitch();

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

		ThreadSwitch();
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

	szFileIn = MF_GlobalConfiguration->MFLTRoot + "\\MFPROB\\DROP\\";
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
		szFileRestore = MF_GlobalConfiguration->MFLTRoot + "\\SEND\\";
		szFileRestore += fileName;
		szFileRecheck = MF_GlobalConfiguration->GWIARoot + "\\SEND\\";
		szFileRecheck += fileName;
	}
		
	if (iDirection == 2)
	{
		szFileRestore = MF_GlobalConfiguration->GWIARoot + "\\RECEIVE\\";
		szFileRestore += fileName;
		szFileRecheck = MF_GlobalConfiguration->MFLTRoot + "\\RECEIVE\\";
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
		NWSAppendBoolField    (line, 22, NORMAL_FIELD, &newRestore, 0, MF_NutInfo);
		line++;
		
		NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Queue for Re-Check:", MF_NutInfo);
		NWSAppendBoolField    (line, 22, NORMAL_FIELD, &newRecheck, 0, MF_NutInfo);
		line++;
	}	


	rc = NWSEditForm (
		MSG_MAIL_RESTORE,			//headernum
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

	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_DELETE|MFUI_KEYS_SHOW);

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

int MFRestore_AskDelete(const char* fileName)
{
	std::string szFileName;

	if (MFT_NLM_Exiting)
		return -2;

	szFileName = MF_GlobalConfiguration->MFLTRoot + "\\MFPROB\\DROP\\";
	szFileName += fileName;

	if (NWSConfirm(
			MSG_CONFIRM_DELETE,
			5, 0, 0, NULL, MF_NutInfo, NULL) == 1)
			{
				// yes, delete
				unlink(szFileName.c_str());
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
	std::string szFileName;
	FILE* fp;
	unsigned int size = 0;
	
	if (MFT_NLM_Exiting)
		return -2;

	szFileName = MF_GlobalConfiguration->MFLTRoot + "\\MFPROB\\DROP\\";
	szFileName += fileName;

	fp = fopen(szFileName.c_str(),"rt");
	if (fp == NULL)
	{
		NWSDisplayInformation (
			0,
			5,
			0,
			0,
			ERROR_PALETTE,
			VREVERSE,
			(_MF_NUTCHAR)"Unable to open the specified file!",
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

	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_DELETE|MFUI_KEYS_SHOW);

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
		
	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_DELETE|MFUI_KEYS_SHOW);

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

		case M_DELETE:
			if (*elementSelected != NULL)
				return MFRestore_AskDelete((char *)((*elementSelected)->otherInfo));
			break;

		default:
			return(-1);
		}

	return(-2);
	}


static void _mfr_free(void* foo)
{
	_mfd_free(foo,"mfrfree");
}

class RestoreListItem {
	public:
	 std::string filename;
	 time_t mtime;
	 long long size;
};
class CompareItems {
public:
	bool operator() 
			(const RestoreListItem& f1, 
			 const RestoreListItem& f2)
	{
		if (f1.mtime > f2.mtime)
			return true;
		return false;
	}
};

void listAddItem (const RestoreListItem item)
{
	
	char*						cI = NULL;
	char						szFile[80+1];
	char						szDate[80+1];
	char						szList[80+1];
	struct tm* time;

	cI = (char*)_mfd_malloc(strlen(item.filename.c_str())+1,"addEntry");
	strcpy(cI,(item.filename.c_str()));

	strncpy (szFile,item.filename.c_str(),13);
	szFile[13] = 0;
		
	if (item.mtime == -1)
	{
		sprintf ( szDate, "% 12s","- no datetime -");
	} else {
		time = localtime(&item.mtime);

		sprintf ( szDate, "%04d/%02d/%02d %02d:%02d", 
				time->tm_year+1900 ,
				time->tm_mon+1 , time->tm_mday ,
				time->tm_hour , time->tm_min );
	}
	
	sprintf ( szList, " %s | %s | %s | %6d kB ", (szFile[0] == 'S') ? "Out" : " In",
									szDate, szFile, ((item.size)/1024)+1 );
	
	NWSAppendToList ( (_MF_NUTCHAR)szList, cI, MF_NutInfo ); 
		
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
	long						rc = 0;
	
	char scanPath[MAX_PATH];
	
	ThreadSwitch();

	// Init Lists
	NWSPushList(MF_NutInfo);
	NWSInitList(MF_NutInfo, _mfr_free);

	// init function keys ...
	MFRestore_UI_ShowKeys(MFUI_KEYS_EXIT|MFUI_KEYS_SELECT|MFUI_KEYS_DELETE|MFUI_KEYS_SHOW);
	
	/*------------------------------------------------------------------------
	** Look for dropped mails and put them in the list.
	*/
	
	sprintf(scanPath,"%s\\MFPROB\\DROP",MF_GlobalConfiguration->MFLTRoot.c_str());
	chdir(scanPath);
	
	std::vector<RestoreListItem> list;
	list.clear();
	
	iXDir dir(scanPath);
	int i=0;
	while ( dir.ReadNextEntry() )
	{
		if (MFT_NLM_Exiting > 0)	break;
		
		RestoreListItem item;

		item.filename = dir.GetCurrentEntryName();
		item.mtime = dir.GetCurrentEntryModificationTime();
		item.size = dir.GetCurrentEntrySize();
		
		list.push_back(item);

		ThreadSwitch();
		
		MFD_Out(MFD_SOURCE_MAIL,"%d:%s ",++i,item.filename.c_str());	
	}
	sort(list.begin(),list.end(),CompareItems());
	for_each(list.begin(),list.end(),listAddItem);

	if (MFT_NLM_Exiting > 0)
		return;

	// Display the list and allow user interaction.
	rc = (long) NWSList(
		/* I-	header			*/	MSG_MAIL_RESTORE,
		/* I-	centerLine		*/	5,
		/* I-	centerColumn	*/	1,
		/* I-	height			*/	16,
		/* I-	width			*/  80,
		/* I-	validKeyFlags	*/	M_ESCAPE|M_SELECT|M_CYCLE|M_DELETE|M_NO_SORT,
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
	strncpy(szTemp+7,MF_GlobalConfiguration->GWIARoot.c_str(),60);
	NWSShowLineAttribute ( 22 , 0 , (_MF_NUTCHAR)szTemp , VREVERSE , 80 , (ScreenStruct*)MF_NutInfo->screenID );

	strcpy(szTemp+1,"MFLT: ");
	strncpy(szTemp+7,MF_GlobalConfiguration->MFLTRoot.c_str(),60);
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
		MSG_MAIL_RESTORE,
		4,
		0,
		0,
		ERROR_PALETTE,
		VREVERSE,
		(_MF_NUTCHAR)"WARNING: This Utility Program is a potential security risk!\n\nREFRAIN FROM USING THIS TOOL IF YOU DO NOT KNOW EXACTLY WHAT YOU ARE DOING!\n\nYou can use this tool to bypass MailFilter Checks! This can lead to viruses in the GroupWise system!\n\n",
		MF_NutInfo
		);


	NLM_Main();

	if (!bStandalone)
		MF_NutDeinit();
		
	return true;
}

