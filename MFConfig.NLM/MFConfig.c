/****************************************************************************
**	File:	MFCONFIG.C
**
**	Desc:	MailFilter Configuration NLM
**
**  (C) Copyright 2000-2004 Christian Hofstädtler. All Rights Reserved.
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
	#include <string.h>

	/*------------------------------------------------------------------------
	**	MFConfig
	*/
	#include "MFConfig.mlc"	/* File maintained by MSGLIB.EXE */
	#include "MFConfig.mlh"	/* File maintained by MSGLIB.EXE */
	#include "mfpre.h"
	
	#ifndef FALSE
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

	#include <stddef.h>

	/*------------------------------------------------------------------------
	**	NetWare
	*/
	#include <nwsnut.h>		/* NWSRestoreNut(), NWSPushList(), ...*/
	#include <time.h>
	#include <unistd.h>		/* F_OK */

//	#define __NOVELL_LIBC__
#ifdef __NOVELL_LIBC__
	#define _WITH_LIBC
	#define NUTCHAR char*
	typedef int BOOL;
#else
	#define NUTCHAR unsigned char*
	typedef unsigned char BOOL;
#endif

	#ifdef _WITH_LIBC
	#define _MAX_SERVER 17
	#include <nks/thread.h>
	#include <netware.h>
	#include <screen.h>
	#include <sys/stat.h>
	#include <utsname.h>

	#define ConsolePrintf printf
	#define ThreadSwitch NXThreadYield
	#define ThreadSwitchWithDelay NXThreadYield

	#else

	#include <nwadv.h>		/* AllocatResourceTag() */
	#include <nwmisc.h> 	/* LongSwap() */
	#include <obsolete\conio.h>  	/* CreateScreen(), DestroyScreen() */
	#include <obsolete\process.h>	/* GetNLMHandle() */
	#include <nit\nwenvrn.h>
	#include <nwfattr.h>    /* for _MA_SERVER */
	#include <string.h>
	#include <sys/stat.h>
	
	#endif

/****************************************************************************
**	Global storage
*/
	/*------------------------------------------------------------------------
	**	Variables
	*/
	int		NLM_exiting 	=	FALSE;
	int		NLM_noUserInterface = FALSE;
	int		NLM_threadCnt	=	0;
	NUTInfo *NLM_nutHandle 	=	NULL;
#ifdef _WITH_LIBC
	struct ResourceTagStructure*  screenResourceTag;
	void*	NLM_screenID;
#else
	int		NLM_screenID	=	(int)NULL;
#endif
	int		NLM_mainThreadGroupID;
	char 	NLM_ServerName[_MAX_SERVER+1];

	#define MF_Msg(id)		programMesgTable[id]
	#define MF_NMsg(id)		(NUTCHAR)programMesgTable[id]
	
	#define _MAX_PATH    255 /* maximum length of full pathname */
	#define _MAX_SERVER  48  /* maximum length of server name */
	#define _MAX_VOLUME  16  /* maximum length of volume component */
	#define _MAX_DRIVE   3   /* maximum length of drive component */
	#define _MAX_DIR     255 /* maximum length of path component */
	#define _MAX_FNAME   9   /* maximum length of file name component */
	#define _MAX_EXT     5   /* maximum length of extension component */
	#define _MAX_NAME    13  /* maximum length of file name */

	/*
	**	MailFilter Configurator Status Store
	*/
	int MFC_CurrentList = 0;

	#define MFCONFIG_KEYS_NONE			0x0000
	#define MFCONFIG_KEYS_IMPORT		0x0001
	#define MFCONFIG_KEYS_EXPORT		0x0002
	#define MFCONFIG_KEYS_NEW			0x0004
	#define MFCONFIG_KEYS_DELETE		0x0008
	#define MFCONFIG_KEYS_SELECT		0x0010
	#define MFCONFIG_KEYS_EXIT			0x0020
	#define MFCONFIG_KEYS_SAVE			0x0040
	#define MFCONFIG_KEYS_CANCEL		0x0080
	void MFConfig_UI_ShowKeys(int keys);

	/*------------------------------------------------------------------------
	**	MailFilter Config Storage
	*/
	#define _MAILFILTER_MFCONFIG_STORAGE
	#include "../MailFilter/Config/MFConfig.h"

	static MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];
	static MailFilter_Filter id[MailFilter_MaxFilters];		// this is for sorting ONLY!

int __init_environment( void *reserved )	
{
#pragma unused(reserved)
return 0;	
}
int __deinit_environment( void *reserved )	
{
#pragma unused(reserved)
	return 0;	
}

void MF_RegError(int errcode, const regex_t *preg)
{
	char szErrBuf[3500];
	if (!errcode)
		return;
		
	regerror(errcode, preg, szErrBuf, sizeof(szErrBuf));

	if (!NLM_noUserInterface)
	{
		char szTemp[4000];
		
		sprintf(szTemp,"Regular Expression Error:\n%s\n",szErrBuf);
		
		NWSDisplayInformation (
		
			NULL,
			1,
			0,
			0,
			ERROR_PALETTE,
			VREVERSE,
			(NUTCHAR)szTemp,
			NLM_nutHandle
			);
		
	} else {
		ConsolePrintf("MFCONFIG: RegExp Error: %s\n",szErrBuf);
	}
}

void MFConfig_ImportLicenseKey(void *)
{

	char szTemp[MAX_PATH];
	char szTemp2[MAX_PATH];
	char* cResult;
	int rc;
	FILE *fImp;
//	size_t dRead;
	int formatPosition = 0;
	
	szTemp[0] = 0;
	
	rc = NWSEditString ( 0,
					0,
					1,	/* height */
					50, /* width */
					EDIT_UTIL_IMPORTFILE_GETLICENSE, /* header */
					NO_MESSAGE, /* prompt */
					(NUTCHAR)szTemp, /* buffer */
					49, /* maxlen */
					EF_ANY, /* type */
					NLM_nutHandle,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((rc & E_SELECT) && (rc & E_CHANGE))
	{
		if (access(szTemp,F_OK) != 0)
		{
			NWSDisplayErrorText ( ERROR_FILE_NOT_FOUND , SEVERITY_FATAL , NLM_nutHandle , szTemp );
			
		} else {
		
/*
    -----------------------MAILFILTER  CLEARINGHOUSE-----------------------
    SERVER: IONUSMAIL - VSCAN: 1 - ALIAS: 1 - MCOPY: 1
    KEY: 3cTZ11RZtINjD[NjHJiZNJwZS0XVViVF0P[e32fmQ54oPv7C6Xfby98w0MLx00K2
    -----------------------ELECTRONIC  KEY  DELIVERY-----------------------
*/

			NWSStartWait(0,0,NLM_nutHandle);
		
			fImp = fopen(szTemp,"rt");
			if (!fImp)
			{
				NWSDisplayErrorText ( ERROR_FILE_NOT_FOUND , SEVERITY_FATAL , NLM_nutHandle , szTemp );
				return;
			}
			
			while (!feof(fImp))
			{
				szTemp[0]=0;
				fgets(szTemp, MAX_PATH, fImp);
				szTemp[MAX_PATH-2]=0;
				

				if (formatPosition == 2) {
					cResult = strstr(szTemp,"KEY: ");	
					if (cResult != NULL)
						{ 
						  strcpy(MFC_License_Key,cResult+5);
						  if (formatPosition==2) formatPosition = 3;
						} else formatPosition=0;		
				}

				if (formatPosition == 1) {
					sprintf(szTemp2,"SERVER: ",NLM_ServerName);
					cResult = strstr(szTemp,szTemp2);	
					if (cResult != NULL)
						formatPosition = 2;
						else formatPosition=0;		
				}

				if (formatPosition == 0) {
					cResult = strstr(szTemp,"-----------------------MAILFILTER  CLEARINGHOUSE-----------------------");
					if (cResult != NULL)
						formatPosition = 1;
				}
				
				if (formatPosition == 3)
					break;
			}
			
			fclose(fImp);
			
			NWSEndWait(NLM_nutHandle);
			
			NWSUngetKey(UGK_SPECIAL_KEY,UGK_REFRESH_KEY,NLM_nutHandle);

			if (formatPosition != 3)
				NWSDisplayErrorText ( ERROR_LICENSE_NOT_FOUND , SEVERITY_FATAL , NLM_nutHandle , NULL );

		}
		
	
	} else return;
				
}

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

int MF_ConfigReadInt(const char ConfigFile[MAX_PATH], int Entry)
{
	int retVal = 0;

	char readBuf[16];

	FILE* cfgFile;

	readBuf[0]='\0';

	cfgFile = fopen(ConfigFile,"rb");

	if (cfgFile == NULL)
		return -2;

	fseek(cfgFile,Entry,SEEK_SET);
	fread(readBuf,sizeof(char),15,cfgFile);
	fclose(cfgFile);

	readBuf[14]=0;

	retVal = atoi(readBuf);

	return retVal;
}

//
// Frees the FilterList-Cache
//
void MF_Filter_FreeLists()
{
	// ATTACHMENTS
//	if (	MFT_Filter_ListAttachments	!= NULL	)	{		free(MFT_Filter_ListAttachments);		MFT_Filter_ListAttachments = NULL;		}

	// SUBJECTS
//	if (	MFT_Filter_ListSubjects		!= NULL	)	{		free(MFT_Filter_ListSubjects);			MFT_Filter_ListSubjects = NULL;			}

	// SENDERS
//	if (	MFT_Filter_ListSenders		!= NULL )	{		free(MFT_Filter_ListSenders);			MFT_Filter_ListSenders = NULL;			}
}

void MF_Filter_Sort()
{
	int fltItem;
	int curId = 0;

	// pass 1 ...
	for (fltItem = 0; fltItem<MailFilter_MaxFilters; fltItem++)
	{
		if (MFC_Filters[fltItem].expression[0] == 0)
			break;
			
		if (MFC_Filters[fltItem].action == MAILFILTER_MATCHACTION_PASS)
		{
			id[curId] = MFC_Filters[fltItem];
			if (curId+1 > MailFilter_MaxFilters) {	ConsolePrintf("***ERROR*** %s > %s\n",curId,MailFilter_MaxFilters);	break; }
			curId++;	id[curId].expression[0] = 0;
		}
	}

	// pass 2 ...
	for (fltItem = 0; fltItem<MailFilter_MaxFilters; fltItem++)
	{
		if (MFC_Filters[fltItem].expression[0] == 0)
			break;
			
		if (MFC_Filters[fltItem].action != MAILFILTER_MATCHACTION_PASS)
		{
			id[curId] = MFC_Filters[fltItem];
			if (curId+1 > MailFilter_MaxFilters) {	ConsolePrintf("***ERROR*** %s > %s\n",curId,MailFilter_MaxFilters);	break; }
			curId++;	id[curId].expression[0] = 0;
		}
	}
	
	// and now put it back ...
	for (fltItem = 0; fltItem<MailFilter_MaxFilters; fltItem++)
	{
		if (id[fltItem].expression[0] == 0)
			break;

		MFC_Filters[fltItem] = id[fltItem];
	}
	
	return;
}

/*int MF_Filter_InitListsV6();
int MF_Filter_InitListsV7();
int MF_Filter_InitListsV8();
*/
//
// Init the Cache (Subject and Attachment Scan)
//
int MF_Filter_InitLists()
{
	if (MFC_ConfigBuild < 7)
		return MF_Filter_InitListsV6();
		else
	if (MFC_ConfigBuild < 8)
		return MF_Filter_InitListsV7();
		else
	if (MFC_ConfigBuild < 9)
		return MF_Filter_InitListsV8();
		else
	if (MFC_ConfigBuild < 10)
		return MF_Filter_InitListsV8();
		else
		return MF_Filter_InitListsV8();
}

// this one can read the V9 config file and put it in the current memory config
int MF_Filter_InitListsV9()
{
	FILE* cfgFile;
	int curItem;
	long rc1;
	long rc2;
	char szTemp[1002];

	// version 9

	cfgFile = fopen(MAILFILTER_CONFIGURATION_MAILFILTER,"rb");
	fseek(cfgFile,4000,SEEK_SET);

	for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
	{
		MFC_Filters[curItem].matchfield = (char)fgetc(cfgFile);
		MFC_Filters[curItem].notify = (char)fgetc(cfgFile);
		MFC_Filters[curItem].type = (char)fgetc(cfgFile);
		MFC_Filters[curItem].action = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabled = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledIncoming = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledOutgoing = (char)fgetc(cfgFile);
		MFC_Filters[curItem].expression[0] = 0;

		rc1 = (long)fread(szTemp,sizeof(char),501,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc1+1,SEEK_CUR);
		
		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].expression[0]=0;		

		rc2 = (long)fread(szTemp,sizeof(char),61,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc2+1,SEEK_CUR);

		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].name[0]=0;
		
		if (MFC_Filters[curItem].expression[0]==0)
			break;
	}

	fclose(cfgFile);
	return 0;
}

