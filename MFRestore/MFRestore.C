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
	/*------------------------------------------------------------------------
	**	ANSI
	*/
	#include <stdlib.h>		/* exit()	*/
	#include <signal.h>		/* SIGTERM, signal() */
	#include <stdio.h>		/* sprintf() */
	#include <ctype.h>		/* isspace() */
	
	/*------------------------------------------------------------------------
	**	MFRestore
	*/
	#include "MFRestore.mlc"	/* Resource File */
	#include "MFRestore.mlh"	/* Resource File */
	#include "comi18n.h"
	
/*	#ifndef FALSE
	#define FALSE 0
	#endif
	#ifndef TRUE
	#define TRUE  1
	#endif
	#ifndef false
	#define false 0
	#endif
	#ifndef true
	#define true  1
	#endif
*/
	/*------------------------------------------------------------------------
	**	NetWare
	*/
	#include <nwsnut.h>		/* NWSRestoreNut(), NWSPushList(), ...*/
	#include <time.h>
	#include <unistd.h>		/* F_OK */


//	#define _WITH_LIBC
	#ifdef _WITH_LIBC
	#include <nks/thread.h>
	#include <netware.h>
	#include <screen.h>
	#include <stat.h>

	#define ConsolePrintf consoleprintf
	#define ThreadSwitch NXThreadYield
	#define ThreadSwitchWithDelay NXThreadYield

	#else

	#define NW_MAX_PORTABILITY
	#include <nwadv.h>		/* AllocatResourceTag() */
	#include <time.h>
	#include <nwadv.h>
	#include <nwdir.h>
	#include <nwfile.h>
	#include <nwtime.h>
	#include <nwstring.h>
	#include <conio.h>  	/* CreateScreen(), DestroyScreen() */
	#include <process.h>	/* GetNLMHandle() */
	#include <nit\nwenvrn.h>
	#include <nwfattr.h>    // for _MA_SERVER
	#include <string.h>
	
	#endif

/****************************************************************************
**	Global storage
*/
	/*------------------------------------------------------------------------
	**	Variables
	*/
	int		NLM_exiting 		=	false;
//	int		NLM_noUserInterface = 	false;
	int		NLM_threadCnt		=	0;
	NUTInfo *NLM_nutHandle 		=	NULL;
#ifdef _WITH_LIBC
	struct ResourceTagStructure*  screenResourceTag;
	scr_t	NLM_screenID;
#else
	int		NLM_screenID		=	(int)NULL;
#endif
	int		NLM_mainThreadGroupID;
//	char 	NLM_ServerName[_MAX_SERVER+1];
	char	MF_ProductName[]	= "MailFilter/ax 1.5 Restore [NLM]";

	typedef unsigned char BOOL;

	#define MF_Msg(id)		programMesgTable[id]
	#define MF_NMsg(id)		(unsigned char*)programMesgTable[id]
	
//	#define _MAX_PATH    255 /* maximum length of full pathname */
//	#define _MAX_SERVER  48  /* maximum length of server name */
//	#define _MAX_VOLUME  16  /* maximum length of volume component */
//	#define _MAX_DRIVE   3   /* maximum length of drive component */
//	#define _MAX_DIR     255 /* maximum length of path component */
//	#define _MAX_FNAME   9   /* maximum length of file name component */
//	#define _MAX_EXT     5   /* maximum length of extension component */
//	#define _MAX_NAME    13  /* maximum length of file name */

	/*
	**	MailFilter Configurator Status Store
	*/
/*	int MFC_CurrentList = 0;
*/
	#define MFUTIL_KEYS_NONE			0x0000
	#define MFUTIL_KEYS_IMPORT			0x0001
	#define MFUTIL_KEYS_EXPORT			0x0002
	#define MFUTIL_KEYS_NEW				0x0004
	#define MFUTIL_KEYS_DELETE			0x0008
	#define MFUTIL_KEYS_SELECT			0x0010
	#define MFUTIL_KEYS_EXIT			0x0020
	#define MFUTIL_KEYS_SAVE			0x0040
	#define MFUTIL_KEYS_CANCEL			0x0080
	#define MFUTIL_KEYS_SHOW			0x0100
	void MFUtil_UI_ShowKeys(int keys);

	/*------------------------------------------------------------------------
	**	MailFilter Config Storage
	*/
	#define MAILFILTER_VERSION7_MIGRATION	1
	#define _MAILFILTER_MFCONFIG_STORAGE
	#define _MAILFILTER_MFCONFIG_DONTWANTFILTERCACHE
	#include "../MailFilter/Config/MFConfig.h"

