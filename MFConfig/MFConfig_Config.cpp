/****************************************************************************
**	File:	MFCONFIG.CPP
**
**	Desc:	MailFilter/ax professional Configuration
**
**  (C) Copyright 2000-2002 Christian Hofstädtler. All Rights Reserved.
**							
*/


	#include "stdafx.h"
	#include <sys/stat.h>
	#include <stdio.h>
	
//	#define MAILFILTER_VERSION7_MIGRATION	1
	#define _MAILFILTER_MFCONFIG_STORAGE
	#include "../MailFilter/Config/MFConfig.h"

	MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];
	static MailFilter_Filter id[MailFilter_MaxFilters];		// this is for sorting ONLY!

/*void MF_RegError(int errcode, const regex_t *preg)
{
	char szErrBuf[3500];
	if (!errcode)
		return;
		
	regerror(errcode, preg, szErrBuf, sizeof(szErrBuf));

	char szTemp[4000];
	
	sprintf(szTemp,"Regular Expression Error:\n%s\n",szErrBuf);
	
	// TODO: add afxmessagebox

}*/

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
			if (curId+1 > MailFilter_MaxFilters) { break; }
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
			if (curId+1 > MailFilter_MaxFilters) {	break; }
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

//
// Init the Filter Lists
//
int MF_Filter_InitLists()
{
	FILE* cfgFile;
	int curItem;
	long rc1;
	long rc2;
	char szTemp[1002];

	// version 8

	cfgFile = fopen(MAILFILTER_CONFIGURATION_MAILFILTER,"rb");
#ifdef WIN32
	if (cfgFile == NULL)
		AfxMessageBox("Invalid Filehandle while reading filters.",MB_OK,0);
#endif
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
		
		rc1 = (long)fread(szTemp,sizeof(char),1000,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc1+1,SEEK_CUR);
		
		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].expression[0]=0;		

		rc2 = (long)fread(szTemp,sizeof(char),1000,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc2+1,SEEK_CUR);

		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].name[0]=0;
		
		if (MFC_Filters[curItem].expression[0]==0)
			break;	

	}

	fclose(cfgFile);
	return 0;
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

	if (MFC_ControlPassword != NULL)			{	free(MFC_ControlPassword);			MFC_ControlPassword			= NULL;	}
	if (MFC_Multi2One != NULL)					{	free(MFC_Multi2One);				MFC_Multi2One				= NULL;	}

}

void MFU_strxor(char* string, char xorVal)
{
	if (string == NULL) return;
	
	do {
		if (*string == 0) break;
		
		*string = (char)(*string ^ xorVal);
	} while(*string++);
}

// Reads the configuration ...
int MF_ConfigRead()
{
	int rc;
	struct stat statInfo;
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
	MFC_ControlPassword						[0]	= 0;

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
		// GWIA Version is filled in by Install.W32 !!!
//		MFC_GWIA_Version					= 600;
		
		MFC_ControlPassword[0]				= 0;
		
		if (( fImp = fopen("SYS:\\ETC\\MAILFLT\\DEFAULTS\\FILTERS.BIN","rb")) != NULL ) {
		fseek(fImp, 54, SEEK_SET);

		while (!feof(fImp))
		{
			if (curItem > MailFilter_MaxFilters)
				break;

			MFC_Filters[curItem].matchfield = (char)fgetc(fImp);
			MFC_Filters[curItem].notify = (char)fgetc(fImp);
			MFC_Filters[curItem].action = (char)fgetc(fImp);
			MFC_Filters[curItem].enabled = (char)fgetc(fImp);
			MFC_Filters[curItem].enabledIncoming = (char)fgetc(fImp);
			MFC_Filters[curItem].enabledOutgoing = (char)fgetc(fImp);
			
			rc = (long)fread(szTemp,sizeof(char),1000,fImp);
			fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
			
			if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
				else MFC_Filters[curItem].expression[0]=0;		

			rc = (long)fread(szTemp,sizeof(char),1000,fImp);
			fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

			if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
				else MFC_Filters[curItem].name[0]=0;
			
			if (MFC_Filters[curItem].expression[0]==0)
				break;

			curItem++;
		}
		
		fclose(fImp);
		
		} else {
		
			AfxMessageBox("Error: Importing the Default Filter List failed.",MB_OK,0);
			// TODO: Add AFXMESSAGEBOX HERE ConsolePrintf("*** ERROR IMPORTING DEFAULT FILTER LIST ***\n");
		}


/*	if ((!rc) && (MFC_ControlPassword[0] != 0))
	{
		unsigned char* readPass = (unsigned char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
		readPass[0] = 0;
		NWSEditString( 0, 0, 1, 50, 0 , EDIT_UTIL_GETCONTROLPASSWORD, readPass, 50, EF_MASK|EF_NOSPACES|EF_SET|EF_NOCONFIRM_EXIT, NLM_nutHandle, NULL, NULL, (unsigned char*)"a..z0..9A..Z" );
		
		strlwr((char*)readPass);
		MFU_strxor((char*)readPass,(char)18);
		if (strcmp((const char*)readPass, MFC_ControlPassword) != 0)
			rc = 500;

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

			goto MF_ConfigRead_ERR;
		}
	}
*/
	
// Error Handling goes here ...
MF_ConfigRead_ERR:
	{
		if (rc)
		{
			MF_ConfigFree();
			// We just let the caller handle this for us...
			//AfxMessageBox("An error occoured while reading the configuration.",MB_OK,0);
		}
	

/*		if (rc == 99)
		{	
		
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
							(unsigned char*)"MFConfig found a Configuration File Backup that could be valid.\n\nDo you want to try the Backup?\n",
							NLM_nutHandle
							);
							
						switch (rc)
						{
						
						case (0xFFFFFFFF):
							NWSTrace(NLM_nutHandle,(unsigned char*)"Error occoured.");
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
							NWSTrace(NLM_nutHandle,(unsigned char*)"*#* CRASH *#* -- Memory Corruption?");
							break;
							
						}
					
				}
			}
			}*/
		}

		if (rc)
		{
			//ConsolePrintf("MFCONFIG: Error Code %d\n",rc);
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
		
#define doNull(howMany)	{ iMax = ftell(cfgFile); for (iCnt=0;iCnt<(howMany-iMax);iCnt++) fputc(0,cfgFile); }

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
	// removed in a later v8

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
	// removed in a later v8
//	fprintf(cfgFile, MFC_DropUnresolvableRelayHosts == 0 ? "0" : "1");

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



//	flushall();
	fclose(cfgFile);

	return TRUE;	
}