// this one can read the V8, V9 and V10 config file and put it in the current memory config
int MF_Filter_InitListsV8()
{
	FILE* cfgFile;
	int curItem;
	long rc1;
	long rc2;
	char szTemp[1002];

	// version 8, 9, 10

	cfgFile = fopen(MAILFILTER_CONFIGURATION_MAILFILTER,"rb");
	fseek(cfgFile,4000,SEEK_SET);

	for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
	{
		MFC_Filters[curItem].matchfield = (char)fgetc(cfgFile);
		MFC_Filters[curItem].notify = (char)fgetc(cfgFile);
		MFC_Filters[curItem].type = (char)fgetc(cfgFile);
		MFC_Filters[curItem].action = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabled = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledIncoming = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledOutgoing = (char)fgetc(cfgFile);
		MFC_Filters[curItem].expression[0] = 0;

		if (MFC_ConfigBuild < 9)
		{
			switch (MFC_Filters[curItem].matchfield)
			{
			case MAILFILTER_OLD_MATCHFIELD_SUBJECT:
				MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_SUBJECT;
				break;
			case MAILFILTER_OLD_MATCHFIELD_SIZE:
				MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_SIZE;
				break;
			case MAILFILTER_OLD_MATCHFIELD_EMAIL_FROM:
				MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_FROM;
				break;
			case MAILFILTER_OLD_MATCHFIELD_EMAIL_TO:
				MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_TO;
				break;
			}
		}
		if (MFC_ConfigBuild < 10)
		{
			switch (MFC_Filters[curItem].matchfield)
			{
			case MAILFILTER_MATCHFIELD_EMAIL_TO:
				MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_TOANDCC;
				break;
			case MAILFILTER_MATCHFIELD_EMAIL:
				MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC;
				break;
			}
		}
		
		rc1 = (long)fread(szTemp,sizeof(char),501,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc1+1,SEEK_CUR);
		
		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].expression[0]=0;		

		rc2 = (long)fread(szTemp,sizeof(char),61,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc2+1,SEEK_CUR);

		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].name[0]=0;
		
		if (MFC_Filters[curItem].expression[0]==0)
			break;
	}

	fclose(cfgFile);
	return 0;
}

// this one can read the V7 config file and put it in the current memory config
int MF_Filter_InitListsV7()
{
	FILE* cfgFile;
	int curItem;
	long rc1;
	long rc2;
	char szTemp[1002];

	// version 7

	cfgFile = fopen(MAILFILTER_CONFIGURATION_MAILFILTER,"rb");
	fseek(cfgFile,4000,SEEK_SET);

	for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
	{
		MFC_Filters[curItem].matchfield = (char)fgetc(cfgFile);

		switch (MFC_Filters[curItem].matchfield)
		{
		case MAILFILTER_OLD_MATCHFIELD_SUBJECT:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_SUBJECT;
			break;
		case MAILFILTER_OLD_MATCHFIELD_SIZE:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_SIZE;
			break;
		case MAILFILTER_OLD_MATCHFIELD_EMAIL_FROM:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_FROM;
			break;
		case MAILFILTER_OLD_MATCHFIELD_EMAIL_TO:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_TOANDCC;
			break;
		}
			
		MFC_Filters[curItem].notify = (char)fgetc(cfgFile);
		MFC_Filters[curItem].type = 0;
		MFC_Filters[curItem].action = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabled = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledIncoming = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledOutgoing = (char)fgetc(cfgFile);
		MFC_Filters[curItem].expression[0] = 0;
		
		rc1 = (long)fread(szTemp,sizeof(char),501,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc1+1,SEEK_CUR);
		
		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].expression[0]=0;		

		rc2 = (long)fread(szTemp,sizeof(char),61,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc2+1,SEEK_CUR);

		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].name[0]=0;
		
		if (MFC_Filters[curItem].expression[0]==0)
			break;
	}

	fclose(cfgFile);
	return 0;
}

// this one can read the V6 config file and put it in the current memory config
int MF_Filter_InitListsV6()
{
	FILE* filterList;
	int curItem=0;
	int curFilter=0;
	int curPos=-1;
	int curChr = 0;
	char migrateNotify;

	migrateNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
	if (MFC_Notification_EnableInternalRcpt)	migrateNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
	if (MFC_Notification_EnableInternalSndr)	migrateNotify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
	if (MFC_Notification_EnableExternalRcpt)	migrateNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;
	if (MFC_Notification_EnableExternalSndr)	migrateNotify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING;

	////////////////////
	//
	//  Begin with the ATTACHMENTS
	//

	filterList = fopen(MAILFILTER_CONFIGURATION_MAILFILTER,"rb");
	fseek(filterList,4000,SEEK_SET);

	if (filterList == NULL)
	{
		ConsolePrintf("MAILFILTER: Error while opening configuration file.\n");
		return FALSE;
	}

	while( !feof(filterList) )
	{
		if (curItem > MFT_Filter_ListAttachments_MAX-1) break;

		MFC_Filters[curFilter].expression[0]=0;
		fread(MFC_Filters[curFilter].expression,sizeof(char),50,filterList);
		MFC_Filters[curFilter].expression[50-1]=0;
		if (MFC_Filters[curFilter].expression[0]==0)
			break;

		MFC_Filters[curFilter].matchfield = MAILFILTER_MATCHFIELD_ATTACHMENT;
		MFC_Filters[curFilter].name[0] = 0;
		MFC_Filters[curFilter].enabled = true;
		MFC_Filters[curFilter].enabledIncoming = true;
		MFC_Filters[curFilter].enabledOutgoing = true;
		MFC_Filters[curFilter].enabled = (MFC_Filters[curItem].enabledIncoming == true) && (MFC_Filters[curFilter].enabledOutgoing == true);
		MFC_Filters[curFilter].notify = migrateNotify;
		MFC_Filters[curFilter].action = MAILFILTER_MATCHACTION_DROP_MAIL;
				
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curFilter++;
		curItem++;
	}

	//
	//
	////////////////////


	////////////////////
	//
	//  Next, the SUBJECT cache
	//

	fseek(filterList,11550,SEEK_SET);

	while( !feof(filterList) )
	{
		if (curItem > MFT_Filter_ListSubjects_MAX-1) break;

		MFC_Filters[curFilter].expression[0]=0;
		curPos=-1;
		curChr=0;
		fread(MFC_Filters[curFilter].expression,sizeof(char),75,filterList);
		MFC_Filters[curFilter].expression[75-1]=0;
		if (MFC_Filters[curFilter].expression[0]==0)
			break;

		MFC_Filters[curFilter].matchfield = MAILFILTER_MATCHFIELD_SUBJECT;
		MFC_Filters[curFilter].name[0] = 0;
		MFC_Filters[curFilter].enabled = true;
		MFC_Filters[curFilter].enabledIncoming = true;
		MFC_Filters[curFilter].enabledOutgoing = true;
		MFC_Filters[curFilter].enabled = (MFC_Filters[curFilter].enabledIncoming == true) && (MFC_Filters[curFilter].enabledOutgoing == true);
		MFC_Filters[curFilter].notify = migrateNotify;
		MFC_Filters[curFilter].action = MAILFILTER_MATCHACTION_DROP_MAIL;
				
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curFilter++;
	}
	//
	//
	////////////////////

	////////////////////
	//
	//  Next, the SENDER cache
	//
	fseek(filterList,87000,SEEK_SET);

	while( !feof(filterList) )
	{
		if (curItem > MFT_Filter_ListSenders_MAX-1) break;

		MFC_Filters[curFilter].expression[0]=0;

		curPos=-1;
		curChr=0;

		fread(MFC_Filters[curFilter].expression,sizeof(char),50,filterList);
		MFC_Filters[curFilter].expression[50-1]=0;

		if (MFC_Filters[curFilter].expression[0]==0)
			break;

		MFC_Filters[curFilter].matchfield = MAILFILTER_MATCHFIELD_EMAIL;
		MFC_Filters[curFilter].name[0] = 0;
		MFC_Filters[curFilter].enabledIncoming = true;
		MFC_Filters[curFilter].enabledOutgoing = true;
		MFC_Filters[curFilter].enabled = (MFC_Filters[curFilter].enabledIncoming == true) && (MFC_Filters[curFilter].enabledOutgoing == true);
		MFC_Filters[curFilter].notify = migrateNotify;
		MFC_Filters[curFilter].action = MAILFILTER_MATCHACTION_DROP_MAIL;
				
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curFilter++;
	}

	//
	//
	////////////////////

	// ** MODIFY LISTS **

	////////////////////
	//
	//  Next, the FROM cache
	//
	curItem = 0;
	
	fseek(filterList,95000,SEEK_SET);

/*	while( !feof(filterList) )
	{
		if (curItem > MFT_ModifyInt_ListFrom_MAX-1) break;

		curPos=-1;
		curChr=0;
*/
//		fread(MFT_ModifyInt_ListFrom[curItem],sizeof(char),50,filterList);
//		MFT_ModifyInt_ListFrom[curItem][50-1]=0;
		fread(MFC_Multi2One,sizeof(char),50,filterList);
		MFC_Multi2One[49]=0;
/*
		if (MFT_ModifyInt_ListFrom[curItem][0]==0)
			break;

#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curItem++;
	}
*/	//
	//
	////////////////////
	
	fclose(filterList);
	
	return TRUE;

}

void MF_ConfigFree(void)
{

	// Free Attachment, Subject, etc. Caches ...
	MF_Filter_FreeLists();

	if (MFC_License_Key != NULL)				{	free(MFC_License_Key);				MFC_License_Key				= NULL;	}

	if (MFC_GWIA_ROOT != NULL)					{	free(MFC_GWIA_ROOT);				MFC_GWIA_ROOT				= NULL;	}
	if (MFC_MFLT_ROOT != NULL)					{	free(MFC_MFLT_ROOT);				MFC_MFLT_ROOT				= NULL;	}

	if (MFC_DOMAIN_Name != NULL)				{	free(MFC_DOMAIN_Name);				MFC_DOMAIN_Name				= NULL;	}
	if (MFC_DOMAIN_Hostname != NULL)			{	free(MFC_DOMAIN_Hostname);			MFC_DOMAIN_Hostname			= NULL;	}	
	if (MFC_DOMAIN_EMail_PostMaster != NULL)	{	free(MFC_DOMAIN_EMail_PostMaster);	MFC_DOMAIN_EMail_PostMaster = NULL;	}
	if (MFC_DOMAIN_EMail_MailFilter != NULL)	{	free(MFC_DOMAIN_EMail_MailFilter);	MFC_DOMAIN_EMail_MailFilter = NULL;	}

	if (MFC_BWL_ScheduleTime != NULL)			{	free(MFC_BWL_ScheduleTime);			MFC_BWL_ScheduleTime		= NULL;	}
//	if (MFC_DNSBL_Zonename != NULL)				{	free(MFC_DNSBL_Zonename);			MFC_DNSBL_Zonename			= NULL;	}

	if (MFC_ControlPassword != NULL)			{	free(MFC_ControlPassword);			MFC_ControlPassword			= NULL;	}
	if (MFC_Multi2One != NULL)					{	free(MFC_Multi2One);				MFC_Multi2One				= NULL; }

}