//	static MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];
//	static MailFilter_Filter id[MailFilter_MaxFilters];		// this is for sorting ONLY!

int MF_ConfigReadString(const char ConfigFile[MAX_PATH], int Entry, char* Value)
{
	FILE* cfgFile;
	size_t dRead;

	Value[0]=0;

	cfgFile = fopen(ConfigFile,"rb");

	if (cfgFile == NULL)
		return -2;

	fseek(cfgFile,Entry,SEEK_SET);
	
	dRead = fread(Value,sizeof(char),MAILFILTER_CONFIGURATION_LENGTH-2,cfgFile);
	
	fclose(cfgFile);
	Value[dRead+1]=0;

	Value[MAX_PATH-2]=0;

	return (int)(strlen(Value));
}


void MF_ConfigFree(void)
{
	if (MFC_GWIA_ROOT != NULL)					{	free(MFC_GWIA_ROOT);				MFC_GWIA_ROOT				= NULL;	}
	if (MFC_MFLT_ROOT != NULL)					{	free(MFC_MFLT_ROOT);				MFC_MFLT_ROOT				= NULL;	}
	if (MFC_ControlPassword != NULL)			{	free(MFC_ControlPassword);			MFC_ControlPassword			= NULL;	}
}

void MFU_strxor(char* string, char xorVal)
{
	if (string == NULL) return;
	
	do {
		if (*string == 0) break;
		
		*string = (char)(*string xor xorVal);
	} while(*string++);
}

// Reads the configuration ...
int MF_ConfigRead()
{
	int rc;
	struct stat statInfo;
	char* MFC_ConfigBuildStr;
	char* MFC_ConfigVersion;
	rc = 0;



	if (stat(MAILFILTER_CONFIGURATION_MAILFILTER,&statInfo))
		rc = 99;
	if (rc != 0)	goto MF_ConfigRead_ERR;

	if (statInfo.st_size < 3999)
		rc = 99;	
	if (rc != 0)	goto MF_ConfigRead_ERR;


	// Init Vars ...
	MFC_ConfigVersion = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_GWIA_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_MFLT_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_ControlPassword = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);

	MFC_GWIA_ROOT							[0] = 0;
	MFC_MFLT_ROOT							[0] = 0;
	MFC_ControlPassword						[0]	= 0;

	//
	// Read In Configuration Version/Build

	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 0, MFC_ConfigVersion) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}
	if (memicmp(MFC_ConfigVersion,"MAILFILTER_R001",15) != 0)
		rc = 99;
	MFC_ConfigBuildStr = (MFC_ConfigVersion + 16);
	MFC_ConfigBuild = atoi(MFC_ConfigBuildStr);

	free(MFC_ConfigVersion);

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 60, MFC_ControlPassword);

	//
	// Read In Configuration Values
	
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 1*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_GWIA_ROOT);

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 2*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_MFLT_ROOT);

	if ((!rc) && (MFC_ControlPassword[0] != 0))
	{
		unsigned char* readPass = (unsigned char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
		readPass[0] = 0;
		NWSEditString( 0, 0, 1, 50, 0 /* header */, EDIT_UTIL_GETCONTROLPASSWORD, readPass, 50, EF_MASK|EF_NOSPACES|EF_SET|EF_NOCONFIRM_EXIT, NLM_nutHandle, NULL, NULL, (unsigned char*)"a..z0..9A..Z" );
		
		strlwr((char*)readPass);
		MFU_strxor((char*)readPass,(char)18);
		if (strcmp((const char*)readPass, MFC_ControlPassword) != 0)
			rc = 500;
		free(readPass);
		
		if (rc)
		{
			NWSDisplayInformation (
				NULL,
				5,
				0,
				0,
				ERROR_PALETTE,
				VREVERSE,
				(unsigned char*)"Incorrect Password Specified!",
				NLM_nutHandle
				);
			
			return FALSE;
		}
	}
	
// Error Handling goes here ...
MF_ConfigRead_ERR:
	{
		if ( (rc) && (rc != 500) )
		{
			MF_ConfigFree();

			NWSDisplayErrorText ( ERROR_CONFIGURATION_INVALID , SEVERITY_FATAL , NLM_nutHandle , "" );
			ConsolePrintf("MFRestore: Error Code %d\n",rc);

			return FALSE;
		}
	}

	// No Errors! Register Cleanup ...

	atexit(MF_ConfigFree);

	return TRUE;
	
}

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

