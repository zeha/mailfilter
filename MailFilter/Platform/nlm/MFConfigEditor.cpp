/*
 +
 +		MFConfigEditor.cpp
 +		
 +		Configuration Editor Code
 +		
 +		Copyright 2001-2004 Christian Hofstädtler.
 +		
 +		
 +		
 */
 
#define _MFD_MODULE			"MFConfigEditor.cpp"

#include "MailFilter.h"
#include "MFConfig.h++"
#include "MFConfig-Filter.h++"
#include <pcreposix45.h>

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
#define MFCONFIG_KEYS_SORT			0x0100

#ifndef NUT_SEVERITY_FATAL
#define NUT_SEVERITY_FATAL 2
#endif


static void MFConfig_Util_Sort(void* nutHandle);


static void MFConfig_UI_ShowKeys(int keys)
{
	char szTemp[80+2];
	int curItem;
	memset(szTemp,' ',80);
	szTemp[81]=0;
	NWSShowLineAttribute ( 24 , 0 , (_MF_NUTCHAR)szTemp , VREVERSE , 80 , (struct ScreenStruct*)MF_NutInfo->screenID );
	
	curItem=1;
	if (chkFlag(keys,MFCONFIG_KEYS_SORT)) 		{	strncpy(szTemp+curItem, "<F7> Sort",9);			curItem=curItem+12;	}
	if (chkFlag(keys,MFCONFIG_KEYS_IMPORT)) 	{	strncpy(szTemp+curItem, "<F8> Import",11);		curItem=curItem+14;	}
	if (chkFlag(keys,MFCONFIG_KEYS_EXPORT)) 	{	strncpy(szTemp+curItem, "<F9> Export",11);		curItem=curItem+14; }
	if (chkFlag(keys,MFCONFIG_KEYS_NEW)) 		{	strncpy(szTemp+curItem, "<INS> New",9);			curItem=curItem+12;	}
	if (chkFlag(keys,MFCONFIG_KEYS_DELETE)) 	{	strncpy(szTemp+curItem, "<DEL> Delete",12); 	curItem=curItem+14;	}
	if (chkFlag(keys,MFCONFIG_KEYS_SELECT)) 	{	strncpy(szTemp+curItem, "<RET> Select",12);		curItem=curItem+14; }
	if (chkFlag(keys,MFCONFIG_KEYS_EXIT)) 		{	strncpy(szTemp+curItem, "<ESC> Exit",10);		curItem=curItem+13; }
	if (chkFlag(keys,MFCONFIG_KEYS_SAVE)) 		{	strncpy(szTemp+curItem, "<ESC> Save",10);		curItem=curItem+13; }
	if (chkFlag(keys,MFCONFIG_KEYS_CANCEL)) 	{	strncpy(szTemp+curItem, "<ESC> Cancel",12);		curItem=curItem+14; }

	NWSShowLineAttribute ( 24 , (unsigned long)((80-curItem)/2) , (_MF_NUTCHAR)szTemp , VREVERSE , (unsigned long)curItem , (struct ScreenStruct*)MF_NutInfo->screenID );

}


static void MF_RegError(int errcode, const regex_t *preg)
{
	char szErrBuf[3500];
	if (!errcode)
		return;
		
	regerror(errcode, preg, szErrBuf, sizeof(szErrBuf));

	char szTemp[4000];
	
	sprintf(szTemp,"Regular Expression Error:\n%s\n",szErrBuf);
	
	NWSDisplayInformation (
	
		NULL,
		1,
		0,
		0,
		ERROR_PALETTE,
		VREVERSE,
		(_MF_NUTCHAR) szTemp,
		MF_NutInfo
		);
}