void MFU_strxor(char* string, char xorVal)
{
	if (string == NULL) return;
	
	do {
		if (*string == 0) break;
		
		*string = (char)(*string ^ xorVal);
// had xor here !
	} while(*string++);
}

// Reads the configuration ...
int MF_ConfigRead()
{
	struct stat statInfo;
	int rc;
	char* MFC_ConfigBuildStr;
	char* MFC_ConfigVersion;
	int freshInstall;
	int lastRead = 0;
	freshInstall = FALSE;
	rc = 0;



	if (stat(MAILFILTER_CONFIGURATION_MAILFILTER,&statInfo))
		rc = 99;
	if (rc != 0)	goto MF_ConfigRead_ERR;

	if (statInfo.st_size < 3999)
		rc = 99;	
	if (rc != 0)	goto MF_ConfigRead_ERR;


	// Init Vars ...
	MFC_ConfigVersion = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_License_Key = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_GWIA_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_MFLT_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_DOMAIN_Name = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_DOMAIN_Hostname = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_DOMAIN_EMail_MailFilter = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_DOMAIN_EMail_PostMaster = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_BWL_ScheduleTime = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
//	MFC_DNSBL_Zonename = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_ControlPassword = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_Multi2One = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	
	MFC_License_Key							[0] = 0;
	MFC_GWIA_ROOT							[0] = 0;
	MFC_MFLT_ROOT							[0] = 0;
	MFC_DOMAIN_Name							[0] = 0;
	MFC_DOMAIN_Hostname						[0] = 0;
	MFC_DOMAIN_EMail_MailFilter				[0] = 0;
	MFC_DOMAIN_EMail_PostMaster				[0] = 0;
	MFC_BWL_ScheduleTime					[0] = 0;
//	MFC_DNSBL_Zonename						[0] = 0;
	MFC_ControlPassword						[0]	= 0;
	MFC_Multi2One							[0] = 0;

/*** WARNING: If you have Dynamic Allocated Lists: This *WILL*  CRASH ****/

/*	MFT_Filter_ListAttachments				[0][0]=0;
	MFT_Filter_ListSubjects					[0][0]=0;
	MFT_Filter_ListSenders					[0][0]=0;
	MFT_ModifyInt_ListFrom					[0][0]=0;*/

	//
	// Read In Configuration Version/Build

	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 0, MFC_ConfigVersion) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}
	if (memicmp(MFC_ConfigVersion,"MAILFILTER_R001",15) != 0)
		rc = 99;
	MFC_ConfigBuildStr = (MFC_ConfigVersion + 16);
	MFC_ConfigBuild = atoi(MFC_ConfigBuildStr);

	if (memicmp(MFC_ConfigVersion+22,"FRESH",5) == 0)
		freshInstall = TRUE;

	free(MFC_ConfigVersion);

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 60, MFC_ControlPassword);

	//
	// Read In Configuration Values
//7*MAILFILTER_CONFIGURATIONFILE_STRING
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 240, MFC_License_Key);

	lastRead = MAILFILTER_CONFIGURATIONFILE_STRING; //1*MAILFILTER_CONFIGURATIONFILE_STRING;
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_GWIA_ROOT);
	lastRead = lastRead + MAILFILTER_CONFIGURATIONFILE_STRING;

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_MFLT_ROOT);
	lastRead = lastRead + MAILFILTER_CONFIGURATIONFILE_STRING;

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_DOMAIN_Name);
	lastRead = lastRead + MAILFILTER_CONFIGURATIONFILE_STRING;

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_DOMAIN_EMail_MailFilter);
	lastRead = lastRead + MAILFILTER_CONFIGURATIONFILE_STRING;

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_DOMAIN_EMail_PostMaster);
	lastRead = lastRead + MAILFILTER_CONFIGURATIONFILE_STRING;
	
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_DOMAIN_Hostname);
	lastRead = lastRead + MAILFILTER_CONFIGURATIONFILE_STRING;

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_Multi2One);
	lastRead = lastRead + (MAILFILTER_CONFIGURATIONFILE_STRING/2);

	// new in v8
//  removed in a later v8
//	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_DNSBL_Zonename);
	lastRead = lastRead + (MAILFILTER_CONFIGURATIONFILE_STRING/2);

	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, lastRead, MFC_BWL_ScheduleTime);
	lastRead = lastRead + MAILFILTER_CONFIGURATIONFILE_STRING;
	
	// **
	
	MFC_Notification_EnableInternalSndr = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3500);

	MFC_MAILSCAN_Scan_DirNum = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3502);

	MFC_MAILSCAN_Scan_DirWait = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3506);

	MFC_Notification_EnableInternalRcpt = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3510);

	MFC_Notification_AdminLogs = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3512);

	MFC_Notification_AdminMailsKilled = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3514);

	MFC_ProblemDir_MaxAge = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3516);

	MFC_ProblemDir_MaxSize = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3521);

	MFC_Notification_AdminDailyReport = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3529);

	MFC_Notification_EnableExternalSndr = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3531);

	MFC_Notification_EnableExternalRcpt = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3533);

	MFC_GWIA_Version =
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3535);

	if (MFC_GWIA_Version == 0)
		MFC_GWIA_Version = 600;	// upgrade: set default to GWIA V6

	MFC_EnableIncomingRcptCheck =
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3539);
	
	MFC_EnableAttachmentDecoder =
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3541);
		
	// new in v8+
	
	MFC_EnablePFAFunctionality =
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3543);

//	removed with v8/1118	
//	MFC_DropUnresolvableRelayHosts = 
//		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3545);

	// Next start at 3547

	MF_Filter_InitLists();

	if (freshInstall)
	{
		int myItems = 0;
		unsigned int myMax = 0;
		FILE* fImp;
		char szTemp[MAX_BUF+1];
		int curItem = -1;

		MFC_Notification_EnableInternalRcpt = 0;
		MFC_Notification_EnableInternalSndr = 1;
		MFC_Notification_EnableExternalRcpt = 0;
		MFC_Notification_EnableExternalSndr = 1;
		MFC_MAILSCAN_Scan_DirNum  			= 10;
		MFC_MAILSCAN_Scan_DirWait 			= 180;
		MFC_Notification_AdminLogs			= 1;
		MFC_ProblemDir_MaxSize				= 0;
		MFC_ProblemDir_MaxAge				= 180;
		MFC_Notification_AdminMailsKilled	= 1;
		MFC_Notification_AdminDailyReport	= 0;
		MFC_EnableIncomingRcptCheck			= 0;
		MFC_EnableAttachmentDecoder 		= 1;
		MFC_EnablePFAFunctionality			= 0;
		// GWIA Version is filled in by Install.W32 !!!
//		MFC_GWIA_Version					= 600;
		
		MFC_ControlPassword[0]				= 0;
		
		sprintf(szTemp,"%s\\DEFAULTS\\FILTERS.BIN",MFC_ConfigDirectory);
		if (( fImp = fopen(szTemp,"rb")) != NULL ) {
		fseek(fImp, 54, SEEK_SET);

		while (!feof(fImp))
		{
			if (curItem > MailFilter_MaxFilters)
				break;

			MFC_Filters[curItem].matchfield = (char)fgetc(fImp);
			MFC_Filters[curItem].notify = (char)fgetc(fImp);
			MFC_Filters[curItem].type = (char)fgetc(fImp);
			MFC_Filters[curItem].action = (char)fgetc(fImp);
			MFC_Filters[curItem].enabled = (char)fgetc(fImp);
			MFC_Filters[curItem].enabledIncoming = (char)fgetc(fImp);
			MFC_Filters[curItem].enabledOutgoing = (char)fgetc(fImp);
			
			rc = (long)fread(szTemp,sizeof(char),501,fImp);
			fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
			
			if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
				else MFC_Filters[curItem].expression[0]=0;		

			rc = (long)fread(szTemp,sizeof(char),61,fImp);
			fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

			if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
				else MFC_Filters[curItem].name[0]=0;
			
			if (MFC_Filters[curItem].expression[0]==0)
				break;

			curItem++;
		}
		
		fclose(fImp);
		
		} else {
		
			ConsolePrintf("*** ERROR IMPORTING DEFAULT FILTER LIST ***\n");
		}

		rc = 0;
	}

	if (!NLM_noUserInterface)
	{

	if ((!rc) && (MFC_ControlPassword[0] != 0))
	{
		NUTCHAR readPass = (NUTCHAR)malloc(MAILFILTER_CONFIGURATION_LENGTH);
		readPass[0] = 0;
		NWSEditString( 0, 0, 1, 50, 0 /* header */, EDIT_UTIL_GETCONTROLPASSWORD, readPass, 50, EF_MASK|EF_NOSPACES|EF_SET|EF_NOCONFIRM_EXIT, NLM_nutHandle, NULL, NULL, (NUTCHAR)"a..z0..9A..Z" );
		
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
				(NUTCHAR)"Incorrect Password Specified!",
				NLM_nutHandle
				);

			goto MF_ConfigRead_ERR;
		}
	}

	}
	
// Error Handling goes here ...
MF_ConfigRead_ERR:
	{
		if (rc)
			MF_ConfigFree();

		if (rc == 99)
		{	
			if (NLM_noUserInterface)
			{
				ConsolePrintf("MFCONFIG: Either the Configuration File is not existant, corrupt or too old.\nMFCONFIG: Aborting Upgrade.\n");
			} else {
			/** CONFIG FILE RECOVERY **/
			NWSDisplayErrorText ( ERROR_CONFIGURATION_INVALID , SEVERITY_FATAL , NLM_nutHandle , "" );
		
			if (!stat(MAILFILTER_CONFIGBACKUP__MAILFILTER,&statInfo))
			{
				if (statInfo.st_size > 40000)
				{

						rc = (int)NWSDisplayInformation (
							NULL,
							2,
							0,
							0,
							HELP_PALETTE,
							VREVERSE,
							(NUTCHAR)"MFConfig found a Configuration File Backup that could be valid.\n\nDo you want to try the Backup?\n",
							NLM_nutHandle
							);
							
						switch (rc)
						{
						
						case (0xFFFFFFFF):
							NWSTrace(NLM_nutHandle,(NUTCHAR)"Error occoured.");
							break;
							
						case (0xFE):
							return FALSE;
							break;
							
						case 0:
						
							rename(MAILFILTER_CONFIGURATION_MAILFILTER,MAILFILTER_CONFIGERROR___MAILFILTER);
							rename(MAILFILTER_CONFIGBACKUP__MAILFILTER,MAILFILTER_CONFIGURATION_MAILFILTER);
							return MF_ConfigRead();
						
							break;
						default:
							NWSTrace(NLM_nutHandle,(NUTCHAR)"*#* CRASH *#* -- Memory Corruption?");
							break;
							
						}
					
				}
			}
			}
		}

		if (rc)
		{
			ConsolePrintf("MFCONFIG: Error Code %d\n",rc);
			return FALSE;
		}
	}

	// No Errors! Register Cleanup ...

	atexit(MF_ConfigFree);

	return TRUE;
	
}