int MFRestore_RestoreFile(const char* szInFile, const char* szOutFile)
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
	
#ifndef WIN32
	flushall();
#endif
	fclose(inputFile);
	fclose(outputFile);
	free(szScanBuffer);
	unlink(szInFile);
	
	return 0;
}
/****************************************************************************
** Show Details
*/
int MFRestore_ShowDetails(char* fileName, int enableRestoreField)
{

   	unsigned long	line;
	char	szFilename [MAX_PATH+2];
	char	szDirection[MAX_PATH+2];
	char	szFileIn [MAX_PATH+2];
	char	szFileOut[MAX_PATH+2];
	int		iDirection = 0;
	BOOL	newRestore = 0;
	int		rc = 0;
	char	szMailFrom[252];
	char	szMailRcpt[252];
	char	szMailSubject[252];

	if(NLM_exiting)
		return -2;


	sprintf(szFileIn,"%s\\MFPROB\\DROP\\%s",MFC_MFLT_ROOT,fileName);
	if (MF_ExamineFile(szFileIn,szMailFrom,szMailRcpt,szMailSubject) != 1)
	{
			NWSDisplayErrorText (
				ERROR_FILE_NOT_FOUND,
				SEVERITY_FATAL,
				NLM_nutHandle
				);
		
		return -2;
    }

	MFUtil_UI_ShowKeys(MFUTIL_KEYS_SAVE | MFUTIL_KEYS_SELECT);

	NWSPushList(NLM_nutHandle);
	NWSInitForm(NLM_nutHandle);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 0;

	strncpy(szFilename,fileName,80);
	iDirection = (fileName[0] == 'S') ? 1 : 2;
	strcpy(szDirection,iDirection == 1 ? "Outgoing" : "Incoming");

	if (iDirection == 1)
		sprintf(szFileOut,"%s\\SEND\\%s",MFC_MFLT_ROOT,fileName);
	if (iDirection == 2)
		sprintf(szFileOut,"%s\\RECEIVE\\%s",MFC_GWIA_ROOT,fileName);

	NWSAppendCommentField (line, 1, (unsigned char*)"Mail Basename:", NLM_nutHandle);
	NWSAppendCommentField (line, 22, (unsigned char*)szFilename, NLM_nutHandle);
	line++;
	
	NWSAppendCommentField (line, 1, (unsigned char*)"Mail Direction:", NLM_nutHandle);
	NWSAppendCommentField (line, 22, (unsigned char*)szDirection, NLM_nutHandle);
	line++;
	
	NWSAppendCommentField (line, 1, (unsigned char*)"File and Path:", NLM_nutHandle);
	NWSAppendCommentField (line, 22, (unsigned char*)szFileIn, NLM_nutHandle);
	line++;
	
	NWSAppendCommentField (line, 1, (unsigned char*)"Restore Destination:", NLM_nutHandle);
	NWSAppendCommentField (line, 22, (unsigned char*)szFileOut, NLM_nutHandle);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (unsigned char*)"Mail From:", NLM_nutHandle);
	NWSAppendCommentField (line, 22, (unsigned char*)szMailFrom, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 1, (unsigned char*)"Mail To:", NLM_nutHandle);
	NWSAppendCommentField (line, 22, (unsigned char*)szMailRcpt, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 1, (unsigned char*)"Mail Subject:", NLM_nutHandle);
	NWSAppendCommentField (line, 22, (unsigned char*)szMailSubject, NLM_nutHandle);
	line++;

	if (enableRestoreField)	
	{
		line++;
		
		NWSAppendCommentField (line, 1, (unsigned char*)"Restore this Mail:", NLM_nutHandle);
		NWSAppendBoolField    (line, 22, NORMAL_FIELD, &newRestore, NULL, NLM_nutHandle);
		line++;
	}	

	rc = NWSEditForm (
		PROGRAM_NAME,				//headernum
		25-line-4-3,				//line
		0,							//col
		line+4,						//portalHeight
		80,							//portalWidth
		line+1,						//virtualHeight,
		78,							//virtualWidth,
		false,						//ESCverify,
		false,						//forceverify,
		MSG_SAVE_CHANGES,			//confirmMessage,
		NLM_nutHandle
	);

	NWSDestroyForm	(NLM_nutHandle);
	NWSPopList		(NLM_nutHandle);

	MFUtil_UI_ShowKeys(MFUTIL_KEYS_EXIT|MFUTIL_KEYS_SELECT|MFUTIL_KEYS_SHOW);

	if (rc)
	{
		if (newRestore)
		{
			MFRestore_RestoreFile(szFileIn,szFileOut);
			return -2;
		}
	}

	return -1;

}

