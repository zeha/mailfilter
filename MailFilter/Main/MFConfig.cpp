/*+
 +		MFConfig.cpp
 +		
 +		MailFilter Configuration
 +		
 +		Copyright 2001 Christian Hofstädtler
 +		
 +		
 +		- Aug/2001 ; ch   ; Initial Code
 +		
 +		
+*/

#define _MFD_MODULE			"MFCONFIG.CPP"
#include "MailFilter.h"

#define MAILFILTER_CONFIGURATIONFILE_STRING (MAILFILTER_CONFIGURATION_LENGTH+1)

//
//  Show Configuration
//
void MF_UI_ShowConfiguration(void)
{
	char 	szTemp[82];
#ifdef __NOVELL_LIBC__
	struct utsname u;
	uname(&u);
		
	sprintf(szTemp, "Active Configuration:");
	MF_StatusUI_UpdateLog(szTemp);

	/*------------------------------------------------------------------------
	**	Place information on portal.
	*/
	//
	sprintf(szTemp, " Host Name....: %s (%s - %d.%d SP%d)",MF_GlobalConfiguration.DomainHostname.c_str(),MF_GlobalConfiguration.ServerName.c_str(),u.netware_major,u.netware_minor,u.servicepack);
#else
	sprintf(szTemp, " Host Name....: %s (%s)",MF_GlobalConfiguration.DomainHostname.c_str(),MF_GlobalConfiguration.ServerName.c_str());
#endif
	MF_StatusUI_UpdateLog(szTemp);
	
	strcpy(szTemp, " Domains......: ");
	strncat(szTemp,MF_GlobalConfiguration.DomainName.c_str(),82-16);
	MF_StatusUI_UpdateLog(szTemp);

	strcpy(szTemp, " Prb Mail From: ");
	strncat(szTemp,MF_GlobalConfiguration.DomainEmailMailFilter.c_str(),82-16);
	MF_StatusUI_UpdateLog(szTemp);

	strcpy(szTemp, " Prb Mail Rcpt: ");
	strncat(szTemp,MF_GlobalConfiguration.DomainEmailPostmaster.c_str(),82-16);
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, " Prb Dir Clean: %d kBytes, %d Days, Notify: %s",MF_GlobalConfiguration.ProblemDirMaxSize,MF_GlobalConfiguration.ProblemDirMaxAge,MF_GlobalConfiguration.NotificationAdminMailsKilled==true?"On":"Off");
	MF_StatusUI_UpdateLog(szTemp);

/*	sprintf(szTemp, " Notification.: Internal: Sender: %s, Recipient: %s",MFC_Notification_EnableInternalSndr==1?"On":"Off",MFC_Notification_EnableInternalRcpt==1?"On":"Off"); 
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, " Notification.: External: Sender: %s, Recipient: %s",MFC_Notification_EnableExternalSndr==1?"On":"Off",MFC_Notification_EnableExternalRcpt==1?"On":"Off"); 
	MF_StatusUI_UpdateLog(szTemp);
*/
	sprintf(szTemp, " Virus Scan...: %s, Delay: %d, Decode: %s",MFC_MAILSCAN_Enabled == 0 ? "Off" : "On",MF_GlobalConfiguration.MailscanTimeout,MF_GlobalConfiguration.EnableAttachmentDecoder == 0 ? "Off" : "On");
	MF_StatusUI_UpdateLog(szTemp);

	/* this is inverse */
	sprintf(szTemp, " Scheduling...: %s, %d rule(s)", MFBW_CheckCurrentScheduleState() == false ? "On" : "Off",MF_CountFilters(MAILFILTER_MATCHACTION_SCHEDULE));
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, " Scheduling...: %s",MF_GlobalConfiguration.BWLScheduleTime.c_str());
	MF_StatusUI_UpdateLog(szTemp);

/*	if (MFC_DNSBL_Zonename[0] != 0)
	{
	sprintf(szTemp, " DNS-Blacklist: On, using %s",MFC_DNSBL_Zonename);
		MF_StatusUI_UpdateLog(szTemp);
	} else {
	sprintf(szTemp, " DNS-Blacklist: Off");
		MF_StatusUI_UpdateLog(szTemp);
	}
*/	
	sprintf(szTemp, " Directories..: %d Queues",MF_GlobalConfiguration.MailscanDirNum); 
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, "  MailFilter..: %s",MF_GlobalConfiguration.MFLTRoot.c_str());
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, "  GWIA Home...: %s",MF_GlobalConfiguration.GWIARoot.c_str()); 
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, "  Logging To..: %s",MF_GlobalConfiguration.LogDirectory.c_str()); 
	MF_StatusUI_UpdateLog(szTemp);


}