static void MFConfig_ImportLicenseKey(void *)
{

	char szTemp[MAX_PATH];
	char szTemp2[MAX_PATH];
	char szLocalServerName[64];
	char* cResult;
	int rc;
	FILE *fImp;
	int formatPosition = 0;
	
	szTemp[0] = 0;
	
	MF_GetServerName(szLocalServerName, sizeof(szLocalServerName));
	
	rc = NWSEditString ( 0,
					0,
					1,	/* height */
					50, /* width */
					EDIT_UTIL_IMPORTFILE_GETLICENSE, /* header */
					NO_MESSAGE, /* prompt */
					(_MF_NUTCHAR)szTemp, /* buffer */
					49, /* maxlen */
					EF_ANY, /* type */
					MF_NutInfo,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((rc & E_SELECT) && (rc & E_CHANGE))
	{
		if (access(szTemp,F_OK) != 0)
		{
			NWSDisplayErrorText ( ERROR_FILE_NOT_FOUND , NUT_SEVERITY_FATAL , MF_NutInfo , szTemp );
			
		} else {
		
/*
    -----------------------MAILFILTER  CLEARINGHOUSE-----------------------
    SERVER: IONUSMAIL - VSCAN: 1 - ALIAS: 1 - MCOPY: 1
    KEY: 3cTZ11RZtINjD[NjHJiZNJwZS0XVViVF0P[e32fmQ54oPv7C6Xfby98w0MLx00K2
    -----------------------ELECTRONIC  KEY  DELIVERY-----------------------
*/

			NWSStartWait(0,0,MF_NutInfo);
		
			fImp = fopen(szTemp,"rt");
			if (!fImp)
			{
				NWSDisplayErrorText ( ERROR_FILE_NOT_FOUND , NUT_SEVERITY_FATAL , MF_NutInfo , szTemp );
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
						  MF_GlobalConfiguration.LicenseKey = cResult+5;
						  if (formatPosition==2) formatPosition = 3;
						} else formatPosition=0;		
				}

				if (formatPosition == 1) {
					sprintf(szTemp2,"SERVER: ",szLocalServerName);
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
			
			NWSEndWait(MF_NutInfo);
			
			NWSUngetKey(UGK_SPECIAL_KEY,UGK_REFRESH_KEY,MF_NutInfo);

			if (formatPosition != 3)
				NWSDisplayErrorText ( ERROR_LICENSE_NOT_FOUND , NUT_SEVERITY_FATAL , MF_NutInfo , NULL );

		}
		
	
	} else return;
				
}

class CompareFilters {
public:
	bool operator() 
			(const MailFilter_Configuration::Filter& f1, 
			 const MailFilter_Configuration::Filter& f2)
		{
			bool rc = false;
		
			if (f1.enabled == false)
				rc = false;
			else
			if (f1.action == f2.action)
			{
				if (f1.matchfield == f2.matchfield)
					rc = f1.expression > f2.expression;
				else
				{
					if (f1.matchfield == MailFilter_Configuration::attachment)
						rc = true;
					else
					if (f2.matchfield == MailFilter_Configuration::attachment)
						rc = false;
					else
					if (f1.matchfield == MailFilter_Configuration::blacklist)
						rc = true;
					else
					if (f2.matchfield == MailFilter_Configuration::blacklist)
						rc = false;
					else
						rc = f1.matchfield > f2.matchfield;
				}
			}
			else
			{
				if (f1.action == MailFilter_Configuration::pass)
				{
					rc = true;
				}
				else
				{
					if (f2.action == MailFilter_Configuration::pass)
						rc = false;
				}
			}
			
			return rc;
		}
};


static void MFConfig_AddFilterItem(MailFilter_Configuration::Filter flt, int curItem)
{
	 char				szTemp[80+1];

	int *cI;
	cI = (int*)malloc(sizeof(int));
	*cI = curItem;
	
	szTemp[0] = ' '; szTemp[1] = ' ';
	switch (flt.matchfield)
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
	
	if (flt.type == MAILFILTER_MATCHTYPE_NOMATCH)
	{
		szTemp[1] = '!';
	}
	
	strncpy(szTemp+2,flt.expression.c_str(),70);
	szTemp[73]=0;
	NWSAppendToList((_MF_NUTCHAR)szTemp, cI, MF_NutInfo);
}
	
static void MFConfig_UpdateFilterList()
{
	// delete old stuff
	{
		LIST* start = NWSGetListHead(MF_NutInfo);
		while (start)
		{
			start = NWSDeleteFromList(start,MF_NutInfo);
		}
	}
	// insert new stuff
	{
	
		MAILFILTER_CONFIGURATION_FILTERLISTTYPE::const_iterator first = MF_GlobalConfiguration.filterList.begin();
		MAILFILTER_CONFIGURATION_FILTERLISTTYPE::const_iterator last = MF_GlobalConfiguration.filterList.end();

		int curItem = 0;
		for (; first != last; ++first)
		{
			MFConfig_AddFilterItem(*first,curItem);
			++curItem;
		}
	}
}


static void MFU_strxor(char* string, char xorVal)
{
	if (string == NULL) return;
	
	do {
		if (*string == 0) break;
		
		*string = (char)(*string ^ xorVal);
// had xor here !
	} while(*string++);
}

// Reads the configuration ...
static int MF_ConfigReadXXX()
{
	bool freshInstall = false;

/*	//
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
*/
/*

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
		
			fprintf(stderr,"*** ERROR IMPORTING DEFAULT FILTER LIST ***\n");
		}

		rc = 0;
	}

	if (!NLM_noUserInterface)
	{

	if ((!rc) && (MFC_ControlPassword[0] != 0))
	{
		NUTCHAR readPass = (_MF_NUTCHAR)malloc(MAILFILTER_CONFIGURATION_LENGTH);
		readPass[0] = 0;
		NWSEditString( 0, 0, 1, 50, 0 , EDIT_UTIL_GETCONTROLPASSWORD, readPass, 50, EF_MASK|EF_NOSPACES|EF_SET|EF_NOCONFIRM_EXIT, MF_NutInfo, NULL, NULL, (_MF_NUTCHAR)"a..z0..9A..Z" );
		
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
				(_MF_NUTCHAR)"Incorrect Password Specified!",
				MF_NutInfo
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
				fprintf(stderr,"MFCONFIG: Either the Configuration File is not existant, corrupt or too old.\nMFCONFIG: Aborting Upgrade.\n");
			} else {
			//**** CONFIG FILE RECOVERY *
			NWSDisplayErrorText ( ERROR_CONFIGURATION_INVALID , NUT_SEVERITY_FATAL , MF_NutInfo , "" );
		
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
							(_MF_NUTCHAR)"MFConfig found a Configuration File Backup that could be valid.\n\nDo you want to try the Backup?\n",
							MF_NutInfo
							);
							
						switch (rc)
						{
						
						case (0xFFFFFFFF):
							NWSTrace(MF_NutInfo,(_MF_NUTCHAR)"Error occoured.");
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
							NWSTrace(MF_NutInfo,(_MF_NUTCHAR)"*#* CRASH *#* -- Memory Corruption?");
							break;
							
						}
					
				}
			}
			}
		}

		if (rc)
		{
			fprintf(stderr,"MFCONFIG: Error Code %d\n",rc);
			return FALSE;
		}
	}
*/
	return TRUE;
	
}


/****************************************************************************
** Main menu action procedure.
*/
static int NLM_VerifyProgramExit(void)
	{
	int cCode;
	
	cCode=NWSConfirm(
		/*	I-	header				*/	MSG_EXIT_NOSAVE,
		/*	I-	centerLine			*/	0,
		/*	I-	centerColumn		*/	0,
		/*	I-	defaultChoice		*/	0,
		/*	I-	actionProcedure	*/	NULL,
		/*	I-	handle				*/	MF_NutInfo,
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
static int NLM_VerifySaveExit(void)
	{
	int cCode;
	
	cCode=NWSConfirm(
		/*	I-	header				*/	MSG_SAVE_CHANGES,
		/*	I-	centerLine			*/	0,
		/*	I-	centerColumn		*/	0,
		/*	I-	defaultChoice		*/	1,
		/*	I-	actionProcedure	*/	NULL,
		/*	I-	handle				*/	MF_NutInfo,
		/*	I-	actionParameter	*/	NULL
		);
	
	/*------------------------------------------------------------------------
	**	Escape(-1) means No(0).
	*/
	if(cCode == (-1))
		cCode = 0;

	return cCode;
	}


static void MFConfig_Util_ImportListFromFile(void* nutHandle)
{
	FILE* fImp;
	char szTemp[MAX_BUF+1];
	int rc;
	//int curItem = -1;
	//int* cI;
	char defaultNotify;
	int overrideNotify = false;
	
	nutHandle = nutHandle;	/* rid compiler warning */
	
	defaultNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
	if (MF_GlobalConfiguration.DefaultNotification_InternalRecipient)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
	if (MF_GlobalConfiguration.DefaultNotification_InternalSender)		defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
	if (MF_GlobalConfiguration.DefaultNotification_ExternalRecipient)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;
	if (MF_GlobalConfiguration.DefaultNotification_ExternalSender)		defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING;
	
	szTemp[0] = 0;
	
	rc = NWSEditString ( 0,
					0,
					1,	/* height */
					50, /* width */
					EDIT_UTIL_IMPORTFILE_GET, /* header */
					NO_MESSAGE, /* prompt */
					(_MF_NUTCHAR)szTemp, /* buffer */
					49, /* maxlen */
					EF_ANY, /* type */
					MF_NutInfo,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((rc & E_SELECT) && (rc & E_CHANGE))
	{
		if (access(szTemp,F_OK) != 0)
		{
			NWSDisplayErrorText ( ERROR_FILE_NOT_FOUND , NUT_SEVERITY_FATAL , MF_NutInfo , szTemp );
			
		} else {
			//
			// ask if we should import the notification settings too
			//
			rc = NWSConfirm (
				      EDIT_UTIL_IMPORTFILE_ASKNOTIFICATION,
				      10,
				      12,
				      1,	// default = yes
				      NULL,	//int       (*action) (         int    option,          void  *parameter),
				      MF_NutInfo,
				      NULL	//       void     * actionParameter
				      );
			if (rc == -1)
				return;
			if (rc == 0)
				overrideNotify = true;
			if (rc == 1)
				overrideNotify = false;
		
			//
			// ask if we should zap out the old filter list
			// before importing the new one
			// 
			rc = NWSConfirm (
				      EDIT_UTIL_IMPORTFILE_CLEARLIST,
				      10,
				      12,
				      0,	// default = no
				      NULL,	//int       (*action) (         int    option,          void  *parameter),
				      MF_NutInfo,
				      NULL	//       void     * actionParameter
				      );
			
			if (rc == -1)
				return;
			if (rc == 1)
			{
				MF_GlobalConfiguration.filterList.clear();
			}
			
			//
			// let's go!
			//
			
			NWSStartWait(0,0,MF_NutInfo);
		
			fImp = fopen(szTemp,"rb");
			fseek(fImp, 54, SEEK_SET);
			
			fgetc(fImp);
			while (!feof(fImp))
			{
				fseek(fImp,-1,SEEK_CUR);
				
				MailFilter_Configuration::Filter *flt = new MailFilter_Configuration::Filter();
				
				flt->matchfield = (MailFilter_Configuration::FilterField)(fgetc(fImp));
				flt->notify = (MailFilter_Configuration::Notification)(fgetc(fImp));
				flt->type = (MailFilter_Configuration::FilterType)(fgetc(fImp));
				flt->action = (MailFilter_Configuration::FilterAction)(fgetc(fImp));
				flt->enabled = (bool)(fgetc(fImp));
				flt->enabledIncoming = (bool)(fgetc(fImp));
				flt->enabledOutgoing = (bool)(fgetc(fImp));
				
				if (overrideNotify == true)
					flt->notify = (MailFilter_Configuration::Notification)defaultNotify;

				rc = (long)fread(szTemp,sizeof(char),1000,fImp);
				fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
				
				if (strlen(szTemp) > 0) flt->expression = szTemp;

				rc = (long)fread(szTemp,sizeof(char),1000,fImp);
				fseek(fImp,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

				if (strlen(szTemp) > 0) flt->name = szTemp;
				
				if (flt->expression == "")
				{
					break;
				}
				
				MF_GlobalConfiguration.filterList.push_back(*flt);
				
				fgetc(fImp);
			}
			
			fclose(fImp);
			
			NWSEndWait(MF_NutInfo);
			
			MFConfig_UpdateFilterList();
			NWSUngetKey(UGK_SPECIAL_KEY,UGK_REFRESH_KEY,MF_NutInfo);
		}
		
	
	} else return;
				
}

static void MFConfig_Util_ExportListToFile(void* nutHandle)
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
					(_MF_NUTCHAR)szTemp, /* buffer */
					49, /* maxlen */
					EF_ANY, /* type */
					MF_NutInfo,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((rc & E_SELECT) && (rc & E_CHANGE))
	{
		if (access(szTemp,F_OK) == 0)
		{
			NWSDisplayErrorText ( ERROR_FILE_EXISTS , NUT_SEVERITY_FATAL , MF_NutInfo , szTemp );
			
		} else {
		
			MAILFILTER_CONFIGURATION_FILTERLISTTYPE::const_iterator first = MF_GlobalConfiguration.filterList.begin();
			MAILFILTER_CONFIGURATION_FILTERLISTTYPE::const_iterator last = MF_GlobalConfiguration.filterList.end();
		
			NWSStartWait(0,0,MF_NutInfo);

			fLst = fopen(szTemp,"wb");


			fprintf(fLst,"MAILFILTER_FILTER_EXCHANGEFILE");
			fputc(0,fLst);
			fprintf(fLst,MAILFILTER_CONFIGURATION_SIGNATURE);
			fputc(0,fLst);
			fputc(0,fLst);
			fputc(0,fLst);
			fputc(0,fLst);
			

			//for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
			for (; first != last; ++first)
			{
				fputc((*first).matchfield,fLst);
				fputc((*first).notify,fLst);
				fputc((*first).type,fLst);
				fputc((*first).action,fLst);
				fputc((*first).enabled,fLst);
				fputc((*first).enabledIncoming,fLst);
				fputc((*first).enabledOutgoing,fLst);
			
				fprintf(fLst,"%s",(*first).expression.c_str());
				fputc(0,fLst);
				fprintf(fLst,"%s",(*first).name.c_str());
				fputc(0,fLst);

			}
				
		
			fclose(fLst);
			
			NWSEndWait(MF_NutInfo);
			
		}
		
	
	} else return;
				
}

/****************************************************************************
** Edit filter detail dialog
*/
static	FIELD		*MFConfig_EditFilterDialog_fieldDescription;

static int MFConfig_EditFilterDialog_MenuAction(int option, void *parameter)   {      parameter = parameter;	return option;   }
static int MFConfig_EditFilterDialog_MenuActionAction(int option, void *parameter)   
{
#pragma unused(parameter)

	if(option==MAILFILTER_MATCHACTION_COPY)
		MFConfig_EditFilterDialog_fieldDescription->fieldData = (unsigned char*)MF_NMsg(EDIT_FILTERS_DESTINATION);
		else
		MFConfig_EditFilterDialog_fieldDescription->fieldData = (unsigned char*)MF_NMsg(EDIT_FILTERS_DESCRIPTION); 
		
	NWSSetFormRepaintFlag(TRUE,MF_NutInfo); return option; 
}
int MFConfig_EditFilterDialog(MailFilter_Configuration::Filter *flt)
{

	regex_t re;
	MFCONTROL	*ctlMatchfield;
	MFCONTROL	*ctlMatchaction;
	MFCONTROL	*ctlMatchtype;
   	unsigned long	line;
	int	formSaved;
	
	BOOL	newEnabledIncoming = (BOOL)flt->enabledIncoming;

	BOOL	newEnabledOutgoing = (BOOL)flt->enabledOutgoing;
	char	newDescription[60];
	char	newExpression[500];
	int		newMatchtype = flt->type;
	int		newMatchfield = flt->matchfield;
	int		newMatchaction = flt->action;
	BOOL	newNotifyAdminIncoming = (BOOL)chkFlag(flt->notify,MAILFILTER_NOTIFICATION_ADMIN_INCOMING);
	BOOL	newNotifyAdminOutgoing = (BOOL)chkFlag(flt->notify,MAILFILTER_NOTIFICATION_ADMIN_OUTGOING);
	BOOL	newNotifySenderIncoming = (BOOL)chkFlag(flt->notify,MAILFILTER_NOTIFICATION_SENDER_INCOMING);
	BOOL	newNotifySenderOutgoing = (BOOL)chkFlag(flt->notify,MAILFILTER_NOTIFICATION_SENDER_OUTGOING);
	BOOL	newNotifyRecipientIncoming = (BOOL)chkFlag(flt->notify,MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING);
	BOOL	newNotifyRecipientOutgoing = (BOOL)chkFlag(flt->notify,MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING);

	NWSPushList(MF_NutInfo);
	NWSInitForm(MF_NutInfo);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 0;
	
//	NWSAppendStringField (line, 20, 60, NORMAL_FIELD, (_MF_NUTCHAR)newDescription, (_MF_NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", NULL, MF_NutInfo);
//	line++;
	
//	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHFIELD), MF_NutInfo);
	ctlMatchfield = NWSInitMenuField (MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuAction, 		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ALWAYS,		MAILFILTER_MATCHFIELD_ALWAYS, 			MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ATTACHMENT,	MAILFILTER_MATCHFIELD_ATTACHMENT, 		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL,			MAILFILTER_MATCHFIELD_EMAIL, 			MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_BOTHANDCC,MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC,	MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_FROM,	MAILFILTER_MATCHFIELD_EMAIL_FROM,		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_TO,		MAILFILTER_MATCHFIELD_EMAIL_TO, 		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_TOANDCC,	MAILFILTER_MATCHFIELD_EMAIL_TOANDCC,	MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_SUBJECT,		MAILFILTER_MATCHFIELD_SUBJECT, 			MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_SIZE,			MAILFILTER_MATCHFIELD_SIZE, 			MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_BLACKLIST,		MAILFILTER_MATCHFIELD_BLACKLIST, 		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_IPUNRESOLVABLE,MAILFILTER_MATCHFIELD_IPUNRESOLVABLE, 	MF_NutInfo);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchfield, ctlMatchfield, NULL, MF_NutInfo);   
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHACTION), MF_NutInfo);
	ctlMatchaction = NWSInitMenuField (MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuActionAction, 			MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_DROP_MAIL,	MAILFILTER_MATCHACTION_DROP_MAIL, 		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_PASS,		MAILFILTER_MATCHACTION_PASS,		 	MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_SCHEDULE,	MAILFILTER_MATCHACTION_SCHEDULE,		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_NOSCHEDULE,	MAILFILTER_MATCHACTION_NOSCHEDULE,		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_COPY,		MAILFILTER_MATCHACTION_COPY,			MF_NutInfo);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchaction, ctlMatchaction, NULL, MF_NutInfo);   
	line++;


	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHTYPE), MF_NutInfo);
	ctlMatchtype = NWSInitMenuField (MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuAction, 			MF_NutInfo);
	NWSAppendToMenuField (ctlMatchtype, EDIT_FILTERS_MATCHTYPE_MATCH,			MAILFILTER_MATCHTYPE_MATCH, 		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchtype, EDIT_FILTERS_MATCHTYPE_NOMATCH,			MAILFILTER_MATCHTYPE_NOMATCH,		MF_NutInfo);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchtype, ctlMatchtype, NULL, MF_NutInfo);   
	line++;
	
	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_EXPRESSION), MF_NutInfo);
	strcpy (newExpression, flt->expression.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, REQUIRED_FIELD, (_MF_NUTCHAR)newExpression, 500, (_MF_NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", EF_ANY, NULL, MF_NutInfo); 
	line++;

	MFConfig_EditFilterDialog_fieldDescription = NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_DESCRIPTION), MF_NutInfo);
	strcpy (newDescription, flt->name.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newDescription, 60, (_MF_NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", EF_ANY, NULL, MF_NutInfo); 
	
	line++;
	
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_ENABLEDINCOMING), MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newEnabledIncoming, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_ENABLEDOUTGOING), MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newEnabledOutgoing, NULL, MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_NOTIFICATION_IN), MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)">> Postmaster:", MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyAdminIncoming, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)">> Sender:", MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifySenderIncoming, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)">> Recipient:", MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyRecipientIncoming, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_NOTIFICATION_OUT), MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)">> Postmaster:", MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyAdminOutgoing, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)">> Sender:", MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifySenderOutgoing, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)">> Recipient:", MF_NutInfo);
	NWSAppendBoolField (line, 30, NORMAL_FIELD, &newNotifyRecipientOutgoing, NULL, MF_NutInfo);
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
		MF_NutInfo
	);

	/*------------------------------------------------------------------------
	** This function returns TRUE if the form was saved, FALSE if not.
	** If the form was not saved you must restore all variables to their
	** original values manually
	*/

	if (formSaved)
	{
		flt->enabled = 1;
		flt->enabledIncoming = (bool)newEnabledIncoming;
		flt->enabledOutgoing = (bool)newEnabledOutgoing;

		flt->matchfield = (MailFilter_Configuration::FilterField)newMatchfield;
		flt->type = (MailFilter_Configuration::FilterType)newMatchtype;
		flt->action = (MailFilter_Configuration::FilterAction)newMatchaction;

		flt->expression = newExpression;
		flt->name = newDescription;
		

		flt->notify = (MailFilter_Configuration::Notification)MAILFILTER_NOTIFICATION_NONE;
		if (newNotifyAdminIncoming)			flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_ADMIN_INCOMING);
		if (newNotifyAdminOutgoing)			flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING);
		if (newNotifySenderIncoming)		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_SENDER_INCOMING);
		if (newNotifySenderOutgoing)		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_SENDER_OUTGOING);
		if (newNotifyRecipientIncoming)		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING);
		if (newNotifyRecipientOutgoing)		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING);
		

		if (
			(flt->action == MAILFILTER_MATCHACTION_SCHEDULE)
			||
			(flt->action == MAILFILTER_MATCHACTION_NOSCHEDULE)
		)
		{
			flt->notify = (MailFilter_Configuration::Notification)MAILFILTER_NOTIFICATION_NONE;
			flt->enabledIncoming = 0;
		}
	}

	if ( (newMatchfield != MAILFILTER_MATCHFIELD_SIZE) || (newMatchfield != MAILFILTER_MATCHFIELD_BLACKLIST) || (newMatchfield != MAILFILTER_MATCHFIELD_IPUNRESOLVABLE) )
	{
		MF_RegError(regcomp(&re,flt->expression.c_str(),0),&re);
		regfree(&re);
	}

	NWSDestroyForm (MF_NutInfo);
	NWSPopList(MF_NutInfo);

	if (formSaved)
		return -2;

	return -1;

}