int MF_ConfigWrite()
{
	int rc =0;
	int curItem=0;
	long iCnt;		// ** DO NOT USE **
	long iMax = 0;	// ** DO NOT USE **
	
	FILE* cfgFile;
//	size_t dRead;

	if(!NLM_noUserInterface)
		NWSStartWait(0,0,NLM_nutHandle);
		
	ThreadSwitch();

#define doNull(howMany)	{ ThreadSwitch(); iMax = ftell(cfgFile); for (iCnt=0;iCnt<(howMany-iMax);iCnt++) fputc(0,cfgFile); }

	rename(MFC_ConfigFile,MFC_ConfigFileBackup);

	cfgFile = fopen(MFC_ConfigFile,"wb");

	if (cfgFile == NULL)
		return -2;

	// Header ...
	fprintf(cfgFile,"%s",MAILFILTER_CONFIGURATION_SIGNATURE);

	doNull(60);
	fprintf(cfgFile,"%s",MFC_ControlPassword);

	// License Key
	doNull(240);
	fprintf(cfgFile,"%s",MFC_License_Key);

	// Values
	doNull(330);
	fprintf(cfgFile,"%s",MFC_GWIA_ROOT);

	doNull(660);
	fprintf(cfgFile,"%s",MFC_MFLT_ROOT);

	doNull(990);
	fprintf(cfgFile,"%s",MFC_DOMAIN_Name);

	doNull(1320);
	fprintf(cfgFile,"%s",MFC_DOMAIN_EMail_MailFilter);

	doNull(1650);
	fprintf(cfgFile,"%s",MFC_DOMAIN_EMail_PostMaster);
	
	doNull(1980);
	fprintf(cfgFile,"%s",MFC_DOMAIN_Hostname);
	
	// version 7	
	doNull(2310);
	fprintf(cfgFile,"%s",MFC_Multi2One);
	
	doNull(2475);
	//removed in a later v8
	//fprintf(cfgFile,"%s",MFC_DNSBL_Zonename);

	// version 8
	doNull(2640);
	fprintf(cfgFile,"%s",MFC_BWL_ScheduleTime);
	
	doNull(2970);
	// next one goes here...
	
	doNull(3500);
	fprintf(cfgFile, MFC_Notification_EnableInternalSndr == 0 ? "0" : "1");
	
	doNull(3502);
	fprintf(cfgFile, "%d", MFC_MAILSCAN_Scan_DirNum);
	
	doNull(3506);
	fprintf(cfgFile, "%d", MFC_MAILSCAN_Scan_DirWait);
	
	doNull(3510);
	fprintf(cfgFile, MFC_Notification_EnableInternalRcpt == 0 ? "0" : "1");
	
	doNull(3512);
	fprintf(cfgFile, MFC_Notification_AdminLogs == 0 ? "0" : "1");

	doNull(3514);
	fprintf(cfgFile, MFC_Notification_AdminMailsKilled == 0 ? "0" : "1");

	doNull(3516);
	fprintf(cfgFile, "%d", MFC_ProblemDir_MaxAge);

	doNull(3521);
	fprintf(cfgFile, "%d", MFC_ProblemDir_MaxSize);
	
	doNull(3529);
	fprintf(cfgFile, MFC_Notification_AdminDailyReport == 0 ? "0" : "1");
	
	doNull(3531);
	fprintf(cfgFile, MFC_Notification_EnableExternalSndr == 0 ? "0" : "1");
	
	doNull(3533);
	fprintf(cfgFile, MFC_Notification_EnableExternalRcpt == 0 ? "0" : "1");
	
	doNull(3535);
	fprintf(cfgFile, "%d", MFC_GWIA_Version);

	doNull(3539);
	fprintf(cfgFile, MFC_EnableIncomingRcptCheck == 0 ? "0" : "1");

	doNull(3541);
	fprintf(cfgFile, MFC_EnableAttachmentDecoder == 0 ? "0" : "1");

	doNull(3543);
	fprintf(cfgFile, MFC_EnablePFAFunctionality == 0 ? "0" : "1");

	// version 8
	doNull(3545);
	//removed in a later v8
	//fprintf(cfgFile, MFC_DropUnresolvableRelayHosts == 0 ? "0" : "1");

	// version 9
	doNull(4000);
	
	for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
	{
		fputc(MFC_Filters[curItem].matchfield,cfgFile);
		fputc(MFC_Filters[curItem].notify,cfgFile);
		fputc(MFC_Filters[curItem].type,cfgFile);		// new in v8
		fputc(MFC_Filters[curItem].action,cfgFile);
		fputc(MFC_Filters[curItem].enabled,cfgFile);
		fputc(MFC_Filters[curItem].enabledIncoming,cfgFile);
		fputc(MFC_Filters[curItem].enabledOutgoing,cfgFile);
	
		fprintf(cfgFile,MFC_Filters[curItem].expression);
		fputc(0,cfgFile);
		fprintf(cfgFile,MFC_Filters[curItem].name);
		fputc(0,cfgFile);
		
		if (
			(MFC_Filters[curItem].expression[0]==0)
			&&
			(MFC_Filters[curItem].name[0]==0)
		   )
		  	break;
	}

	////////////////////

	fclose(cfgFile);

	if(!NLM_noUserInterface)
		NWSEndWait(NLM_nutHandle);

	return TRUE;	
}

/****************************************************************************
** Main menu action procedure.
*/
int NLM_VerifyProgramExit(void)
	{
	int cCode;
	
	if (NLM_exiting == TRUE)
		return -1;
	
	cCode=NWSConfirm(
		/*	I-	header				*/	PROGRAM_EXIT,
		/*	I-	centerLine			*/	0,
		/*	I-	centerColumn		*/	0,
		/*	I-	defaultChoice		*/	0,
		/*	I-	actionProcedure	*/	NULL,
		/*	I-	handle				*/	NLM_nutHandle,
		/*	I-	actionParameter	*/	NULL
		);
	
	/*------------------------------------------------------------------------
	**	Escape(-1) means No(0).
	*/
	if(cCode == (-1))
		cCode = 0;

	return(cCode);
	}

/****************************************************************************
** Main menu action procedure.
*/
int NLM_VerifySaveExit(void)
	{
	int cCode;
	
	cCode=NWSConfirm(
		/*	I-	header				*/	MSG_SAVE_CHANGES,
		/*	I-	centerLine			*/	0,
		/*	I-	centerColumn		*/	0,
		/*	I-	defaultChoice		*/	1,
		/*	I-	actionProcedure	*/	NULL,
		/*	I-	handle				*/	NLM_nutHandle,
		/*	I-	actionParameter	*/	NULL
		);
	
	/*------------------------------------------------------------------------
	**	Escape(-1) means No(0).
	*/
	if(cCode == (-1))
		cCode = 0;

	return cCode;
	}


