
#define _MFD_MODULE			"MFNLM-UI.CPP"
#include "MailFilter.h"

void MF_UI_ShowConfiguration(void)
{
	char 	szTemp[82];
#ifdef __NOVELL_LIBC__
	struct utsname u;
	uname(&u);
#endif
		
	sprintf(szTemp, "Active Configuration - Use F9 to re-read configuration.");
	MF_StatusUI_UpdateLog(szTemp);

	/*------------------------------------------------------------------------
	**	Place information on portal.
	*/
	//
#ifdef __NOVELL_LIBC__
	char szHost[82];
	if (!gethostname (szHost,81))
	{
		sprintf(szTemp, " gethostname..: %s ",szHost);
		MF_StatusUI_UpdateLog(szTemp);
	}

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

	sprintf(szTemp, " Directories..: %d Queues",MF_GlobalConfiguration.MailscanDirNum); 
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, "  MailFilter..: %s",MF_GlobalConfiguration.MFLTRoot.c_str());
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, "  GWIA Home...: %s",MF_GlobalConfiguration.GWIARoot.c_str()); 
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, "  Logging To..: %s",MF_GlobalConfiguration.LogDirectory.c_str()); 
	MF_StatusUI_UpdateLog(szTemp);


}

/* eof */