/****************************************************************************
** Show Mail File (4k max)
*/
int MFRestore_ShowFile(char* fileName)
{
	unsigned char szBuffer[8001];
	FILE* fp;
	unsigned int size = 0;
	
	if(NLM_exiting)
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
				(unsigned char*)"Unable to open specified file!",
				NLM_nutHandle
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
	
	if(NLM_exiting)
		return -2;

	MFUtil_UI_ShowKeys(MFUTIL_KEYS_SAVE | MFUTIL_KEYS_SELECT);

	NWSPushList	(NLM_nutHandle);

	NWSViewText	(0,0,20,78,VIEW_MAILFILE_HEADER,szBuffer,8000,NLM_nutHandle);
	
	NWSPopList	(NLM_nutHandle);

	MFUtil_UI_ShowKeys(MFUTIL_KEYS_EXIT|MFUTIL_KEYS_SELECT|MFUTIL_KEYS_SHOW);

	return (-1);
}

/****************************************************************************
** Restore List Action
*/
int MFRestore_RstList_Act(LONG keyPressed, LIST **elementSelected,
		LONG *itemLineNumber, void *actionParameter)
	{

	if(NLM_exiting)
		return 0;

	if (keyPressed == M_ESCAPE)
		return 0;
		
	MFUtil_UI_ShowKeys(MFUTIL_KEYS_EXIT|MFUTIL_KEYS_SELECT|MFUTIL_KEYS_SHOW);

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
	if(NLM_exiting)
		return;

	/*------------------------------------------------------------------------
	**	At this point, the current list is uninitialized (being that it is the
	** first list of the program.)  Before using the current list it must be
	**	initialized (set head and tail to NULL) by calling InitMenu().
	**	Note that Lists use NWInitList() and Menus use NWInitMenu().
	*/
	DIR *						mailDir;
	char						szFile[80+1];
	char						szDate[80+1];
//	char						szTemp[80+1];
	char						szList[80+1];
	long						rc = 0;
	char*						cI = NULL;
//	struct NW_DATE						sDate;
	

	char scanDir[MAX_PATH];
	char scanPath[MAX_PATH];
	
	// Init Lists
	NWSPushList(NLM_nutHandle);
	NWSInitList(NLM_nutHandle, free);

	// init function keys ...
	MFUtil_UI_ShowKeys(MFUTIL_KEYS_EXIT|MFUTIL_KEYS_SELECT|MFUTIL_KEYS_SHOW);
	
	/*------------------------------------------------------------------------
	** Look for dropped mails and put them in the list.
	*/
	
	sprintf(scanPath,"%s\\MFPROB\\DROP",MFC_MFLT_ROOT);
	chdir(scanPath);
	
	sprintf(scanDir,"%s\\*.*",scanPath);
	mailDir = opendir(scanDir);
	
	while ( mailDir != NULL )
	{
		if (NLM_exiting > 0)	break;

		mailDir = readdir(mailDir);
		if (mailDir == NULL) { break;	} // No more files.

		cI = (char*)malloc(sizeof(mailDir->d_nameDOS));
		strcpy(cI,(mailDir->d_nameDOS));
		
		strcpy (szFile,mailDir->d_nameDOS);
		szFile[13] = 0;

//		NWUnpackDate ( mailDir->d_date , sDate );
//		strftime ( szDate, 80, "%d.%m.%Y", (tm*)mailDir->d_cdatetime );
		sprintf ( szDate, "%04d/%02d/%02d %02d:%02d", GET_YEAR_FROM_DATE(mailDir->d_date)+1980 , GET_MONTH_FROM_DATE(mailDir->d_date) , GET_DAY_FROM_DATE(mailDir->d_date),
														GET_HOUR_FROM_TIME(mailDir->d_time) , GET_MINUTE_FROM_TIME(mailDir->d_time) );
		
		sprintf ( szList, " %s | %s | %s | %6d kB ", (szFile[0] == 'S') ? "Out" : " In", szDate, szFile, ((mailDir->d_size)/1024)+1 );
		
		NWSAppendToList ( (unsigned char*)szList, cI, NLM_nutHandle );


#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif

	}
	closedir(mailDir);

	if (NLM_exiting > 0)
		return;

	// Display the list and allow user interaction.
	rc = (long) NWSList(
		/* I-	header			*/	PROGRAM_NAME,
		/* I-	centerLine		*/	5,
		/* I-	centerColumn	*/	1,
		/* I-	height			*/	16,
		/* I-	width			*/  80,
		/* I-	validKeyFlags	*/	M_ESCAPE|M_SELECT|M_CYCLE, //|M_NO_SORT,
		/* IO	element			*/	0,
		/* I-	handle			*/  NLM_nutHandle,
		/* I-	formatProcedure	*/	NULL,
		/* I-	actionProcedure	*/	MFRestore_RstList_Act,
		/* I-	actionParameter	*/	NULL
		);
	
	// free...
	NWSDestroyList(NLM_nutHandle);
	NWSPopList(NLM_nutHandle);

	// Really Exit? 
	if (rc == -2)
		NLM_Main();

	return;
	}