void MFConfig_Util_ImportListFromFile(void* nutHandle)
{
	FILE* fImp;
	char szTemp[MAX_BUF+1];
	int rc;
	int curItem = -1;
	int* cI;
	char defaultNotify;
	int overrideNotify = false;
	
	nutHandle = nutHandle;	/* rid compiler warning */
	
	defaultNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
	if (MFC_Notification_EnableInternalRcpt)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
	if (MFC_Notification_EnableInternalSndr)	defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
	if (MFC_Notification_EnableExternalRcpt)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;
	if (MFC_Notification_EnableExternalSndr)	defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING;
	
	szTemp[0] = 0;
	
	rc = NWSEditString ( 0,
					0,
					1,	/* height */
					50, /* width */
					EDIT_UTIL_IMPORTFILE_GET, /* header */
					NO_MESSAGE, /* prompt */
					(NUTCHAR)szTemp, /* buffer */
					49, /* maxlen */
					EF_ANY, /* type */
					NLM_nutHandle,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((rc & E_SELECT) && (rc & E_CHANGE))
	{
		if (access(szTemp,F_OK) != 0)
		{
			NWSDisplayErrorText ( ERROR_FILE_NOT_FOUND , SEVERITY_FATAL , NLM_nutHandle , szTemp );
			
		} else {
		
			rc = NWSConfirm (
				      EDIT_UTIL_IMPORTFILE_ASKNOTIFICATION,
				      10,
				      12,
				      1,	// default = yes
				      NULL,	//int       (*action) (         int    option,          void  *parameter),
				      NLM_nutHandle,
				      NULL	//       void     * actionParameter
				      );
			if (rc == -1)
				return;
			if (rc == 0)
				overrideNotify = true;
			if (rc == 1)
				overrideNotify = false;
			
			for (curItem = 0; curItem<(MailFilter_MaxFilters+1); curItem++)
			{
				if (MFC_Filters[curItem].expression[0] == 0)		break;
			}
				
			NWSStartWait(0,0,NLM_nutHandle);
		
			fImp = fopen(szTemp,"rb");
			fseek(fImp, 54, SEEK_SET);

			while (!feof(fImp))
			{
				if (curItem > MailFilter_MaxFilters)
					break;

				MFC_Filters[curItem].matchfield = (char)fgetc(fImp);
				MFC_Filters[curItem].notify = (char)fgetc(fImp);
				MFC_Filters[curItem].type = (char)fgetc(fImp);
				MFC_Filters[curItem].action = (char)fgetc(fImp);
				MFC_Filters[curItem].enabled = (char)fgetc(fImp);
				MFC_Filters[curItem].enabledIncoming = (char)fgetc(fImp);
				MFC_Filters[curItem].enabledOutgoing = (char)fgetc(fImp);
				
				if (overrideNotify == true)
					MFC_Filters[curItem].notify = defaultNotify;
				
				rc = (long)fread(szTemp,sizeof(char),500,fImp);
				fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
				
				if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
					else MFC_Filters[curItem].expression[0]=0;		

				rc = (long)fread(szTemp,sizeof(char),60,fImp);
				fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

				if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
					else MFC_Filters[curItem].name[0]=0;
				
				if (MFC_Filters[curItem].expression[0]==0)
					break;

				cI = (int*)malloc(sizeof(int));
				*cI = curItem;
//				NWSAppendToList((NUTCHAR)MFC_Filters[curItem].expression, cI, NLM_nutHandle);
//
//				curItem++;


				szTemp[0] = ' '; szTemp[1] = ' ';
				switch (MFC_Filters[curItem].matchfield)
				{
				case MAILFILTER_MATCHFIELD_ALWAYS:
					szTemp[0] = '*';	break;
				case MAILFILTER_MATCHFIELD_ATTACHMENT:
					szTemp[0] = 'A';	break;
				case MAILFILTER_MATCHFIELD_EMAIL:
				case MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC:
					szTemp[0] = 'E';	break;
				case MAILFILTER_MATCHFIELD_SUBJECT:
					szTemp[0] = 'S';	break;
				case MAILFILTER_MATCHFIELD_SIZE:
					szTemp[0] = 's';	break;
				case MAILFILTER_MATCHFIELD_EMAIL_FROM:
					szTemp[0] = 'F';	break;
				case MAILFILTER_MATCHFIELD_EMAIL_TO:
				case MAILFILTER_MATCHFIELD_EMAIL_TOANDCC:
					szTemp[0] = 'T';	break;
				case MAILFILTER_MATCHFIELD_BLACKLIST:
					szTemp[0] = 'B';	break;
				case MAILFILTER_MATCHFIELD_IPUNRESOLVABLE:
					szTemp[0] = 'U';	break;
				default:
					szTemp[0] = '?';	break;
				}
				
				if (MFC_Filters[curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
				{
					szTemp[1] = '!';
				}
				
				strncpy(szTemp+2,MFC_Filters[curItem].expression,70);
				NWSAppendToList((NUTCHAR)szTemp, cI, NLM_nutHandle);

				curItem++;
ConsolePrintf("Imported %d: %s\n",curItem,szTemp);
			}
			
			fclose(fImp);
			
			NWSEndWait(NLM_nutHandle);
			
			NWSUngetKey(UGK_SPECIAL_KEY,UGK_REFRESH_KEY,NLM_nutHandle);
/*			NWSUngetKey(UGK_NORMAL_KEY,UGK_ESCAPE_KEY,nutHandle);
			NWSUngetKey(UGK_NORMAL_KEY,UGK_ENTER_KEY,nutHandle);
*/		}
		
	
	} else return;
				
}


void MFConfig_Util_ExportListToFile(void* nutHandle)
{
	FILE* fLst;
	char szTemp[MAX_BUF+1];
	int rc;
	int curItem = -1;
	
	nutHandle = nutHandle;	/* rid compiler warning */
	
	szTemp[0] = 0;
	
	rc = NWSEditString ( 0,
					0,
					1,	/* height */
					50, /* width */
					EDIT_UTIL_EXPORTFILE_GET, /* header */
					NO_MESSAGE, /* prompt */
					(NUTCHAR)szTemp, /* buffer */
					49, /* maxlen */
					EF_ANY, /* type */
					NLM_nutHandle,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((rc & E_SELECT) && (rc & E_CHANGE))
	{
		if (access(szTemp,F_OK) == 0)
		{
			NWSDisplayErrorText ( ERROR_FILE_EXISTS , SEVERITY_FATAL , NLM_nutHandle , szTemp );
			
		} else {
		
			NWSStartWait(0,0,NLM_nutHandle);

			fLst = fopen(szTemp,"wb");

/*			for (curItem = 0; curItem<(MailFilter_MaxFilters+1); curItem++)
			{
				if (0==MFC_Filters[curItem].expression[0])	break;
				fprintf(fLst,"%s\n",MFC_Filters[curItem].expression);
			}
*/
			fprintf(fLst,"MAILFILTER_FILTER_EXCHANGEFILE");
			fputc(0,fLst);
			fprintf(fLst,MAILFILTER_CONFIGURATION_SIGNATURE);
			fputc(0,fLst);
			fputc(0,fLst);
			fputc(0,fLst);
			fputc(0,fLst);
			

			for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
			{
				fputc(MFC_Filters[curItem].matchfield,fLst);
				fputc(MFC_Filters[curItem].notify,fLst);
				fputc(MFC_Filters[curItem].type,fLst);
				fputc(MFC_Filters[curItem].action,fLst);
				fputc(MFC_Filters[curItem].enabled,fLst);
				fputc(MFC_Filters[curItem].enabledIncoming,fLst);
				fputc(MFC_Filters[curItem].enabledOutgoing,fLst);
			
				fprintf(fLst,MFC_Filters[curItem].expression);
				fputc(0,fLst);
				fprintf(fLst,MFC_Filters[curItem].name);
				fputc(0,fLst);
				
				if (
					(MFC_Filters[curItem].expression[0]==0)
					&&
					(MFC_Filters[curItem].name[0]==0)
				   )
				  	break;
			}
				
		
			fclose(fLst);
			
			NWSEndWait(NLM_nutHandle);
			
		}
		
	
	} else return;
				
}

/****************************************************************************
** Edit filter detail dialog
*/
static	FIELD		*MFConfig_EditFilterDialog_fieldDescription;

int MFConfig_EditFilterDialog_MenuAction(int option, void *parameter)   {      parameter = parameter;	return option;   }
int MFConfig_EditFilterDialog_MenuActionAction(int option, void *parameter)   
{
#pragma unused(parameter)

	if(option==MAILFILTER_MATCHACTION_COPY)
		MFConfig_EditFilterDialog_fieldDescription->fieldData = (unsigned char*)MF_NMsg(EDIT_FILTERS_DESTINATION);
		else
		MFConfig_EditFilterDialog_fieldDescription->fieldData = (unsigned char*)MF_NMsg(EDIT_FILTERS_DESCRIPTION); 
		
	NWSSetFormRepaintFlag(TRUE,NLM_nutHandle); return option; 
}
int MFConfig_EditFilterDialog(unsigned int filterHandle)
{

	regex_t re;
	MFCONTROL	*ctlMatchfield;
	MFCONTROL	*ctlMatchaction;
	MFCONTROL	*ctlMatchtype;
   	unsigned long	line;
	int	formSaved;
	BOOL	newEnabledIncoming = (BOOL)MFC_Filters[filterHandle].enabledIncoming;
	BOOL	newEnabledOutgoing = (BOOL)MFC_Filters[filterHandle].enabledOutgoing;
	char	newDescription[60];
	char	newExpression[500];
	int		newMatchtype = MFC_Filters[filterHandle].type;
	int		newMatchfield = MFC_Filters[filterHandle].matchfield;
	int		newMatchaction = MFC_Filters[filterHandle].action;
	BOOL	newNotifyAdminIncoming = (BOOL)chkFlag(MFC_Filters[filterHandle].notify,MAILFILTER_NOTIFICATION_ADMIN_INCOMING);
	BOOL	newNotifyAdminOutgoing = (BOOL)chkFlag(MFC_Filters[filterHandle].notify,MAILFILTER_NOTIFICATION_ADMIN_OUTGOING);
	BOOL	newNotifySenderIncoming = (BOOL)chkFlag(MFC_Filters[filterHandle].notify,MAILFILTER_NOTIFICATION_SENDER_INCOMING);
	BOOL	newNotifySenderOutgoing = (BOOL)chkFlag(MFC_Filters[filterHandle].notify,MAILFILTER_NOTIFICATION_SENDER_OUTGOING);
	BOOL	newNotifyRecipientIncoming = (BOOL)chkFlag(MFC_Filters[filterHandle].notify,MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING);
	BOOL	newNotifyRecipientOutgoing = (BOOL)chkFlag(MFC_Filters[filterHandle].notify,MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING);

	if(NLM_exiting)
		return 0;

	NWSPushList(NLM_nutHandle);
	NWSInitForm(NLM_nutHandle);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 0;
	
//	NWSAppendStringField (line, 20, 60, NORMAL_FIELD, (NUTCHAR)newDescription, (NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", NULL, NLM_nutHandle);
//	line++;
	
//	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHFIELD), NLM_nutHandle);
	ctlMatchfield = NWSInitMenuField (MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuAction, 		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ALWAYS,		MAILFILTER_MATCHFIELD_ALWAYS, 			NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ATTACHMENT,	MAILFILTER_MATCHFIELD_ATTACHMENT, 		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL,			MAILFILTER_MATCHFIELD_EMAIL, 			NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_BOTHANDCC,MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC,	NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_FROM,	MAILFILTER_MATCHFIELD_EMAIL_FROM,		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_TO,		MAILFILTER_MATCHFIELD_EMAIL_TO, 		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_TOANDCC,	MAILFILTER_MATCHFIELD_EMAIL_TOANDCC,	NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_SUBJECT,		MAILFILTER_MATCHFIELD_SUBJECT, 			NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_SIZE,			MAILFILTER_MATCHFIELD_SIZE, 			NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_BLACKLIST,		MAILFILTER_MATCHFIELD_BLACKLIST, 		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_IPUNRESOLVABLE,MAILFILTER_MATCHFIELD_IPUNRESOLVABLE, 	NLM_nutHandle);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchfield, ctlMatchfield, NULL, NLM_nutHandle);   
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHACTION), NLM_nutHandle);
	ctlMatchaction = NWSInitMenuField (MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuActionAction, 			NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_DROP_MAIL,	MAILFILTER_MATCHACTION_DROP_MAIL, 		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_PASS,		MAILFILTER_MATCHACTION_PASS,		 	NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_SCHEDULE,	MAILFILTER_MATCHACTION_SCHEDULE,		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_NOSCHEDULE,	MAILFILTER_MATCHACTION_NOSCHEDULE,		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_COPY,		MAILFILTER_MATCHACTION_COPY,			NLM_nutHandle);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchaction, ctlMatchaction, NULL, NLM_nutHandle);   
	line++;


	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHTYPE), NLM_nutHandle);
	ctlMatchtype = NWSInitMenuField (MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuAction, 			NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchtype, EDIT_FILTERS_MATCHTYPE_MATCH,			MAILFILTER_MATCHTYPE_MATCH, 		NLM_nutHandle);
	NWSAppendToMenuField (ctlMatchtype, EDIT_FILTERS_MATCHTYPE_NOMATCH,			MAILFILTER_MATCHTYPE_NOMATCH,		NLM_nutHandle);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchtype, ctlMatchtype, NULL, NLM_nutHandle);   
	line++;
	
	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_EXPRESSION), NLM_nutHandle);
	strcpy (newExpression, MFC_Filters[filterHandle].expression);
//	NWSAppendStringField (line, 20, 500, NORMAL_FIELD, (NUTCHAR)newExpression, (NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", NULL, NLM_nutHandle);
	NWSAppendScrollableStringField  (line, 20, 55, REQUIRED_FIELD, (NUTCHAR)newExpression, 500, (NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", EF_ANY, NULL, NLM_nutHandle); 
	line++;

	MFConfig_EditFilterDialog_fieldDescription = NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_DESCRIPTION), NLM_nutHandle);
	strcpy (newDescription, MFC_Filters[filterHandle].name);
//	MFConfig_EditFilterDialog_fieldDescription->fieldLine = line;
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newDescription, 60, (NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", EF_ANY, NULL, NLM_nutHandle); 
	
	line++;
	
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_ENABLEDINCOMING), NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newEnabledIncoming, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_ENABLEDOUTGOING), NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newEnabledOutgoing, NULL, NLM_nutHandle);
	line++;

	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_NOTIFICATION_IN), NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)">> Postmaster:", NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyAdminIncoming, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)">> Sender:", NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifySenderIncoming, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)">> Recipient:", NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyRecipientIncoming, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_NOTIFICATION_OUT), NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)">> Postmaster:", NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyAdminOutgoing, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)">> Sender:", NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifySenderOutgoing, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)">> Recipient:", NLM_nutHandle);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyRecipientOutgoing, NULL, NLM_nutHandle);
	line++;

	// fix some fields
	if (newMatchaction==MAILFILTER_MATCHACTION_COPY) 
		MFConfig_EditFilterDialog_fieldDescription->fieldData = (unsigned char*)"Destination";
	
	
	formSaved = NWSEditForm (
		EDIT_FILTERS_EDITRULE,		//headernum	//0
		3,							//line
		0,							//col
		line+4,						//portalHeight
		80,							//portalWidth
		line+1,						//virtualHeight,
		78,							//virtualWidth,
		TRUE,						//ESCverify,
		TRUE,						//forceverify,
		MSG_SAVE_CHANGES,			//confirmMessage,
		NLM_nutHandle
	);

	/*------------------------------------------------------------------------
	** This function returns TRUE if the form was saved, FALSE if not.
	** If the form was not saved you must restore all variables to their
	** original values manually
	*/

	if (formSaved)
	{
		MFC_Filters[filterHandle].enabled = 1;
		MFC_Filters[filterHandle].enabledIncoming = (char)newEnabledIncoming;
		MFC_Filters[filterHandle].enabledOutgoing = (char)newEnabledOutgoing;

		MFC_Filters[filterHandle].matchfield = (char)newMatchfield;
		MFC_Filters[filterHandle].type = (char)newMatchtype;
		MFC_Filters[filterHandle].action = (char)newMatchaction;

		strncpy(MFC_Filters[filterHandle].expression,newExpression		,500);
		strncpy(MFC_Filters[filterHandle].name,newDescription			,60);
		

		MFC_Filters[filterHandle].notify = MAILFILTER_NOTIFICATION_NONE;
		if (newNotifyAdminIncoming)			MFC_Filters[filterHandle].notify |= MAILFILTER_NOTIFICATION_ADMIN_INCOMING;
		if (newNotifyAdminOutgoing)			MFC_Filters[filterHandle].notify |= MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
		if (newNotifySenderIncoming)		MFC_Filters[filterHandle].notify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING;
		if (newNotifySenderOutgoing)		MFC_Filters[filterHandle].notify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
		if (newNotifyRecipientIncoming)		MFC_Filters[filterHandle].notify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
		if (newNotifyRecipientOutgoing)		MFC_Filters[filterHandle].notify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;
		

		if (
			(MFC_Filters[filterHandle].action == MAILFILTER_MATCHACTION_SCHEDULE)
			||
			(MFC_Filters[filterHandle].action == MAILFILTER_MATCHACTION_NOSCHEDULE)
		)
		{
			MFC_Filters[filterHandle].notify = MAILFILTER_NOTIFICATION_NONE;
			MFC_Filters[filterHandle].enabledIncoming = 0;
		}
	}

	if ( (newMatchfield != MAILFILTER_MATCHFIELD_SIZE) || (newMatchfield != MAILFILTER_MATCHFIELD_BLACKLIST) || (newMatchfield != MAILFILTER_MATCHFIELD_IPUNRESOLVABLE) )
	{
		MF_RegError(regcomp(&re,MFC_Filters[filterHandle].expression),&re);
		regfree(&re);
	}

	NWSDestroyForm (NLM_nutHandle);
	NWSPopList(NLM_nutHandle);

	if (formSaved)
		return -2;

	return -1;

}

/****************************************************************************
** Edit filter list action
*/