/****************************************************************************
** Edit filter list action
*/

static int MFConfig_EditFilters_Act(LONG keyPressed, LIST **elementSelected,
		LONG *itemLineNumber, void *actionParameter)
	{
	MailFilter_Configuration::Filter myItem;
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
			
			myItem.enabled = true;
			myItem.enabledIncoming = true;
			myItem.enabledOutgoing = true;
			myItem.action = (MailFilter_Configuration::FilterAction)MAILFILTER_MATCHACTION_DROP_MAIL;

			myItem.notify = (MailFilter_Configuration::Notification)(MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING);
			if (MF_GlobalConfiguration.DefaultNotification_InternalRecipient)	myItem.notify = (MailFilter_Configuration::Notification)(myItem.notify | MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING);
			if (MF_GlobalConfiguration.DefaultNotification_InternalSender)		myItem.notify = (MailFilter_Configuration::Notification)(myItem.notify | MAILFILTER_NOTIFICATION_SENDER_OUTGOING);
			if (MF_GlobalConfiguration.DefaultNotification_ExternalRecipient)	myItem.notify = (MailFilter_Configuration::Notification)(myItem.notify | MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING);
			if (MF_GlobalConfiguration.DefaultNotification_ExternalSender)		myItem.notify = (MailFilter_Configuration::Notification)(myItem.notify | MAILFILTER_NOTIFICATION_SENDER_INCOMING);
			
			
			rc = MFConfig_EditFilterDialog(&myItem);
			MF_GlobalConfiguration.filterList.push_back(myItem);
//			MF_Filter_Sort();
			return rc;

		case M_DELETE:
			int ci = *(int *)((*elementSelected)->otherInfo);
			MFD_Out(MFD_SOURCE_GENERIC,"debug: %d\n",ci);

			MAILFILTER_CONFIGURATION_FILTERLISTTYPE::iterator first = MF_GlobalConfiguration.filterList.begin();
			first += ci;

			MF_GlobalConfiguration.filterList.erase(first);

			return -2;


		case M_SELECT:
			rc = MFConfig_EditFilterDialog(&MF_GlobalConfiguration.filterList[*(unsigned int *)((*elementSelected)->otherInfo)]);
			return rc;
		}



	return(-1);	
	}