/****************************************************************************
** Program shut-down.
*/
void NLM_ShutDown(void)
	{

	/*------------------------------------------------------------------------
	**	Shut down NWSNUT.
	*/
	if(NLM_nutHandle != NULL)
		NWSRestoreNut(NLM_nutHandle);

	/*------------------------------------------------------------------------
	**	We won't need this screen any more.
	*/
#ifndef _WITH_LIBC
	if(NLM_screenID != NULL)
		DestroyScreen(NLM_screenID);
#else
		CloseScreen(NLM_screenID);
#endif

	return;
	}

/****************************************************************************
** Program signal handler.
*/
void NLM_SignalHandler(int sig)
	{
	int handlerThreadGroupID;
	time_t t;

	switch(sig)
		{
  		/*---------------------------------------------------------------------
		**	SIGTERM is called when this NLM is unloaded via console command.
		*/
		case SIGTERM:

			/*------------------------------------------------------------------
			**	Tell all of our threads to "get out of the pool."
			*/
			NLM_exiting=TRUE;

			/*------------------------------------------------------------------
			** Wait for main() to terminate.
			**
			** If main() has not terminateded within a 1/2 second, ungetch an
			**	escape key.  This will "trick" a blocking NWSList() or NWSMenu()
			**	function and wake it up. 
			*/
#ifndef _WITH_LIBC
			handlerThreadGroupID = SetThreadGroupID(NLM_mainThreadGroupID);

			t=time(NULL);
			while(NLM_threadCnt != 0)
				{

				if(difftime(time(NULL), t) > 0.5)
					{
					ungetch(27);
					t=time(NULL);
					}

				ThreadSwitchWithDelay();
				}

			SetThreadGroupID(handlerThreadGroupID);
#else
			t=time(NULL);
			while(NLM_threadCnt != 0)
				{

				if(difftime(time(NULL), t) > 0.5)
					{
					ungetcharacter(27);
					t=time(NULL);
					}

				NXThreadYield();
				}

#endif
			break;

		}

	return;
	}