int MFConfig_EditFilters_Act(LONG keyPressed, LIST **elementSelected,
		LONG *itemLineNumber, void *actionParameter)
	{
	unsigned int myItem = 0;
	unsigned int curItem = 0;
//	char szTemp[80+1];
	int rc;

	if (keyPressed == M_ESCAPE)
		return 0;
		
	itemLineNumber=itemLineNumber;			/* Rid compiler warning */
	actionParameter=actionParameter;		/* Rid compiler warning */

	switch(keyPressed)
		{
		case M_ESCAPE:
			return(0);

		case M_INSERT:
			for (curItem = 0; curItem<(MailFilter_MaxFilters+1); curItem++)
			{
				if (MFC_Filters[curItem].expression[0] == 0)
					break;
			}
			
			MFC_Filters[curItem].enabled = true;
			MFC_Filters[curItem].enabledIncoming = true;
			MFC_Filters[curItem].enabledOutgoing = true;
			MFC_Filters[curItem].action = MAILFILTER_MATCHACTION_DROP_MAIL;

			MFC_Filters[curItem].notify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
			if (MFC_Notification_EnableInternalRcpt)	MFC_Filters[curItem].notify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
			if (MFC_Notification_EnableInternalSndr)	MFC_Filters[curItem].notify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
			if (MFC_Notification_EnableExternalRcpt)	MFC_Filters[curItem].notify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;
			if (MFC_Notification_EnableExternalSndr)	MFC_Filters[curItem].notify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING;
			
			
			rc = MFConfig_EditFilterDialog(curItem);		
			MF_Filter_Sort();
			return rc;

		case M_DELETE:
			myItem = *(unsigned int *)((*elementSelected)->otherInfo);

			for (curItem = 0; curItem<(MailFilter_MaxFilters+1); curItem++)
			{
				if (MFC_Filters[curItem].expression[0] == 0)
					break;
				if (curItem >= myItem)
					MFC_Filters[curItem] = MFC_Filters[curItem+1];
			}
			MF_Filter_Sort();
			return(-2);


		case M_SELECT:
			rc = MFConfig_EditFilterDialog(*(unsigned int *)((*elementSelected)->otherInfo));
			MF_Filter_Sort();
			return rc;
		}



	return(-1);	
	}



/****************************************************************************
** Edit filters.
*/
void MFConfig_EditFilters()
	{
	
	 char				szTemp[80+1];
	int							curItem = 0;
	long						rc = 0;
	int*						cI = NULL;
	
	/*------------------------------------------------------------------------
	**	Don't do this list if we should be exiting.
	*/
	if(NLM_exiting)
		return;

	/*------------------------------------------------------------------------
	**	At this point, the current list is the Main Menu.  If we begin adding
	**	new items to the current list, it would mess up the Main menu (to say
	**	the least).  So, we will save the Main Menu List on the List stack
	** (PushList) and then initialize a new list (set head and tail to NULL)
	**	by calling InitList().  Note that Lists use NWInitList() and Menus use
	**	NWInitMenu().
	*/
	NWSPushList(NLM_nutHandle);
	NWSInitList(NLM_nutHandle, free);

	// init function keys ...
	NWSEnableInterruptKey	(	K_F8,	MFConfig_Util_ImportListFromFile,	NLM_nutHandle	);
	NWSEnableInterruptKey	(	K_F9,	MFConfig_Util_ExportListToFile,	NLM_nutHandle	);
	
	/*------------------------------------------------------------------------
	**	This one function (NLM_ListSub) is called for both an NDS and Bindery
	**	style list.  The list that will actually be built depends on the value
	** of mode.
	*/
	
	for (curItem = 0; curItem<(MailFilter_MaxFilters); curItem++)
	{
		if (MFC_Filters[curItem].expression[0] == 0)
			break;

		cI = (int*)malloc(sizeof(int));
		*cI = curItem;
		
		szTemp[0] = ' '; szTemp[1] = ' ';
		switch (MFC_Filters[curItem].matchfield)
		{
		case MAILFILTER_MATCHFIELD_ALWAYS:
			szTemp[0] = '*';	break;
		case MAILFILTER_MATCHFIELD_ATTACHMENT:
			szTemp[0] = 'A';	break;
		case MAILFILTER_MATCHFIELD_EMAIL:
		case MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC:
			szTemp[0] = 'E';	break;
		case MAILFILTER_MATCHFIELD_SUBJECT:
			szTemp[0] = 'S';	break;
		case MAILFILTER_MATCHFIELD_SIZE:
			szTemp[0] = 's';	break;
		case MAILFILTER_MATCHFIELD_EMAIL_FROM:
			szTemp[0] = 'F';	break;
		case MAILFILTER_MATCHFIELD_EMAIL_TO:
		case MAILFILTER_MATCHFIELD_EMAIL_TOANDCC:
			szTemp[0] = 'T';	break;
		case MAILFILTER_MATCHFIELD_BLACKLIST:
			szTemp[0] = 'B';	break;
		case MAILFILTER_MATCHFIELD_IPUNRESOLVABLE:
			szTemp[0] = 'U';	break;
		default:
			szTemp[0] = '?';	break;
		}
		
		if (MFC_Filters[curItem].type == MAILFILTER_MATCHTYPE_NOMATCH)
		{
			szTemp[1] = '!';
		}
		
		strncpy(szTemp+2,MFC_Filters[curItem].expression,70);
		NWSAppendToList((NUTCHAR)szTemp, cI, NLM_nutHandle);
	}

	/*------------------------------------------------------------------------
	**	Display the list and allow user interaction.
	*/
	MFC_CurrentList = 1;
	rc = (long) NWSList(
		/* I-	header			*/	MENU_MAIN_EDIT_FILTERS,
		/*	I-	centerLine		*/	1,
		/*	I-	centerColumn	*/	0,
		/*	I-	height			*/	17,
		/* I-	width			*/  50,
		/* I-	validKeyFlags	*/	M_ESCAPE|M_SELECT|M_INSERT|M_DELETE|M_NO_SORT|M_REFRESH,
		/*	IO	element			*/	0,
		/*	I-	handle			*/  NLM_nutHandle,
		/*	I-	formatProcedure	*/	NULL,
		/* I-	actionProcedure	*/	MFConfig_EditFilters_Act,
		/* I-	actionParameter	*/	NULL
		);
	MFC_CurrentList = 0;
	
	/*------------------------------------------------------------------------
	**	Before returning, we must free the list items allocated by
	**	NLM_ListSubBuild...().  Then the Main Menu list context
	**	must be restored.  Note that Lists use NWDestroyList() and
	** Menus use NWDestroyMenu().
	*/
	NWSDestroyList(NLM_nutHandle);
	NWSPopList(NLM_nutHandle);

	NWSDisableInterruptKey(K_F8,NLM_nutHandle);
	NWSDisableInterruptKey(K_F9,NLM_nutHandle);

	if (rc == -2)
		MFConfig_EditFilters();

	return;
	}

/****************************************************************************
** MFConfig: Edit License Key
*/
void MFConfig_EditLicense()
	{
//	unsigned long	line;
	int	formSaved;
	 char	newLicenseKey[MAILFILTER_CONFIGURATION_LENGTH];

	if(NLM_exiting)
		return;

	newLicenseKey[0]=0;

	if (MFC_License_Key[0] != 0)
	{
		newLicenseKey[0]=( char)MFC_License_Key[0];
		newLicenseKey[1]=( char)MFC_License_Key[1];
		newLicenseKey[2]='.';
		newLicenseKey[3]='.';
		newLicenseKey[4]='.';
		newLicenseKey[5]=0;
	}
	
//	strcpy (newLicenseKey, MFC_License_Key);

	NWSPushList(NLM_nutHandle);
#ifdef _LIC_PORTAL
	NWSInitForm(NLM_nutHandle);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 1;
	NWSAppendCommentField (line, 2, (NUTCHAR)programMesgTable[MENU_MAIN_LICENSEKEY], NLM_nutHandle);
	NWSAppendStringField (line+1, 5, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, newLicenseKey, (NUTCHAR)"A..Za..z0..9 @_-+.[]()\"\\#'*?!§$%&/=<>|", NULL, NLM_nutHandle);

	formSaved = NWSEditPortalForm (
		/* I- header		*/ 0,
		/* I- center line	*/ 0,
		/* I- center col	*/ 40,
		/* I- form height	*/ line+3,
		/* I- form width	*/ 55,
		/* I- ctl flags		*/ F_VERIFY,
		/* I- form help		*/ F_NO_HELP,
		/* I- confirm msg	*/ MSG_SAVE_CHANGES,
		/* I- handle		*/ NLM_nutHandle
		);
	/*------------------------------------------------------------------------
	** This function returns TRUE if the form was saved, FALSE if not.
	** If the form was not saved you must restore all variables to their
	** original values manually
	*/
	if (formSaved)
	{
		strncpy (MFC_License_Key,					newLicenseKey,			MAILFILTER_CONFIGURATION_LENGTH);
	}
	NWSDestroyForm (NLM_nutHandle);
#else
	
	formSaved = NWSEditString ( 0,
					0,
					1,	/* height */
					60, /* width */
					MENU_MAIN_LICENSEKEY, /* header */
					NO_MESSAGE, /* prompt */
					(NUTCHAR)newLicenseKey, /* buffer */
					110, /* maxlen */
					EF_ANY, /* type */
					NLM_nutHandle,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((formSaved & E_SELECT) && (formSaved & E_CHANGE))
		formSaved = (-2);
		else
		formSaved = (-1);
	
	if (formSaved == -2)
	{
		strncpy(MFC_License_Key,newLicenseKey,120);
	}
	
#endif

	NWSPopList(NLM_nutHandle);

	return;
	}
	
/****************************************************************************
** MFConfig: Edit Config Settings
*/
void MFConfig_EditConfig()
	{
	unsigned long	line;
	int		formSaved;
	char	newDomainName[MAILFILTER_CONFIGURATION_LENGTH];
	char	newDomainHostname[MAILFILTER_CONFIGURATION_LENGTH];
	char	newEmailPostMaster[MAILFILTER_CONFIGURATION_LENGTH];
	char	newEmailMailFilter[MAILFILTER_CONFIGURATION_LENGTH];
	char	newScheduleTime[MAILFILTER_CONFIGURATION_LENGTH];
	char	newEmailOffice[50];
	BOOL	newNotification_InternalRcpt = (BOOL)MFC_Notification_EnableInternalRcpt;
	BOOL	newNotification_InternalSndr = (BOOL)MFC_Notification_EnableInternalSndr;
	BOOL	newNotification_ExternalRcpt = (BOOL)MFC_Notification_EnableExternalRcpt;
	BOOL	newNotification_ExternalSndr = (BOOL)MFC_Notification_EnableExternalSndr;
	BOOL	newNotification_AdminLogs = (BOOL)MFC_Notification_AdminLogs;
	BOOL	newNotification_AdminMailsKilled = (BOOL)MFC_Notification_AdminMailsKilled;
	BOOL	newNotification_AdminDailyReport = (BOOL)MFC_Notification_AdminDailyReport;
	LONG	newProblemDir_MaxSize = (LONG)MFC_ProblemDir_MaxSize;
	LONG	newProblemDir_MaxAge = (LONG)MFC_ProblemDir_MaxAge;
	char	newPathGwia[MAILFILTER_CONFIGURATION_LENGTH];
	char	newPathMflt[MAILFILTER_CONFIGURATION_LENGTH];
//	char	newDNSBLZone[MAILFILTER_CONFIGURATION_LENGTH];
	LONG	newScanDirNum = 0;
	LONG	newScanDirWait = 0;
	LONG	newGwiaVersion = 0;
	char	newControlPassword[MAILFILTER_CONFIGURATION_LENGTH];
	BOOL	newEnableIncomingRcptCheck = (BOOL)MFC_EnableIncomingRcptCheck;
  	BOOL	newEnableAttachmentDecoder = (BOOL)MFC_EnableAttachmentDecoder;
  	BOOL	newEnablePFA = (BOOL)MFC_EnablePFAFunctionality;
//  	BOOL	newDropUnresolvableRelayHosts = (BOOL)MFC_DropUnresolvableRelayHosts;

	if (NLM_exiting)
		return;

	NWSPushList(NLM_nutHandle);
	NWSInitForm(NLM_nutHandle);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 0;
	
	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_GENERAL_PATH_GWIA], NLM_nutHandle);
	strcpy (newPathGwia, MFC_GWIA_ROOT);