int MF_ConfigReadFile(char* FileName, char* szDestination, long iBufLen)
{
	FILE* cfgFile;
	size_t dRead;
	
	szDestination[0] = 0;
	
	cfgFile = fopen(FileName,"rb");
	if (cfgFile == NULL)
		return -2;
	
	dRead = fread(szDestination,sizeof(char),(unsigned int)(iBufLen-1),cfgFile);
	fclose(cfgFile);
	
	szDestination[dRead]=0;
	szDestination[iBufLen-1]=0;
	
	return (int)(dRead);
}

int MF_ConfigReadString(char* ConfigFile, int Entry, char Value[])
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

int MF_ConfigReadInt(char ConfigFile[MAX_PATH], int Entry)
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

/*
// Frees the configuration ...
void MF_ConfigFree(void)
{
	// Free Attachment, Subject, etc. Caches ...
	MF_Filter_FreeLists();

	if (MFT_Local_ServerName != NULL)			{	free(MFT_Local_ServerName);			MFT_Local_ServerName		= NULL;	}
	if (MFT_MF_LogDir != NULL)					{	free(MFT_MF_LogDir);				MFT_MF_LogDir				= NULL;	}

	if (MFC_GWIA_ROOT != NULL)					{	free(MFC_GWIA_ROOT);				MFC_GWIA_ROOT				= NULL;	}
	if (MFC_MFLT_ROOT != NULL)					{	free(MFC_MFLT_ROOT);				MFC_MFLT_ROOT				= NULL;	}

	if (MFC_DOMAIN_Name != NULL)				{	free(MFC_DOMAIN_Name);				MFC_DOMAIN_Name				= NULL;	}
	if (MFC_DOMAIN_Hostname != NULL)			{	free(MFC_DOMAIN_Hostname);			MFC_DOMAIN_Hostname			= NULL;	}
	if (MFC_DOMAIN_EMail_PostMaster != NULL)	{	free(MFC_DOMAIN_EMail_PostMaster);	MFC_DOMAIN_EMail_PostMaster = NULL;	}
	if (MFC_DOMAIN_EMail_MailFilter != NULL)	{	free(MFC_DOMAIN_EMail_MailFilter);	MFC_DOMAIN_EMail_MailFilter = NULL;	}
	
//	if (MFC_DNSBL_Zonename != NULL)				{	free(MFC_DNSBL_Zonename);			MFC_DNSBL_Zonename			= NULL; }
	if (MFC_BWL_ScheduleTime != NULL)			{	free(MFC_BWL_ScheduleTime);			MFC_BWL_ScheduleTime		= NULL; }

	if (MFC_Message_FooterText != NULL)			{	free(MFC_Message_FooterText);		MFC_Message_FooterText		= NULL;	}
	if (MFC_Multi2One != NULL)					{	free(MFC_Multi2One);				MFC_Multi2One				= NULL; }
}
*/
/*typedef char MFT_Filter_ListAttachments_STRING[50];
typedef MFT_Filter_ListAttachments_STRING MFT_Filter_ListAttachments_ARRAY[150];
static MFT_Filter_ListAttachments_ARRAY MFT_Filter_ListAttachmentsX;
*/
/*
// Reads the configuration ...
bool MF_ConfigRead(void)
{
	int rc;
	struct stat statInfo;
	char szTemp[MAX_PATH];
	
	rc = 0;

	///////////////////////////
	//
	// Here is the ideal area to test some code.
	//
	// Just return false after your code, 
	// MF will unload and won't complain too much ....
	//
	///////////////////////////
//	return false;


	///////////////////////////
	//
	// --- end of test area ---
	//
	///////////////////////////

	MFD_Out(MFD_SOURCE_CONFIG,"MFC: Starting with %s.\n",MAILFILTER_CONFIGURATION_MAILFILTER);

	if (stat(MAILFILTER_CONFIGURATION_MAILFILTER,&statInfo))
		rc = 98;
	if (rc != 0)	goto MF_ConfigRead_ERR;

	if (statInfo.st_size < 3999)
		rc = 97;	
	if (rc != 0)	goto MF_ConfigRead_ERR;
	
	char* MFC_ConfigVersion = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 0, MFC_ConfigVersion) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}
	if (memcmp(MFC_ConfigVersion,MAILFILTER_CONFIGURATION_SIGNATURE,19) != 0)
		rc = 99;
	free(MFC_ConfigVersion);
	if (rc != 0)
		goto MF_ConfigRead_ERR;

	MFT_Local_ServerName = (char*)malloc(_MAX_SERVER);
	MF_GetServerName ( MFT_Local_ServerName );

	MFC_GWIA_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 1*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_GWIA_ROOT) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}
	MF_MakeValidPath(MFC_GWIA_ROOT);

	MFC_MFLT_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 2*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_MFLT_ROOT) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}
	MF_MakeValidPath(MFC_MFLT_ROOT);

#ifndef WIN32
	MFT_InOutSameServerVolume = (MF_CheckPathSameVolume( MFC_GWIA_ROOT , MFC_MFLT_ROOT ) == 1);
	MFT_RemoteDirectories = (!MF_CheckPathSameServer( MFC_GWIA_ROOT , "SYS:SYSTEM" ));
	if (!MFT_RemoteDirectories) MFT_RemoteDirectories = (!MF_CheckPathSameServer( MFC_MFLT_ROOT , "SYS:SYSTEM" ));
#else
	MFT_InOutSameServerVolume = false;
#endif

	MFT_MF_LogDir = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	sprintf(MFT_MF_LogDir,"%s"IX_DIRECTORY_SEPARATOR_STR"MFLOG"IX_DIRECTORY_SEPARATOR_STR,MFC_MFLT_ROOT);


	MFC_DOMAIN_Name = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 3*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_DOMAIN_Name				) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}

	MFC_DOMAIN_EMail_MailFilter = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 4*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_DOMAIN_EMail_MailFilter	) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}

	MFC_DOMAIN_EMail_PostMaster = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 5*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_DOMAIN_EMail_PostMaster	) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}

	MFC_DOMAIN_Hostname = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 6*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_DOMAIN_Hostname);
//	if (MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 6*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_DOMAIN_Hostname			) < 1 ) {	rc = 10;	goto MF_ConfigRead_ERR;	}

	MFC_Multi2One = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 7*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_Multi2One);

//	MFC_DNSBL_Zonename = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
//	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, (7*MAILFILTER_CONFIGURATIONFILE_STRING + (MAILFILTER_CONFIGURATIONFILE_STRING/2)), MFC_DNSBL_Zonename);

	MFC_BWL_ScheduleTime = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 8*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_BWL_ScheduleTime);
	
	
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

	MFC_EnableIncomingRcptCheck =
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3539);
		
	MFC_EnableAttachmentDecoder =
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3541);
		
	MFC_EnablePFAFunctionality = 
		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3543);

//	MFC_DropUnresolvableRelayHosts = 
//		MF_ConfigReadInt(MAILFILTER_CONFIGURATION_MAILFILTER, 3545);

	// Next start at 3547
	
	// require at least 10 dirs
	if (MFC_MAILSCAN_Scan_DirNum < 10) MFC_MAILSCAN_Scan_DirNum = 10;

	// read in the optional footer message
	strncpy(szTemp,MFC_ConfigDirectory,MAX_PATH);
	strncat(szTemp,"MSGFOOT.MFT",MAX_PATH);
	
	MFC_Message_FooterText = (char*)malloc(4000);
	MF_ConfigReadFile(szTemp,MFC_Message_FooterText,3999);
	
	if (!MFT_RemoteDirectories)
	{
		// Check for existence of directories ...
		if (chdir(MFC_MFLT_ROOT)) {	rc=301; goto MF_ConfigRead_ERR;	}
		if (chdir(MFC_GWIA_ROOT)) {	rc=302; goto MF_ConfigRead_ERR;	}
	
		// Create Logging Directory if it is missing
//#ifdef WIN32
//		if (chdir(MFT_MF_LogDir))				mkdir((const char*)MFT_MF_LogDir,0);
//#else
		if (chdir(MFT_MF_LogDir))				mkdir(MFT_MF_LogDir,S_IRWXU);
//#endif
	}

	// Initialize MailFilter BlockList Cache
	if ( MF_Filter_InitLists() == false ) {	rc = 299;	goto MF_ConfigRead_ERR;	}

// Error Handling goes here ...
MF_ConfigRead_ERR:
	if ( rc > 0 )
	{
#ifdef N_PLAT_NLM
		printf(MF_Msg(MSG_CONFIG_ERRORINFO),rc);
#endif

		if (rc < 100)
		{
			MF_DisplayCriticalError(CONMSG_CONFIG_ERRCONFIGFORMAT);
		} else {
			if (rc < 200)
			{
				MF_DisplayCriticalError(CONMSG_CONFIG_ERRCONFIGURATION);
			} else {
				if ( rc < 300 )
				{
					MF_DisplayCriticalError(CONMSG_CONFIG_ERRFILTERLIST);
				} else {
					if ( rc < 350 )
					{
						MF_DisplayCriticalError(CONMSG_CONFIG_ERRDIRECTORIES);
					} else {
							MF_DisplayCriticalError(CONMSG_CONFIG_ERRGENERIC);
					}
				}
			}
		}
		
		MF_ConfigFree();

		return false;
	}

	// No Errors! Register Cleanup ...

#ifndef __NOVELL_LIBC__
	atexit(MF_ConfigFree);
#endif
	return true;
	
}
*/

/*
 *
 *
 *  --- eof ---
 *
 *
 */