/****************************************************************************
** Edit filters.
*/
static void MFConfig_EditFilters()
	{
	long						rc = 0;

	/*------------------------------------------------------------------------
	**	At this point, the current list is the Main Menu.  If we begin adding
	**	new items to the current list, it would mess up the Main menu (to say
	**	the least).  So, we will save the Main Menu List on the List stack
	** (PushList) and then initialize a new list (set head and tail to NULL)
	**	by calling InitList().  Note that Lists use NWInitList() and Menus use
	**	NWInitMenu().
	*/
	NWSPushList(MF_NutInfo);
	NWSInitList(MF_NutInfo, free);

	// init function keys ...
	NWSEnableInterruptKey	(	K_F8,	MFConfig_Util_ImportListFromFile,	MF_NutInfo	);
	NWSEnableInterruptKey	(	K_F9,	MFConfig_Util_ExportListToFile,	MF_NutInfo	);
	NWSEnableInterruptKey   (	K_F7,	MFConfig_Util_Sort, MF_NutInfo );
	
	/*------------------------------------------------------------------------
	*/
	
	MAILFILTER_CONFIGURATION_FILTERLISTTYPE::const_iterator first = MF_GlobalConfiguration.filterList.begin();
	MAILFILTER_CONFIGURATION_FILTERLISTTYPE::const_iterator last = MF_GlobalConfiguration.filterList.end();

	int curItem = 0;
	for (; first != last; ++first)
	{
		MFConfig_AddFilterItem(*first,curItem);
		++curItem;
	}

	/*------------------------------------------------------------------------
	**	Display the list and allow user interaction.
	*/
	MFC_CurrentList = 1;
	rc = (long) NWSList(
		/* I-	header			*/	MENU_MAIN_EDIT_FILTERS,
		/*	I-	centerLine		*/	0,
		/*	I-	centerColumn	*/	0,
		/*	I-	height			*/	17,
		/* I-	width			*/  80,
		/* I-	validKeyFlags	*/	M_ESCAPE|M_SELECT|M_INSERT|M_DELETE|M_NO_SORT|M_REFRESH,
		/*	IO	element			*/	0,
		/*	I-	handle			*/  MF_NutInfo,
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
	NWSDestroyList(MF_NutInfo);
	NWSPopList(MF_NutInfo);

	NWSDisableInterruptKey(K_F8,MF_NutInfo);
	NWSDisableInterruptKey(K_F9,MF_NutInfo);

	if (rc == -2)
		MFConfig_EditFilters();

	return;
	}

static void MFConfig_Util_Sort(void* nutHandle)
{
#pragma unused(nutHandle)
	sort(MF_GlobalConfiguration.filterList.begin(),MF_GlobalConfiguration.filterList.end(),CompareFilters());
	MFConfig_UpdateFilterList();
	
	NWSUngetKey(UGK_SPECIAL_KEY,UGK_REFRESH_KEY,MF_NutInfo);
}



/****************************************************************************
** MFConfig: Edit License Key
*/
static void MFConfig_EditLicense()
	{
	int	formSaved;
	char	newLicenseKey[MAILFILTER_CONFIGURATION_LENGTH];


	newLicenseKey[0]=0;

	if (MF_GlobalConfiguration.LicenseKey != "")
	{
		newLicenseKey[0]=( char)MF_GlobalConfiguration.LicenseKey.c_str()[0];
		newLicenseKey[1]=( char)MF_GlobalConfiguration.LicenseKey.c_str()[1];
		newLicenseKey[2]='.';
		newLicenseKey[3]='.';
		newLicenseKey[4]='.';
		newLicenseKey[5]=0;
	}
	
//	strcpy (newLicenseKey, MFC_License_Key);

	NWSPushList(MF_NutInfo);
#ifdef _LIC_PORTAL
	NWSInitForm(MF_NutInfo);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 1;
	NWSAppendCommentField (line, 2, (_MF_NUTCHAR)programMesgTable[MENU_MAIN_LICENSEKEY], MF_NutInfo);
	NWSAppendStringField (line+1, 5, MAILFILTER_CONFIGURATION_LENGTH, NORMAL_FIELD, newLicenseKey, (_MF_NUTCHAR)"A..Za..z0..9 @_-+.[]()\"\\#'*?!§$%&/=<>|", NULL, MF_NutInfo);

	formSaved = NWSEditPortalForm (
		/* I- header		*/ 0,
		/* I- center line	*/ 0,
		/* I- center col	*/ 40,
		/* I- form height	*/ line+3,
		/* I- form width	*/ 55,
		/* I- ctl flags		*/ F_VERIFY,
		/* I- form help		*/ F_NO_HELP,
		/* I- confirm msg	*/ MSG_SAVE_CHANGES,
		/* I- handle		*/ MF_NutInfo
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
	NWSDestroyForm (MF_NutInfo);
#else
	
	formSaved = NWSEditString ( 0,
					0,
					1,	/* height */
					60, /* width */
					MENU_MAIN_LICENSEKEY, /* header */
					NO_MESSAGE, /* prompt */
					(_MF_NUTCHAR)newLicenseKey, /* buffer */
					110, /* maxlen */
					EF_ANY, /* type */
					MF_NutInfo,
					NULL, /* insertProc */
					NULL, /* actionProc */
					NULL);

	if ((formSaved & E_SELECT) && (formSaved & E_CHANGE))
		formSaved = (-2);
		else
		formSaved = (-1);
	
	if (formSaved == -2)
	{
		MF_GlobalConfiguration.LicenseKey = newLicenseKey;
	}
	
#endif

	NWSPopList(MF_NutInfo);

	return;
	}
	
/****************************************************************************
** MFConfig: Edit Config Settings
*/
static void MFConfig_EditConfig()
	{
	unsigned long	line;
	int		formSaved;
	char	newDomainName[MAILFILTER_CONFIGURATION_LENGTH];
	char	newDomainHostname[MAILFILTER_CONFIGURATION_LENGTH];
	char	newEmailPostMaster[MAILFILTER_CONFIGURATION_LENGTH];
	char	newEmailMailFilter[MAILFILTER_CONFIGURATION_LENGTH];
	char	newScheduleTime[MAILFILTER_CONFIGURATION_LENGTH];
	char	newEmailOffice[50];
	BOOL	newNotification_InternalRcpt = (BOOL)MF_GlobalConfiguration.DefaultNotification_InternalRecipient;
	BOOL	newNotification_InternalSndr = (BOOL)MF_GlobalConfiguration.DefaultNotification_InternalSender;
	BOOL	newNotification_ExternalRcpt = (BOOL)MF_GlobalConfiguration.DefaultNotification_ExternalRecipient;
	BOOL	newNotification_ExternalSndr = (BOOL)MF_GlobalConfiguration.DefaultNotification_ExternalSender;
	BOOL	newNotification_AdminLogs = (BOOL)MF_GlobalConfiguration.NotificationAdminLogs;
	BOOL	newNotification_AdminMailsKilled = (BOOL)MF_GlobalConfiguration.NotificationAdminMailsKilled;
	BOOL	newNotification_AdminDailyReport = (BOOL)MF_GlobalConfiguration.NotificationAdminDailyReport;
	LONG	newProblemDir_MaxSize = (LONG)MF_GlobalConfiguration.ProblemDirMaxSize;
	LONG	newProblemDir_MaxAge = (LONG)MF_GlobalConfiguration.ProblemDirMaxAge;
	char	newPathGwia[MAILFILTER_CONFIGURATION_LENGTH];
	char	newPathMflt[MAILFILTER_CONFIGURATION_LENGTH];
//	char	newDNSBLZone[MAILFILTER_CONFIGURATION_LENGTH];
	LONG	newScanDirNum = 0;
	LONG	newScanDirWait = 0;
	LONG	newGwiaVersion = 0;
	char	newControlPassword[MAILFILTER_CONFIGURATION_LENGTH];
	BOOL	newEnableIncomingRcptCheck = (BOOL)MF_GlobalConfiguration.EnableIncomingRcptCheck;
  	BOOL	newEnableAttachmentDecoder = (BOOL)MF_GlobalConfiguration.EnableAttachmentDecoder;
  	BOOL	newEnablePFA = (BOOL)MF_GlobalConfiguration.EnablePFAFunctionality;
  	BOOL	newEnableNRMThread = (BOOL)MF_GlobalConfiguration.EnableNRMThread;
  	BOOL	newEnableNRMRestore = (BOOL)MF_GlobalConfiguration.EnableNRMRestore;

	NWSPushList(MF_NutInfo);
	NWSInitForm(MF_NutInfo);

	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 0;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_GENERAL_PATH_GWIA], MF_NutInfo);
	strcpy (newPathGwia, MF_GlobalConfiguration.GWIARoot.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newPathGwia, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"A..Za..z\\/{}[]()=#-_.,:;!§$&+~0..9", EF_SET, NULL, MF_NutInfo); 
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_GENERAL_PATH_MFLT], MF_NutInfo);
	strcpy (newPathMflt, MF_GlobalConfiguration.MFLTRoot.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newPathMflt, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"A..Za..z\\/{}[]()=#-_.,:;!§$&+~0..9", EF_SET, NULL, MF_NutInfo); 
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_DOMAIN_DOMAIN_NAME], MF_NutInfo);
	strcpy (newDomainName, MF_GlobalConfiguration.DomainName.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newDomainName, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"A..Za..z_-,.0..9", EF_SET, NULL, MF_NutInfo); 

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_DOMAIN_DOMAIN_HOSTNAME], MF_NutInfo);
	strcpy (newDomainHostname, MF_GlobalConfiguration.DomainHostname.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newDomainHostname, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"A..Za..z_-.0..9", EF_SET, NULL, MF_NutInfo); 
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_DOMAIN_POSTMASTER], MF_NutInfo);
	strcpy (newEmailPostMaster, MF_GlobalConfiguration.DomainEmailPostmaster.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newEmailPostMaster, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"A..Za..z@_-+!<>.0..9", EF_SET, NULL, MF_NutInfo); 
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_DOMAIN_MAILFILTER], MF_NutInfo);
	strcpy (newEmailMailFilter, MF_GlobalConfiguration.DomainEmailMailFilter.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newEmailMailFilter, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"A..Za..z@_-+!<>.0..9", EF_SET, NULL, MF_NutInfo); 
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_BWL_SCHEDULETIME], MF_NutInfo);
	strcpy (newScheduleTime, MF_GlobalConfiguration.BWLScheduleTime.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newScheduleTime, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"0..9-:,", EF_SET, NULL, MF_NutInfo); 
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"General E-Mail Address w/o Domain", MF_NutInfo);
	NWSAppendCommentField (line, 61, (_MF_NUTCHAR)"(blank=disabled)", MF_NutInfo);
	strcpy (newEmailOffice, MF_GlobalConfiguration.Multi2One.c_str());
	NWSAppendStringField (line+1, 3, 50, NORMAL_FIELD, (_MF_NUTCHAR)newEmailOffice, (_MF_NUTCHAR)"A..Za..z_-+!.0..9", EF_SET, MF_NutInfo);
	line += 2;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"GWIA Mode/Version: (see manual)", MF_NutInfo);
	newGwiaVersion = (unsigned long)MF_GlobalConfiguration.GWIAVersion;
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newGwiaVersion, 550, 600, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"MailFilter Control/Config Password:", MF_NutInfo);
	strcpy (newControlPassword, MF_GlobalConfiguration.ControlPassword.c_str());
	NWSAppendPasswordField(line, 40, 20, NORMAL_FIELD, (_MF_NUTCHAR)newControlPassword, 50, NULL, true, NULL, '*', MF_NutInfo); 
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable PFA Functionality:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnablePFA, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable Incoming Rcpt Rule Check:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnableIncomingRcptCheck, NULL, MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable NRM on load (OS address space only):", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnableNRMThread, NULL, MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable Mail Restore in NRM:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnableNRMRestore, NULL, MF_NutInfo);
	line++;
	
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Virus Scan Integration:", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Number of MFSCAN Directories:", MF_NutInfo);
	newScanDirNum = (unsigned long)MF_GlobalConfiguration.MailscanDirNum;
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newScanDirNum, 0, 50, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Seconds to wait for Real Time Scan:", MF_NutInfo);
	newScanDirWait = (unsigned long)MF_GlobalConfiguration.MailscanTimeout;
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newScanDirWait, 0, 600, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Decode Attachments for VScan:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newEnableAttachmentDecoder, NULL, MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Default Notify On Filter Match:", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Internal Recipient:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_InternalRcpt, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Internal Sender:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_InternalSndr, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"External Recipient:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_ExternalRcpt, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"External Sender:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_ExternalSndr, NULL, MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Notify Admin On Log Cycle Error:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_AdminLogs, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Send Admin Daily Status Report:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_AdminDailyReport, NULL, MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Problem Directory Cleanup:", MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Maximum Total Size (kBytes):", MF_NutInfo);
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newProblemDir_MaxSize, 0, 1000000, NULL, MF_NutInfo);
	NWSAppendCommentField (line, 65, (_MF_NUTCHAR)"(0=disabled)", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Maximum File Age (Days):", MF_NutInfo);
	NWSAppendUnsignedIntegerField (line, 40, NORMAL_FIELD, &newProblemDir_MaxAge, 0, 10000, NULL, MF_NutInfo);
	NWSAppendCommentField (line, 65, (_MF_NUTCHAR)"(0=disabled)", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Notify Admin On Cleanup:", MF_NutInfo);
	NWSAppendBoolField (line, 40, NORMAL_FIELD, &newNotification_AdminMailsKilled, NULL, MF_NutInfo);
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
		MF_NutInfo
	);

	/*------------------------------------------------------------------------
	** This function returns TRUE if the form was saved, FALSE if not.
	** If the form was not saved you must restore all variables to their
	** original values manually
	*/
	if (formSaved)
	{
		MF_GlobalConfiguration.GWIARoot					= newPathGwia;
		MF_GlobalConfiguration.MFLTRoot					= newPathMflt;
		MF_GlobalConfiguration.DomainName				= newDomainName;
		MF_GlobalConfiguration.DomainHostname			= newDomainHostname;
		MF_GlobalConfiguration.DomainEmailPostmaster	= newEmailPostMaster;
		MF_GlobalConfiguration.DomainEmailMailFilter	= newEmailMailFilter;
		MF_GlobalConfiguration.BWLScheduleTime			= newScheduleTime;
		MF_GlobalConfiguration.Multi2One				= newEmailOffice;

		// check if pass has changed, if yes encrypt ...
		if (strcmp(MF_GlobalConfiguration.ControlPassword.c_str(),newControlPassword) != 0)
			MFU_strxor((char*)newControlPassword,(char)18);
			
		MF_GlobalConfiguration.ControlPassword = 			newControlPassword;
		
		MF_GlobalConfiguration.DefaultNotification_InternalRecipient =(MailFilter_Configuration::Notification)newNotification_InternalRcpt;
		MF_GlobalConfiguration.DefaultNotification_InternalSender =(MailFilter_Configuration::Notification)newNotification_InternalSndr;
		
		MF_GlobalConfiguration.DefaultNotification_ExternalRecipient =(MailFilter_Configuration::Notification)newNotification_ExternalRcpt;
		MF_GlobalConfiguration.DefaultNotification_ExternalSender =(MailFilter_Configuration::Notification)newNotification_ExternalSndr;

		MF_GlobalConfiguration.NotificationAdminLogs 		=(bool)newNotification_AdminLogs;
		MF_GlobalConfiguration.NotificationAdminDailyReport	=(bool)newNotification_AdminDailyReport;

		MF_GlobalConfiguration.NotificationAdminMailsKilled	=(bool)newNotification_AdminMailsKilled;
		MF_GlobalConfiguration.ProblemDirMaxSize  			=(unsigned int)newProblemDir_MaxSize;
		MF_GlobalConfiguration.ProblemDirMaxAge 			=(unsigned int)newProblemDir_MaxAge;

		MF_GlobalConfiguration.MailscanTimeout				=(unsigned int)newScanDirWait;
		MF_GlobalConfiguration.MailscanDirNum				=(unsigned int)newScanDirNum;
		MF_GlobalConfiguration.EnableAttachmentDecoder		=(bool)newEnableAttachmentDecoder;
		MF_GlobalConfiguration.EnablePFAFunctionality		=(bool)newEnablePFA;
		MF_GlobalConfiguration.EnableNRMThread				=(bool)newEnableNRMThread;
		MF_GlobalConfiguration.EnableNRMRestore				=(bool)newEnableNRMRestore;

		MF_GlobalConfiguration.GWIAVersion					=(unsigned int)newGwiaVersion;
		
		switch (MF_GlobalConfiguration.GWIAVersion)
		{	
			case 550:
			case 600:
				break;
			default:
				MF_GlobalConfiguration.GWIAVersion = 550;
				break;
		}

		MF_GlobalConfiguration.EnableIncomingRcptCheck		=(bool)newEnableIncomingRcptCheck;
//		MFC_DropUnresolvableRelayHosts		=(int)newDropUnresolvableRelayHosts;
		
	}

	NWSDestroyForm (MF_NutInfo);
	NWSPopList(MF_NutInfo);

	return;
	}
	