//	NWSAppendStringField (line, 20, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, (NUTCHAR)newPathGwia, (NUTCHAR)"A..Za..z \\/:_-+.0..9", NULL, NLM_nutHandle);
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newPathGwia, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"A..Za..z\\/{}[]()=#-_.,:;!§$&+~0..9", EF_SET, NULL, NLM_nutHandle); 
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_GENERAL_PATH_MFLT], NLM_nutHandle);
	strcpy (newPathMflt, MFC_MFLT_ROOT);
//	NWSAppendStringField (line, 20, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, (NUTCHAR)newPathMflt, (NUTCHAR)"A..Za..z \\/:_-+.0..9", NULL, NLM_nutHandle);
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newPathMflt, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"A..Za..z\\/{}[]()=#-_.,:;!§$&+~0..9", EF_SET, NULL, NLM_nutHandle); 
	line++;

	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_DOMAIN_DOMAIN_NAME], NLM_nutHandle);
	strcpy (newDomainName, MFC_DOMAIN_Name);
//	NWSAppendStringField (line, 20, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, (NUTCHAR)newDomainName, (NUTCHAR)"A..Za..z ,_-+.0..9", NULL, NLM_nutHandle);
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newDomainName, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"A..Za..z_-,.0..9", EF_SET, NULL, NLM_nutHandle); 

	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_DOMAIN_DOMAIN_HOSTNAME], NLM_nutHandle);
	strcpy (newDomainHostname, MFC_DOMAIN_Hostname);
//	NWSAppendStringField (line, 20, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, (NUTCHAR)newDomainHostname, (NUTCHAR)"A..Za..z _-+.0..9", NULL, NLM_nutHandle);
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newDomainHostname, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"A..Za..z_-.0..9", EF_SET, NULL, NLM_nutHandle); 
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_DOMAIN_POSTMASTER], NLM_nutHandle);
	strcpy (newEmailPostMaster, MFC_DOMAIN_EMail_PostMaster);
//	NWSAppendStringField (line, 20, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, (NUTCHAR)newEmailPostMaster, (NUTCHAR)"A..Za..z @_-+<>\".0..9", NULL, NLM_nutHandle);
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newEmailPostMaster, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"A..Za..z@_-+!<>.0..9", EF_SET, NULL, NLM_nutHandle); 
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_DOMAIN_MAILFILTER], NLM_nutHandle);
	strcpy (newEmailMailFilter, MFC_DOMAIN_EMail_MailFilter);
//	NWSAppendStringField (line, 20, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, (NUTCHAR)newEmailMailFilter, (NUTCHAR)"A..Za..z @_-+<>\".0..9", NULL, NLM_nutHandle);
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newEmailMailFilter, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"A..Za..z@_-+!<>.0..9", EF_SET, NULL, NLM_nutHandle); 
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_BWL_SCHEDULETIME], NLM_nutHandle);
	strcpy (newScheduleTime, MFC_BWL_ScheduleTime);
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newScheduleTime, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"0..9-:,", EF_SET, NULL, NLM_nutHandle); 
	line++;
	
//	NWSAppendCommentField (line, 1, (NUTCHAR)programMesgTable[EDIT_DNSBL_ZONENAME], NLM_nutHandle);
//	strcpy (newDNSBLZone, MFC_DNSBL_Zonename);
//	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (NUTCHAR)newDNSBLZone, MAILFILTER_CONFIGURATION_LENGTH, (NUTCHAR)"A..Za..z-_+.0..9", EF_SET, NULL, NLM_nutHandle); 
//	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"General E-Mail Address w/o Domain", NLM_nutHandle);
	NWSAppendCommentField (line, 61, (NUTCHAR)"(blank=disabled)", NLM_nutHandle);
	strcpy (newEmailOffice, MFC_Multi2One);
	NWSAppendStringField (line+1, 3, 50, NORMAL_FIELD, (NUTCHAR)newEmailOffice, (NUTCHAR)"A..Za..z_-+!.0..9", EF_SET, NLM_nutHandle);
	line += 2;
	
	NWSAppendCommentField (line, 1, (NUTCHAR)"GWIA Mode/Version: (see manual)", NLM_nutHandle);
	newGwiaVersion = (unsigned long)MFC_GWIA_Version;
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newGwiaVersion, 550, 600, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"MailFilter Control/Config Password:", NLM_nutHandle);
	strcpy (newControlPassword, MFC_ControlPassword);
	NWSAppendPasswordField(line, 40, 20, NORMAL_FIELD, (NUTCHAR)newControlPassword, 50, NULL, true, NULL, '*', NLM_nutHandle); 
	line++;
	
	NWSAppendCommentField (line, 1, (NUTCHAR)"Enable PFA Functionality:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnablePFA, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"Enable Incoming Rcpt Rule Check:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnableIncomingRcptCheck, NULL, NLM_nutHandle);
	line++;
	
//	NWSAppendCommentField (line, 1, (NUTCHAR)"Drop Unresolvable Relay Hosts:", NLM_nutHandle);
//	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newDropUnresolvableRelayHosts, NULL, NLM_nutHandle);
//	line++;
	
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"Virus Scan Integration:", NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"Number of MFSCAN Directories:", NLM_nutHandle);
	newScanDirNum = (unsigned long)MFC_MAILSCAN_Scan_DirNum;
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newScanDirNum, 0, 50, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"Seconds to wait for Real Time Scan:", NLM_nutHandle);
	newScanDirWait = (unsigned long)MFC_MAILSCAN_Scan_DirWait;
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newScanDirWait, 0, 600, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"Decode Attachments for VScan:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnableAttachmentDecoder, NULL, NLM_nutHandle);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"Default Notify On Filter Match:", NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"Internal Recipient:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_InternalRcpt, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"Internal Sender:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_InternalSndr, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"External Recipient:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_ExternalRcpt, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"External Sender:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_ExternalSndr, NULL, NLM_nutHandle);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"Notify Admin On Log Cycle Error:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_AdminLogs, NULL, NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"Send Admin Daily Status Report:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_AdminDailyReport, NULL, NLM_nutHandle);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (NUTCHAR)"Problem Directory Cleanup:", NLM_nutHandle);
	line++;
	
	NWSAppendCommentField (line, 3, (NUTCHAR)"Maximum Total Size (kBytes):", NLM_nutHandle);
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newProblemDir_MaxSize, 0, 1000000, NULL, NLM_nutHandle);
	NWSAppendCommentField (line, 65, (NUTCHAR)"(0=disabled)", NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"Maximum File Age (Days):", NLM_nutHandle);
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newProblemDir_MaxAge, 0, 10000, NULL, NLM_nutHandle);
	NWSAppendCommentField (line, 65, (NUTCHAR)"(0=disabled)", NLM_nutHandle);
	line++;

	NWSAppendCommentField (line, 3, (NUTCHAR)"Notify Admin On Cleanup:", NLM_nutHandle);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_AdminMailsKilled, NULL, NLM_nutHandle);
	line++;

	formSaved = NWSEditForm (
		NULL,		//headernum	//0
		1,		//line
		0,		//col
		23,		//portalHeight
		80,		//portalWidth
		line+1,	//virtualHeight,
		78,		//virtualWidth,
		TRUE,	//ESCverify,
		TRUE,	//forceverify,
		MSG_SAVE_CHANGES,	//confirmMessage,
		NLM_nutHandle
	);

	/*------------------------------------------------------------------------
	** This function returns TRUE if the form was saved, FALSE if not.
	** If the form was not saved you must restore all variables to their
	** original values manually
	*/
	if (formSaved)
	{
		strncpy (MFC_GWIA_ROOT,						newPathGwia,			MAILFILTER_CONFIGURATION_LENGTH);
		strncpy (MFC_MFLT_ROOT,						newPathMflt,			MAILFILTER_CONFIGURATION_LENGTH);
		strncpy (MFC_DOMAIN_Name,					newDomainName,			MAILFILTER_CONFIGURATION_LENGTH);
		strncpy (MFC_DOMAIN_Hostname,				newDomainHostname,		MAILFILTER_CONFIGURATION_LENGTH);
		strncpy (MFC_DOMAIN_EMail_PostMaster,		newEmailPostMaster,		MAILFILTER_CONFIGURATION_LENGTH);
		strncpy (MFC_DOMAIN_EMail_MailFilter,		newEmailMailFilter,		MAILFILTER_CONFIGURATION_LENGTH);
		strncpy (MFC_BWL_ScheduleTime,				newScheduleTime,		MAILFILTER_CONFIGURATION_LENGTH);
//		strncpy (MFC_DNSBL_Zonename,				newDNSBLZone,			MAILFILTER_CONFIGURATION_LENGTH);
		strncpy (MFC_Multi2One,						newEmailOffice,			50);

		// check if pass has changed, if yes encrypt ...
		if (strcmp(MFC_ControlPassword,newControlPassword) != 0)
			MFU_strxor((char*)newControlPassword,(char)18);
			
		strncpy (MFC_ControlPassword,				newControlPassword,		50);
		
		MFC_Notification_EnableInternalRcpt =(int)newNotification_InternalRcpt;
		MFC_Notification_EnableInternalSndr =(int)newNotification_InternalSndr;
		
		MFC_Notification_EnableExternalRcpt =(int)newNotification_ExternalRcpt;
		MFC_Notification_EnableExternalSndr =(int)newNotification_ExternalSndr;

		MFC_Notification_AdminLogs 			=(int)newNotification_AdminLogs;
		MFC_Notification_AdminDailyReport	=(int)newNotification_AdminDailyReport;

		MFC_Notification_AdminMailsKilled	=(int)newNotification_AdminMailsKilled;
		MFC_ProblemDir_MaxSize  			=(int)newProblemDir_MaxSize;
		MFC_ProblemDir_MaxAge 				=(int)newProblemDir_MaxAge;

		MFC_MAILSCAN_Scan_DirWait			=(int)newScanDirWait;
		MFC_MAILSCAN_Scan_DirNum			=(int)newScanDirNum;
		MFC_EnableAttachmentDecoder			=(int)newEnableAttachmentDecoder;
		MFC_EnablePFAFunctionality			=(int)newEnablePFA;

		MFC_GWIA_Version					=(int)newGwiaVersion;
		
		switch (MFC_GWIA_Version)
		{	
			case 550:
			case 600:
				break;
			default:
				MFC_GWIA_Version = 550;
				break;
		}

		MFC_EnableIncomingRcptCheck			=(int)newEnableIncomingRcptCheck;
//		MFC_DropUnresolvableRelayHosts		=(int)newDropUnresolvableRelayHosts;
		
	}

	NWSDestroyForm (NLM_nutHandle);
	NWSPopList(NLM_nutHandle);

	return;
	}
	

/****************************************************************************
** Main menu action procedure.
*/
int NLM_MenuMainAct(int index, void *parm)
	{
	parm=parm;	 /* Rid compiler warning. */

	NWSDisableInterruptKey	(	K_F8,	NLM_nutHandle	);
	MFConfig_UI_ShowKeys(MFCONFIG_KEYS_NONE);

	/*------------------------------------------------------------------------
	**	Perform the user-selected action.
	*/
	switch(index)
		{
		case (-1): 		/* ESCAPE KEY PRESSED */
			if(NLM_VerifyProgramExit())
				return(0);

			break;

		case MENU_MAIN_EDIT_CONFIGURATION:
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_SELECT|MFCONFIG_KEYS_SAVE);
			MFConfig_EditConfig();
			break;

		case MENU_MAIN_EDIT_FILTERS:
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_IMPORT|MFCONFIG_KEYS_EXPORT|MFCONFIG_KEYS_NEW|MFCONFIG_KEYS_DELETE|MFCONFIG_KEYS_SELECT);
			MFConfig_EditFilters();
			break;

		case MENU_MAIN_LICENSEKEY:
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_CANCEL);
			MFConfig_EditLicense();
			break;

		case MENU_MAIN_SAVE_CONFIG:
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_CANCEL);
			if (NLM_VerifySaveExit())
			{
				MF_ConfigWrite();
				return(0);
			}
			
			break;
		}

	/*------------------------------------------------------------------------
	**	If we should be exiting, pretend that ESCAPE was pressed.
	*/
	if(NLM_exiting)
		return(0);


	NWSEnableInterruptKey	(	K_F8,	MFConfig_ImportLicenseKey,	NLM_nutHandle	);
	MFConfig_UI_ShowKeys(MFCONFIG_KEYS_IMPORT);

	return(-1);
	}