/****************************************************************************
** Program initialization.
*/
void NLM_Initialize(void)
	{
	LONG	cCode;
#ifdef _WITH_LIBC
	struct LoadDefinitionStructure* NLMHandle;
#else
	int NLMHandle;
#endif
	LONG	tagID;
	int curItem=0;

	/*------------------------------------------------------------------------
	**	Register SIGTERM hanler to facilitate a console UNLOAD command.
	*/
	signal(SIGTERM, NLM_SignalHandler);

	/*------------------------------------------------------------------------
	**	Get misc. values.
	*/
#ifndef _WITH_LIBC
	NLMHandle=(int)GetNLMHandle();
	NLM_mainThreadGroupID = GetThreadGroupID();				/* Needed by our SIGTERM handler   */
#else
	NLMHandle=(struct LoadDefinitionStructure*)getnlmhandle();
#endif

	/*------------------------------------------------------------------------
	**	Build our own screen.  This is not required.  I could have used just
	**	had the NLM use an automatically created screen, but I set the linker
	**  option to screen="NONE" (QMK386 /x option) and included this code so
	**	that you could see how to create your own screen.
	*/
//	NLM_screenID=GetCurrentScreen();
#ifdef _WITH_LIBC
	screenResourceTag = AllocateResourceTag(NLMHandle, programMesgTable[SCREEN_NAME], ScreenSignature);
	if (screenResourceTag == NULL) {
		OutputToScreen(CONSOLE_SCREEN,"MFRestore Error: Unable to allocate resource tag.\r\n");
	}
	OpenScreen(programMesgTable[SCREEN_NAME], screenResourceTag, &NLM_screenID);
	if (NLM_screenID == NULL) {
		OutputToScreen(CONSOLE_SCREEN, "MFRestore Error: Could not open screen.\r\n");
	}
#else
	NLM_screenID=CreateScreen(
		/*	screenName			*/	programMesgTable[SCREEN_NAME],
		/*	attributes			*/	AUTO_DESTROY_SCREEN
		);
	if(NLM_screenID == NULL)
		{
		NLM_exiting = TRUE;
		goto END;
		}
#endif

	/*------------------------------------------------------------------------
	**	Fire up NWSNUT on our screen.
	*/
	tagID=(unsigned long)AllocateResourceTag(
#ifdef _WITH_LIBC
		/*	NLMHandle			*/	(void*)NLMHandle,
		/*	descriptionString	*/	(const char *)programMesgTable[RS_TAG_NAME],
#else
		/*  NLMHandle			*/  (unsigned long)NLMHandle,
		/*	descriptionString	*/	(const unsigned char*)programMesgTable[RS_TAG_NAME],
#endif
		/*	resourceType		*/	AllocSignature
		);
	if(tagID == NULL)
		{
		ConsolePrintf("MFRestore: Error creating Resource Tag!\n");
		NLM_exiting = TRUE;
		goto END;
		}

	cCode=(unsigned long)NWSInitializeNut(
		/*	utility				*/	PROGRAM_NAME,
		/*	version				*/	PROGRAM_VERSION,
		/*	headerType			*/	SMALL_HEADER,
		/*	compatibilityType	*/	NUT_REVISION_LEVEL,
		/*	messageTable		*/	(unsigned char**)programMesgTable,
		/*	helpScreens			*/	NULL,
		/*	screenID			*/	(int)NLM_screenID,
		/*	resourceTag			*/	tagID,
		/*	handle				*/	&NLM_nutHandle
		);
	if(cCode != NULL)
		{
		ConsolePrintf("MFRestore: FATAL ERROR: NWSInitializeNut returned %d.\n",cCode);
		NLM_exiting = TRUE;
		goto END;
		}

//	NWSSetErrorLabelDisplayFlag ( 0 , NLM_nutHandle );	

	/*------------------------------------------------------------------------
	**	Set our screen as current & active.
	*/
#ifndef _WITH_LIBC
	DisplayScreen(NLM_screenID);
#else
	ActivateScreen(NLM_screenID);
#endif

END:
	return;
	}

