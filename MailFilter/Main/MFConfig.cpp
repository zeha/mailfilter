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
 *
 *
 *  --- eof ---
 *
 *
 */