/****************************************************************************
** Main menu.
*/
void NLM_MenuMain(void)
	{
	LIST *defaultOption;

	/*------------------------------------------------------------------------
	**	Don't do this menu if we should be exiting.
	*/
	if(NLM_exiting)
		return;

	if(NLM_noUserInterface)
		return;

	/*------------------------------------------------------------------------
	**	At this point, the current list is uninitialized (being that it is the
	** first list of the program.)  Before using the current list it must be
	**	initialized (set head and tail to NULL) by calling InitMenu().
	**	Note that Lists use NWInitList() and Menus use NWInitMenu().
	*/
	NWSInitMenu(NLM_nutHandle);

	/*------------------------------------------------------------------------
	**	Insert menu items.  Note that the insertion order does not matter being
	**	that NWSMenu() will always sort the Menu selections automatically.
	** The defaultOption stores a pointer to the menu item which we wish to be
	** highlighed by default.
	*/
	
	NWSAppendToMenu(MENU_MAIN_EDIT_CONFIGURATION, MENU_MAIN_EDIT_CONFIGURATION, NLM_nutHandle);

	NWSAppendToMenu(MENU_MAIN_LICENSEKEY, MENU_MAIN_LICENSEKEY, NLM_nutHandle);
	
//	NWSAppendToMenu(MENU_MAIN_EDIT_FLT_ATTACHMENT, MENU_MAIN_EDIT_FLT_ATTACHMENT, NLM_nutHandle);
//	NWSAppendToMenu(MENU_MAIN_EDIT_FLT_SENDER, MENU_MAIN_EDIT_FLT_SENDER, NLM_nutHandle);
	NWSAppendToMenu(MENU_MAIN_EDIT_FILTERS, MENU_MAIN_EDIT_FILTERS, NLM_nutHandle);
	
	defaultOption = NWSAppendToMenu(MENU_MAIN_SAVE_CONFIG, MENU_MAIN_SAVE_CONFIG, NLM_nutHandle);

	NWSEnableInterruptKey	(	K_F8,	MFConfig_ImportLicenseKey,	NLM_nutHandle	);
	MFConfig_UI_ShowKeys(MFCONFIG_KEYS_IMPORT|MFCONFIG_KEYS_EXIT);

	/*------------------------------------------------------------------------
	**	Display the menu and allow user interaction.
	*/
	NWSMenu(
		/*	header				*/	MENU_MAIN__HDR,
		/*	centerLine			*/	0,					/* [0,0] means Center of screen*/
		/*	centerColumn		*/	0,
		/*	defaultElement		*/	defaultOption,		/* Could use NULL here desired */			
		/*	actionProcedure		*/	NLM_MenuMainAct,
		/*	handle				*/	NLM_nutHandle,
		/*	actionParameter		*/	NULL
		);
	
	/*------------------------------------------------------------------------
	**	Before returning, we must free the list items allocated by
	**	NWSAppendToMenu(). Note that Lists use NWDestroyList() and Menus use
	**	NWDestroyMenu().
	*/
	NWSDestroyMenu(NLM_nutHandle);

	return;
	}

/****************************************************************************
** Program shut-down.
*/
void NLM_ShutDown(void)
	{

	if (NLM_noUserInterface)
		return;

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
#ifndef __NOVELL_LIBC__
	int handlerThreadGroupID;
#endif
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
#ifndef __NOVELL_LIBC__
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
	long	cCode;
#ifdef _WITH_LIBC
	struct LoadDefinitionStructure* NLMHandle;
#else
	int NLMHandle;
#endif
	#ifdef __NOVELL_LIBC__
	rtag_t	tagID;
	#else
	LONG	tagID;
	#endif
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

	if (NLM_noUserInterface)
		return;

//	NLM_screenID=GetCurrentScreen();
#ifdef _WITH_LIBC
	screenResourceTag = AllocateResourceTag(NLMHandle, programMesgTable[SCREEN_NAME], ScreenSignature);
	if (screenResourceTag == NULL) {
		OutputToScreen(CONSOLE_SCREEN,"MFConfig Error: Unable to allocate resource tag.\r\n");
	}
	OpenScreen(programMesgTable[SCREEN_NAME], screenResourceTag, &NLM_screenID);
	if (NLM_screenID == NULL) {
		OutputToScreen(CONSOLE_SCREEN, "MFConfig Error: Could not open screen.\r\n");
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
	tagID=AllocateResourceTag(
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
		ConsolePrintf("MFCONFIG: Error creating Resource Tag!\n");
		NLM_exiting = TRUE;
		goto END;
		}

	cCode=(long)NWSInitializeNut(
		/*	utility				*/	PROGRAM_NAME,
		/*	version				*/	PROGRAM_VERSION,
		/*	headerType			*/	SMALL_HEADER,
		/*	compatibilityType	*/	NUT_REVISION_LEVEL,
#ifdef __NOVELL_LIBC__
		/*	messageTable		*/	(char**)programMesgTable,
#else
		/*	messageTable		*/	(unsigned char**)programMesgTable,
#endif
		/*	helpScreens			*/	NULL,
		/*	screenID			*/	NLM_screenID,
		/*	resourceTag			*/	tagID,
		/*	handle				*/	&NLM_nutHandle
		);
	if(cCode != NULL)
		{
		ConsolePrintf("MFConfig: FATAL ERROR: NWSInitializeNut returned %d.\n",cCode);
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

void MFConfig_UI_ShowKeys(int keys)
{
	char szTemp[80+2];
	int curItem;
	memset(szTemp,' ',80);
	szTemp[81]=0;
	NWSShowLineAttribute ( 24 , 0 , (NUTCHAR)szTemp , VREVERSE , 80 , NLM_nutHandle->screenID );
	
	curItem=1;
	if (chkFlag(keys,MFCONFIG_KEYS_IMPORT)) 	{	strncpy(szTemp+curItem, "<F8> Import ",12);		curItem=curItem+14;	}
	if (chkFlag(keys,MFCONFIG_KEYS_EXPORT)) 	{	strncpy(szTemp+curItem, "<F9> Export ",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFCONFIG_KEYS_NEW)) 		{	strncpy(szTemp+curItem, "<INS> New   ",12);		curItem=curItem+14;	}
	if (chkFlag(keys,MFCONFIG_KEYS_DELETE)) 	{	strncpy(szTemp+curItem, "<DEL> Delete",12); 	curItem=curItem+14;	}
	if (chkFlag(keys,MFCONFIG_KEYS_SELECT)) 	{	strncpy(szTemp+curItem, "<RET> Select",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFCONFIG_KEYS_EXIT)) 		{	strncpy(szTemp+curItem, "<ESC> Exit  ",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFCONFIG_KEYS_SAVE)) 		{	strncpy(szTemp+curItem, "<ESC> Save  ",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFCONFIG_KEYS_CANCEL)) 	{	strncpy(szTemp+curItem, "<ESC> Cancel",12);		curItem=curItem+14; }

	NWSShowLineAttribute ( 24 , (unsigned long)((80-curItem)/2) , (NUTCHAR)szTemp , VREVERSE , (unsigned long)curItem , NLM_nutHandle->screenID );

}

/****************************************************************************
** Program start.
*/

void main(int argc, char *argv[])
	{

	char szTemp[80+2];
	int action = 0;
	unsigned int tmp = 0;
	char* programName;
#ifdef __NOVELL_LIBC__
	struct utsname un;
#endif
	
	++NLM_threadCnt;
	programName = "MFCONFIG";

#ifdef __NOVELL_LIBC__
	uname(&un);
	strcpy(NLM_ServerName,un.servername);
#else
	GetFileServerName(0, NLM_ServerName);
#endif

	if (argc > 1)
	{
/*		if (memicmp(argv[1],"/upgrade",8) == 0)
			action = 1;
		
		NLM_noUserInterface = TRUE;
*/

		ConsolePrintf ("MFCONFIG.NLM: Using %s as Configuration Directory.\n",argv[1]);
		strncpy(MFC_ConfigDirectory,argv[1],MAX_PATH-1);
		sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"%s\\CONFIG.BIN",argv[1]);
		sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"%s\\CONFIG.BAK",argv[1]);
		sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"%s\\CONFIG.ERR",argv[1]);
	} else {
		strcpy(MFC_ConfigDirectory,"SYS:\\ETC\\MAILFLT");
		strcpy(MAILFILTER_CONFIGURATION_MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.BIN");
		strcpy(MAILFILTER_CONFIGBACKUP__MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.BAK");
		strcpy(MAILFILTER_CONFIGERROR___MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.ERR");								
	}

	NLM_noUserInterface = FALSE;

	// Check for other NLM.
	
	// Upgrade ?
	tmp = strlen(argv[0]) - 10;
	if (memicmp(argv[0]+tmp,"mfupgr.nlm",10) == 0)
	{
		programName = "MFUPGR";
		action = 1;
		NLM_noUserInterface = TRUE;
	}

	// Install ?
	tmp = strlen(argv[0]) - 12;
	if (memicmp(argv[0]+tmp,"mfinstal.nlm",12) == 0)
	{
		programName = "MFINSTAL";
		action = 2;
		NLM_noUserInterface = TRUE;
	}

	if (action != 0)
		ConsolePrintf("%s Version %s\n",programName,programMesgTable[PROGRAM_VERSION]);
	
	NLM_Initialize();
	
	if (MF_ConfigRead() == FALSE)
	{
		if (NLM_noUserInterface)
			ConsolePrintf("%s: Configuration File seems to be invalid.\n",programName);
			
		NLM_ShutDown();

		--NLM_threadCnt;
		return;
	}

	switch(action)
	{
	case 0:

		memset(szTemp,' ',80);
		szTemp[81]=0;
		sprintf(szTemp,"  MailFilter/ax Configuration Editor NLM/CUI Version %s",
										programMesgTable[PROGRAM_VERSION]);
		szTemp[81]=0;
		NWSShowLineAttribute ( 0 , 0 , (NUTCHAR)szTemp , VNORMAL , 80 , NLM_nutHandle->screenID );
	
		memset(szTemp,' ',80);
		szTemp[81]=0;
		sprintf(szTemp,"  Path: %s",MFC_ConfigDirectory);
		szTemp[81]=0;
		NWSShowLineAttribute ( 1 , 0 , (NUTCHAR)szTemp , VNORMAL , 80 , NLM_nutHandle->screenID );

		memset(szTemp,' ',80);
		szTemp[81]=0;
		sprintf(szTemp,"  Editor Version: 001.%03i",
							MAILFILTER_CONFIGURATION_THISBUILD);
		NWSShowLineAttribute ( 2 , 0 , (NUTCHAR)szTemp , VNORMAL , 80 , NLM_nutHandle->screenID );

		memset(szTemp,' ',80);
		szTemp[81]=0;
		sprintf(szTemp,"  CFFile Version: 001.%03i",
							MFC_ConfigBuild);
		NWSShowLineAttribute ( 3 , 0 , (NUTCHAR)szTemp , VNORMAL , 80 , NLM_nutHandle->screenID );


	
		NLM_MenuMain();
		break;
	case 1:
		ConsolePrintf("%s: Upgrading Configuration File: %d -> %d\n",programName,MFC_ConfigBuild,MAILFILTER_CONFIGURATION_THISBUILD);
		if (MF_ConfigWrite() == TRUE)
			ConsolePrintf("%s: Upgrade Done.\n",programName);
			else 
			ConsolePrintf("%s: *ERROR*\n",programName);
		break;
	case 2:
		ConsolePrintf("%s: Installing ... (CFG%d)\n",programName,MAILFILTER_CONFIGURATION_THISBUILD);
		if (MF_ConfigWrite() == TRUE)
			ConsolePrintf("%s: Installation Done.\n",programName);
			else 
			ConsolePrintf("%s: *ERROR*\n",programName);
		break;
	}
	
	NLM_ShutDown();
	
	--NLM_threadCnt;

	return;
	}

