/*
 +
 +		MFNLM-UI.cpp
 +		
 +		NetWare Platform Specific Functions
 +		User Interface stuff
 +
 +		Copyright 2001-2004 Christian Hofstädtler.
 +
 */
 
#define _MFD_MODULE			"MFNLM-UI.CPP"
#include "MailFilter.h"
#include "MFVersion.h"

void MF_UI_ShowConfiguration(void)
{
	char 	szTemp[82];
#ifdef __NOVELL_LIBC__
	struct utsname u;
	uname(&u);
#endif
		
	sprintf(szTemp, "Active Configuration:        [MailFilter compiled: %s]",MAILFILTERCOMPILED);
	MF_StatusUI_UpdateLog(szTemp);

	/*------------------------------------------------------------------------
	**	Place information on portal.
	*/
	//
#ifdef __NOVELL_LIBC__
	char szHost[82];
	if (!gethostname (szHost,81))
	{
		sprintf(szTemp, " OS Host Name.: %s ",szHost);
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

	sprintf(szTemp, " Virus Scan...: %s, Delay: %d, Decode: %s",MFC_MAILSCAN_Enabled == 0 ? "Off" : "On",MF_GlobalConfiguration.MailscanTimeout,MF_GlobalConfiguration.EnableAttachmentDecoder == 0 ? "Off" : "On");
	MF_StatusUI_UpdateLog(szTemp);

	sprintf(szTemp, " Loaded Rules.: %d present in memory",MF_CountAllFilters());
	MF_StatusUI_UpdateLog(szTemp);

	/* this is inverse */
	sprintf(szTemp, " Scheduling...: %s, %d rule(s)", MFBW_CheckCurrentScheduleState() == false ? "On" : "Off",MF_CountFilters(MailFilter_Configuration::schedule));
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

	if (MF_GlobalConfiguration.LoginUserName != "")
	{
		sprintf(szTemp, " Logging in as: %s",MF_GlobalConfiguration.LoginUserName.c_str());
		MF_StatusUI_UpdateLog(szTemp);
	}

}

static int _MF_UI_PromptUsernamePassword_Login(FIELD* fp, int selectKey, int *changedField, NUTInfo *handle)
{
}

static int _MF_UI_PromptUsernamePassword_Cancel(FIELD* fp, int selectKey, int *changedField, NUTInfo *handle)
{
}

bool MF_UI_PromptUsernamePassword(std::string prompt, std::string username, std::string password)
{
	unsigned long	line;
	int		formSaved;
	char	newUser[MAX_PATH];
	char	newPass[MAX_PATH];
	std::string myPrompt = prompt;
	
	if (myPrompt == "")
		myPrompt = "Please enter your NDS Username (fully qualified) and your password.";

	NWSPushList(MF_NutInfo);
	NWSInitForm(MF_NutInfo);
	
	/*------------------------------------------------------------------------
	** Define the fields in the form
	*/
	line = 0;
	
	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)(myPrompt.c_str()), MF_NutInfo);
	line += 2;
	
	// pad
	line++;
	
	// username+password
	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Enter NDS Username:", MF_NutInfo);
	line++;

	newUser[0] = 0;
	NWSAppendScrollableStringField  (line, 5, 44, NORMAL_FIELD, (_MF_NUTCHAR)newUser, MAX_PATH, NULL, EF_ANY, NULL, MF_NutInfo);
	line++;


	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Password:", MF_NutInfo);
	line++;
	
	newPass[0] = 0;
	NWSAppendPasswordField  (line, 5, 44, NORMAL_FIELD, (_MF_NUTCHAR)newPass, MAX_PATH, NULL, false, NULL, '*', MF_NutInfo);
	line++;

	line++;
	
	NWSAppendHotSpotField ( line, 10, NORMAL_FIELD, (_MF_NUTCHAR)"Login", _MF_UI_PromptUsernamePassword_Login, MF_NutInfo); 
	NWSAppendHotSpotField ( line, 25, NORMAL_FIELD, (_MF_NUTCHAR)"Cancel", _MF_UI_PromptUsernamePassword_Cancel, MF_NutInfo); 

	

	formSaved = NWSEditForm (
		0,	//headernum
		4,			//line
		14,			//col
		line+5,		//portalHeight
		50,			//portalWidth
		line+1,		//virtualHeight,
		50,			//virtualWidth,
		FALSE,		//ESCverify,
		FALSE,		//forceverify,
		NULL,		//confirmMessage,
		MF_NutInfo
	);

	/*------------------------------------------------------------------------
	** This function returns TRUE if the form was saved, FALSE if not.
	** If the form was not saved you must restore all variables to their
	** original values manually
	*/
	if (formSaved)
	{
		username = newUser;
		password = newPass;
	}

	NWSDestroyForm (MF_NutInfo);
	NWSPopList(MF_NutInfo);

	return (bool)formSaved;
}

bool MF_UI_ShowKeys(int keys)
{
	char szTemp[80+2];
	int curItem;

	memset(szTemp,' ',80);
	szTemp[81]=0;
	NWSShowLineAttribute ( 24 , 0 , (_MF_NUTCHAR)szTemp , VREVERSE , 80 , (ScreenStruct*)MF_NutInfo->screenID );
	
	curItem=1;
	if (chkFlag(keys,MFUI_KEYS_SORT)) 		{	strncpy(szTemp+curItem, "F7-Sort",7);			curItem=curItem+10;	}
	if (chkFlag(keys,MFUI_KEYS_IMPORT)) 	{	strncpy(szTemp+curItem, "F8-Import",9);			curItem=curItem+12;	}
	if (chkFlag(keys,MFUI_KEYS_EXPORT)) 	{	strncpy(szTemp+curItem, "F9-Export",9);			curItem=curItem+12; }
	if (chkFlag(keys,MFUI_KEYS_NEW)) 		{	strncpy(szTemp+curItem, "INS-New",7);			curItem=curItem+10;	}
	if (chkFlag(keys,MFUI_KEYS_DELETE)) 	{	strncpy(szTemp+curItem, "DEL-Delete",10); 		curItem=curItem+12;	}
	if (chkFlag(keys,MFUI_KEYS_SELECT)) 	{	strncpy(szTemp+curItem, "RET-Select",10);		curItem=curItem+12; }
	if (chkFlag(keys,MFUI_KEYS_SHOW)) 		{	strncpy(szTemp+curItem, "TAB-Detail",10);		curItem=curItem+12; }
	if (chkFlag(keys,MFUI_KEYS_EXIT)) 		{	strncpy(szTemp+curItem, "ESC-Exit",8);			curItem=curItem+11; }
	if (chkFlag(keys,MFUI_KEYS_SAVE)) 		{	strncpy(szTemp+curItem, "ESC-Save",8);			curItem=curItem+11; }
	if (chkFlag(keys,MFUI_KEYS_CANCEL)) 	{	strncpy(szTemp+curItem, "ESC-Cancel",10);		curItem=curItem+12; }

	NWSShowLineAttribute ( 24 , (unsigned long)((80-curItem)/2) , (_MF_NUTCHAR)szTemp , VREVERSE , (unsigned long)curItem , (ScreenStruct*)MF_NutInfo->screenID );

	return true;
}



/* eof */