/****************************************************************************
** Main menu action procedure.
*/
static int NLM_MenuMainAct(int index, void *parm)
	{
	parm=parm;	 /* Rid compiler warning. */

	NWSDisableInterruptKey	(	K_F8,	MF_NutInfo	);
	MFConfig_UI_ShowKeys(MFCONFIG_KEYS_NONE);
	
	if (MFT_NLM_Exiting)
		return 0;

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
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_IMPORT|MFCONFIG_KEYS_EXPORT|MFCONFIG_KEYS_NEW|MFCONFIG_KEYS_DELETE|MFCONFIG_KEYS_SELECT|MFCONFIG_KEYS_SORT);
			MFConfig_EditFilters();
			break;

		case MENU_MAIN_LICENSEKEY:
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_CANCEL);
			MFConfig_EditLicense();
			break;

		case MENU_MAIN_SAVE_AND_EXIT:
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_CANCEL);
			if (NLM_VerifySaveExit())
			{
				MF_GlobalConfiguration.WriteToFile("");
				return(0);
			}
			break;
			
		case MENU_MAIN_SAVE_AND_RESTART:
			MFConfig_UI_ShowKeys(MFCONFIG_KEYS_CANCEL);
			MF_GlobalConfiguration.WriteToFile("");
			return(0);
			break;	// never reached
		}

	/*------------------------------------------------------------------------
	**	If we should be exiting, pretend that ESCAPE was pressed.
	*/
	if (MFT_NLM_Exiting)
		return 0;


	NWSEnableInterruptKey	(	K_F8,	MFConfig_ImportLicenseKey,	MF_NutInfo	);
	MFConfig_UI_ShowKeys(MFCONFIG_KEYS_IMPORT);

	return(-1);
	}