void MFUtil_UI_ShowKeys(int keys)
{
	char szTemp[80+2];
	int curItem;
	memset(szTemp,' ',80);
	szTemp[81]=0;

	strcpy(szTemp+1,"GWIA: ");
	strncpy(szTemp+7,MFC_GWIA_ROOT,60);
	NWSShowLineAttribute ( 22 , 0 , (unsigned char*)szTemp , VREVERSE , 80 , (ScreenStruct*)NLM_nutHandle->screenID );

	strcpy(szTemp+1,"MFLT: ");
	strncpy(szTemp+7,MFC_MFLT_ROOT,60);
	NWSShowLineAttribute ( 23 , 0 , (unsigned char*)szTemp , VREVERSE , 80 , (ScreenStruct*)NLM_nutHandle->screenID );

	memset(szTemp,' ',80);
	szTemp[81]=0;
	NWSShowLineAttribute ( 24 , 0 , (unsigned char*)szTemp , VREVERSE , 80 , (ScreenStruct*)NLM_nutHandle->screenID );
	
	curItem=1;
	if (chkFlag(keys,MFUTIL_KEYS_IMPORT)) 	{	strncpy(szTemp+curItem, "<F8> Import ",12);		curItem=curItem+14;	}
	if (chkFlag(keys,MFUTIL_KEYS_EXPORT)) 	{	strncpy(szTemp+curItem, "<F9> Export ",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFUTIL_KEYS_NEW)) 		{	strncpy(szTemp+curItem, "<INS> New   ",12);		curItem=curItem+14;	}
	if (chkFlag(keys,MFUTIL_KEYS_DELETE)) 	{	strncpy(szTemp+curItem, "<DEL> Delete",12); 	curItem=curItem+14;	}
	if (chkFlag(keys,MFUTIL_KEYS_SELECT)) 	{	strncpy(szTemp+curItem, "<RET> Select",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFUTIL_KEYS_SHOW)) 	{	strncpy(szTemp+curItem, "<TAB> Detail",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFUTIL_KEYS_EXIT)) 	{	strncpy(szTemp+curItem, "<ESC> Exit  ",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFUTIL_KEYS_SAVE)) 	{	strncpy(szTemp+curItem, "<ESC> Save  ",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFUTIL_KEYS_CANCEL)) 	{	strncpy(szTemp+curItem, "<ESC> Cancel",12);		curItem=curItem+14; }

	NWSShowLineAttribute ( 24 , (unsigned long)((80-curItem)/2) , (unsigned char*)szTemp , VREVERSE , (unsigned long)curItem , (ScreenStruct*)NLM_nutHandle->screenID );

}

/****************************************************************************
** Program start.
*/

void main(int argc, char *argv[])
	{

	int action = 0;
	unsigned int tmp = 0;
	
	++NLM_threadCnt;

//	GetFileServerName(0, NLM_ServerName);

	NLM_Initialize();
	
	if (argc > 1)
	{
		ConsolePrintf ("MFREST.NLM: Using %s as Configuration Directory.\n",argv[1]);
		sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"%s\\CONFIG.BIN",argv[1]);
		sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"%s\\CONFIG.BAK",argv[1]);
		sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"%s\\CONFIG.ERR",argv[1]);
	} else {
		sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.BIN");
		sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.BAK");
		sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.ERR");								
	}

	if (MF_ConfigRead() == FALSE)
	{
		NLM_ShutDown();

		--NLM_threadCnt;
		return;
	}

	NWSDisplayInformation (
		NULL,
		4,
		0,
		0,
		ERROR_PALETTE,
		VREVERSE,
		(unsigned char*)"WARNING: This Program is a potential security risk!\n\nREFRAIN FROM USING THIS TOOL IF YOU DO NOT KNOW WHAT YOU ARE DOING!\n\nYou can use this tool to bypass MailFilter Checks! This can lead to viruses in the system!\n\n",
		NLM_nutHandle
		);


	NLM_Main();

	NLM_ShutDown();
	
	--NLM_threadCnt;

	return;
	}