/****************************************************************************
** Main menu.
*/
static void NLM_MenuMain(bool bStandalone)
	{
	LIST *defaultOption;

	/*------------------------------------------------------------------------
	**	At this point, the current list is uninitialized (being that it is the
	** first list of the program.)  Before using the current list it must be
	**	initialized (set head and tail to NULL) by calling InitMenu().
	**	Note that Lists use NWInitList() and Menus use NWInitMenu().
	*/
	NWSInitMenu(MF_NutInfo);

	/*------------------------------------------------------------------------
	**	Insert menu items.  Note that the insertion order does not matter being
	**	that NWSMenu() will always sort the Menu selections automatically.
	** The defaultOption stores a pointer to the menu item which we wish to be
	** highlighed by default.
	*/
	
	NWSAppendToMenu(MENU_MAIN_EDIT_CONFIGURATION, MENU_MAIN_EDIT_CONFIGURATION, MF_NutInfo);

	NWSAppendToMenu(MENU_MAIN_LICENSEKEY, MENU_MAIN_LICENSEKEY, MF_NutInfo);
	
	NWSAppendToMenu(MENU_MAIN_EDIT_FILTERS, MENU_MAIN_EDIT_FILTERS, MF_NutInfo);
	
	if (bStandalone)
		defaultOption = NWSAppendToMenu(MENU_MAIN_SAVE_AND_EXIT, MENU_MAIN_SAVE_AND_EXIT, MF_NutInfo);
	else
		defaultOption = NWSAppendToMenu(MENU_MAIN_SAVE_AND_RESTART, MENU_MAIN_SAVE_AND_RESTART, MF_NutInfo);

	NWSEnableInterruptKey	(	K_F8,	MFConfig_ImportLicenseKey,	MF_NutInfo	);
	MFConfig_UI_ShowKeys(MFCONFIG_KEYS_IMPORT|MFCONFIG_KEYS_EXIT);

	/*------------------------------------------------------------------------
	**	Display the menu and allow user interaction.
	*/
	NWSMenu(
		/*	header				*/	CONFIGAPP_PROGRAMNAME,
		/*	centerLine			*/	0,					/* [0,0] means Center of screen*/
		/*	centerColumn		*/	0,
		/*	defaultElement		*/	defaultOption,		/* Could use NULL here desired */			
		/*	actionProcedure		*/	NLM_MenuMainAct,
		/*	handle				*/	MF_NutInfo,
		/*	actionParameter		*/	NULL
		);
	
	/*------------------------------------------------------------------------
	**	Before returning, we must free the list items allocated by
	**	NWSAppendToMenu(). Note that Lists use NWDestroyList() and Menus use
	**	NWDestroyMenu().
	*/
	NWSDestroyMenu(MF_NutInfo);

	return;
	}

int MailFilter_Main_RunAppConfig(bool bStandalone)
{

	char szTemp[80+2];

	//if (bStandalone)
	if (!MF_NutInit())
		return false;

	memset(szTemp,' ',80);
	szTemp[81]=0;
	sprintf(szTemp,"  MailFilter/ax Configuration Editor NLM/CUI Version %s",
									programMesgTable[PROGRAM_VERSION]);
	szTemp[81]=0;
	NWSShowLineAttribute ( 0 , 0 , (_MF_NUTCHAR)szTemp , VNORMAL , 80 , (struct ScreenStruct*)MF_NutInfo->screenID );

	memset(szTemp,' ',80);
	szTemp[81]=0;
	sprintf(szTemp,"  Path: %s",MF_GlobalConfiguration.config_directory.c_str());
	szTemp[81]=0;
	NWSShowLineAttribute ( 1 , 0 , (_MF_NUTCHAR)szTemp , VNORMAL , 80 , (struct ScreenStruct*)MF_NutInfo->screenID );

	memset(szTemp,' ',80);
	szTemp[81]=0;
	sprintf(szTemp,"  MailFilter Version: 001.%03i              Configuration File Version: 001.%03i",
						MAILFILTER_CONFIGURATION_THISBUILD,MF_GlobalConfiguration.config_build);
	NWSShowLineAttribute ( 2 , 0 , (_MF_NUTCHAR)szTemp , VNORMAL , 80 , (struct ScreenStruct*)MF_NutInfo->screenID );

	NLM_MenuMain(bStandalone);

	if (!bStandalone)
		MF_NutDeinit();

	return 0;
}

/* eof */
