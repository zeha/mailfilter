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
#include "MFConfig-defines.h"
#include <pcreposix45.h>
#include "MFVersion.h"

int MFC_CurrentList = 0;

#ifndef NUT_SEVERITY_FATAL
#define NUT_SEVERITY_FATAL 2
#endif


static void MFConfig_Util_Sort(void* nutHandle);


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
			NWSDisplayErrorText ( MSG_ERROR_FILE_NOT_FOUND , NUT_SEVERITY_FATAL , MF_NutInfo , szTemp );
			
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
				NWSDisplayErrorText ( MSG_ERROR_FILE_NOT_FOUND , NUT_SEVERITY_FATAL , MF_NutInfo , szTemp );
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
				NWSDisplayErrorText ( MSG_ERROR_LICENSE_NOT_FOUND , NUT_SEVERITY_FATAL , MF_NutInfo , NULL );

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
					if (f1.matchfield == MailFilter_Configuration::blacklist)
						rc = true;
					else
					if (f2.matchfield == MailFilter_Configuration::blacklist)
						rc = false;
					else
					if (f1.matchfield == MailFilter_Configuration::attachment)
						rc = true;
					else
					if (f2.matchfield == MailFilter_Configuration::attachment)
						rc = false;
					else
					if (f1.matchfield == MailFilter_Configuration::archiveContentName)
						rc = true;
					else
					if (f2.matchfield == MailFilter_Configuration::archiveContentName)
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
	case MailFilter_Configuration::always:
		szTemp[0] = '*';	break;
	case MailFilter_Configuration::attachment:
		szTemp[0] = 'A';	break;
	case MailFilter_Configuration::email:
	case MailFilter_Configuration::emailBothAndCC:
		szTemp[0] = 'E';	break;
	case MailFilter_Configuration::subject:
		szTemp[0] = 'S';	break;
	case MailFilter_Configuration::size:
		szTemp[0] = 's';	break;
	case MailFilter_Configuration::emailFrom:
		szTemp[0] = 'F';	break;
	case MailFilter_Configuration::emailTo:
	case MailFilter_Configuration::emailToAndCC:
		szTemp[0] = 'T';	break;
	case MailFilter_Configuration::blacklist:
		szTemp[0] = 'B';	break;
	case MailFilter_Configuration::ipUnresolvable:
		szTemp[0] = 'U';	break;
	case MailFilter_Configuration::archiveContentName:
	case MailFilter_Configuration::archiveContentCount:
		szTemp[0] = 'Z';	break;
	case MailFilter_Configuration::virus:
		szTemp[0] = 'V';	break;
	default:
		szTemp[0] = '?';	break;
	}
	
	if (flt.type == MailFilter_Configuration::noMatch)
	{
		szTemp[1] = '!';
	}
	
	if (flt.matchfield == MailFilter_Configuration::virus)
	{
		// no useful expression here
		strcpy(szTemp+2,"Virus Scan by AV NLM - if possible");
	} else {
		// use expression as text
		strncpy(szTemp+2,flt.expression.c_str(),70);
	}
	
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
	char szTemp[MAX_BUF+1];
	int rc;
	//int curItem = -1;
	//int* cI;
	char defaultNotify;
	int overrideNotify = false;
	
	nutHandle = nutHandle;	/* rid compiler warning */
	
	defaultNotify = 0;
	if (MF_GlobalConfiguration.DefaultNotification_AdminIncoming)		defaultNotify |= MAILFILTER_NOTIFICATION_ADMIN_INCOMING;
	if (MF_GlobalConfiguration.DefaultNotification_AdminOutgoing)		defaultNotify |= MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
	if (MF_GlobalConfiguration.DefaultNotification_InternalRecipient)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
	if (MF_GlobalConfiguration.DefaultNotification_InternalSender)		defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
	if (MF_GlobalConfiguration.DefaultNotification_ExternalRecipient)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;
	if (MF_GlobalConfiguration.DefaultNotification_ExternalSender)		defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING;
	
	szTemp[0] = 0;
	
	rc = NWSEditString ( 0,
					0,
					1,	/* height */
					50, /* width */
					MSG_EDIT_UTIL_IMPORTFILE_GET, /* header */
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
			NWSDisplayErrorText ( MSG_ERROR_FILE_NOT_FOUND , NUT_SEVERITY_INFORM , MF_NutInfo , szTemp );
			
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
		
			if (!MF_GlobalConfiguration.ReadFilterListFromRulePackage(szTemp))
				NWSDisplayErrorText ( MSG_ERROR_UNKNOWN_ERROR , NUT_SEVERITY_WARNING , MF_NutInfo , szTemp );

			NWSEndWait(MF_NutInfo);
			
			MFConfig_UpdateFilterList();
			NWSUngetKey(UGK_SPECIAL_KEY,UGK_REFRESH_KEY,MF_NutInfo);
		}
		
	
	} else return;
				
}

static void MFConfig_Util_ExportListToFile(void* nutHandle)
{
	char szTemp[MAX_BUF+1];
	int rc;
	int curItem = -1;
	
	nutHandle = nutHandle;	/* rid compiler warning */
	
	szTemp[0] = 0;
	
	rc = NWSEditString ( 0,
					0,
					1,	/* height */
					50, /* width */
					MSG_EDIT_UTIL_EXPORTFILE_GET, /* header */
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
			NWSDisplayErrorText ( MSG_ERROR_FILE_EXISTS , NUT_SEVERITY_INFORM , MF_NutInfo , szTemp );
			
		} else {
			NWSStartWait(0,0,MF_NutInfo);

			if (!MF_GlobalConfiguration.WriteFilterList(szTemp))
				NWSDisplayErrorText ( MSG_ERROR_UNKNOWN_ERROR , NUT_SEVERITY_WARNING , MF_NutInfo , szTemp );
			
			NWSEndWait(MF_NutInfo);
			
		}
		
	
	} else return;
				
}

/****************************************************************************
** Edit filter detail dialog
*/
static	FIELD		*MFConfig_EditFilterDialog_fieldDescriptionLbl;
static	FIELD		*MFConfig_EditFilterDialog_fieldExpressionEdit;

static int MFConfig_EditFilterDialog_MenuAction(int option, void *parameter)   {      parameter = parameter;	return option;   }
static int MFConfig_EditFilterDialog_MenuActionAction(int option, void *parameter)   
{
#pragma unused(parameter)

	if(option==MAILFILTER_MATCHACTION_COPY)
		MFConfig_EditFilterDialog_fieldDescriptionLbl->fieldData = (unsigned char*)MF_NMsg(EDIT_FILTERS_DESTINATION);
		else
		MFConfig_EditFilterDialog_fieldDescriptionLbl->fieldData = (unsigned char*)MF_NMsg(EDIT_FILTERS_DESCRIPTION); 
	
	NWSSetFormRepaintFlag(TRUE,MF_NutInfo);
	return option;
}
static int MFConfig_EditFilterDialog_MenuActionMatchfield(int option, void *parameter)   
{
#pragma unused(parameter)

	if (option==MailFilter_Configuration::virus)
		MFConfig_EditFilterDialog_fieldExpressionEdit->fieldData = (unsigned char*)"-";
	
	NWSSetFormRepaintFlag(TRUE,MF_NutInfo);
	return option;
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
	ctlMatchfield = NWSInitMenuField (MSG_MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuActionMatchfield,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ALWAYS,		MailFilter_Configuration::always		,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ATTACHMENT,	MailFilter_Configuration::attachment	,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL,			MailFilter_Configuration::email			,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_BOTHANDCC,MailFilter_Configuration::emailBothAndCC,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_FROM,	MailFilter_Configuration::emailFrom		,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_TO,		MailFilter_Configuration::emailTo		,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_EMAIL_TOANDCC,	MailFilter_Configuration::emailToAndCC	,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_SUBJECT,		MailFilter_Configuration::subject		,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_SIZE,			MailFilter_Configuration::size			,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ARCHIVECONTENTNAME,MailFilter_Configuration::archiveContentName,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_ARCHIVECONTENTCOUNT,MailFilter_Configuration::archiveContentCount,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_BLACKLIST,		MailFilter_Configuration::blacklist		,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_IPUNRESOLVABLE,MailFilter_Configuration::ipUnresolvable,MF_NutInfo);
	NWSAppendToMenuField (ctlMatchfield, EDIT_FILTERS_MATCHFIELD_VIRUS,			MailFilter_Configuration::virus			,MF_NutInfo);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchfield, ctlMatchfield, NULL, MF_NutInfo);   
	line++;

	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHACTION), MF_NutInfo);
	ctlMatchaction = NWSInitMenuField (MSG_MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuActionAction, 	MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_DROP_MAIL,	MailFilter_Configuration::dropMail, 	MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_PASS,		MailFilter_Configuration::pass,		 	MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_SCHEDULE,	MailFilter_Configuration::schedule,		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_NOSCHEDULE,	MailFilter_Configuration::noschedule,	MF_NutInfo);
	NWSAppendToMenuField (ctlMatchaction, EDIT_FILTERS_MATCHACTION_COPY,		MailFilter_Configuration::copy,			MF_NutInfo);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchaction, ctlMatchaction, NULL, MF_NutInfo);   
	line++;


	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_MATCHTYPE), MF_NutInfo);
	ctlMatchtype = NWSInitMenuField (MSG_MENU_MAIN_EDIT_FILTERS, 10, 40, MFConfig_EditFilterDialog_MenuAction, 			MF_NutInfo);
	NWSAppendToMenuField (ctlMatchtype, EDIT_FILTERS_MATCHTYPE_MATCH,			MailFilter_Configuration::match, 		MF_NutInfo);
	NWSAppendToMenuField (ctlMatchtype, EDIT_FILTERS_MATCHTYPE_NOMATCH,			MailFilter_Configuration::noMatch,		MF_NutInfo);
	NWSAppendMenuField (line, 20, REQUIRED_FIELD, &newMatchtype, ctlMatchtype, NULL, MF_NutInfo);   
	line++;
	
	NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_EXPRESSION), MF_NutInfo);
	strcpy (newExpression, flt->expression.c_str());
	MFConfig_EditFilterDialog_fieldExpressionEdit = NWSAppendScrollableStringField  (line, 20, 55, REQUIRED_FIELD, (_MF_NUTCHAR)newExpression, 500, (_MF_NUTCHAR)"A..Za..z \\/:_-+.0..9{}[]()*#!\"§$%&=?~", EF_ANY, NULL, MF_NutInfo); 
	line++;

	MFConfig_EditFilterDialog_fieldDescriptionLbl = NWSAppendCommentField (line, 1, MF_NMsg(EDIT_FILTERS_DESCRIPTION), MF_NutInfo);
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
		MFConfig_EditFilterDialog_fieldDescriptionLbl->fieldData = (unsigned char*)"Destination";
	
	formSaved = NWSEditForm (
		MSG_EDIT_FILTERS_EDITRULE,		//headernum	//0
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

	if (
			(newMatchfield != MailFilter_Configuration::size) || 
			(newMatchfield != MailFilter_Configuration::blacklist) || (newMatchfield != MailFilter_Configuration::ipUnresolvable) 
		)
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
		/* I-	header			*/	MSG_MENU_MAIN_EDIT_FILTERS,
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
					MSG_MENU_MAIN_LICENSEKEY, /* header */
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
	char	newLoginUsername[MAILFILTER_CONFIGURATION_LENGTH];
	char	newLoginPassword[MAILFILTER_CONFIGURATION_LENGTH];
	char	newEmailOffice[50];
	BOOL	newNotification_InternalRcpt = (BOOL)MF_GlobalConfiguration.DefaultNotification_InternalRecipient;
	BOOL	newNotification_InternalSndr = (BOOL)MF_GlobalConfiguration.DefaultNotification_InternalSender;
	BOOL	newNotification_ExternalRcpt = (BOOL)MF_GlobalConfiguration.DefaultNotification_ExternalRecipient;
	BOOL	newNotification_ExternalSndr = (BOOL)MF_GlobalConfiguration.DefaultNotification_ExternalSender;
	BOOL	newNotification_AdminIn = (BOOL)MF_GlobalConfiguration.DefaultNotification_AdminIncoming;
	BOOL	newNotification_AdminOut = (BOOL)MF_GlobalConfiguration.DefaultNotification_AdminOutgoing;
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
	BOOL	newDropBrokenMessages = (BOOL)MF_GlobalConfiguration.DropBrokenMessages;
	BOOL	newDropPartialMessages = (BOOL)MF_GlobalConfiguration.DropPartialMessages;

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

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"General E-Mail Address without Domain Names:", MF_NutInfo);
	NWSAppendCommentField (line, 61, (_MF_NUTCHAR)"(blank=disabled)", MF_NutInfo);
	strcpy (newEmailOffice, MF_GlobalConfiguration.Multi2One.c_str());
	NWSAppendStringField (line+1, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newEmailOffice, (_MF_NUTCHAR)"A..Za..z_-+!.0..9", EF_SET, MF_NutInfo);
	line += 2;
	
	line++;
	
	// username+password
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_LOGIN_USERNAME], MF_NutInfo);
	strcpy (newLoginUsername, MF_GlobalConfiguration.LoginUserName.c_str());
	NWSAppendScrollableStringField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newLoginUsername, MAILFILTER_CONFIGURATION_LENGTH, NULL, EF_ANY, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_LOGIN_PASSWORD], MF_NutInfo);
	strcpy (newLoginPassword, MF_GlobalConfiguration.LoginUserPassword.c_str());
	NWSAppendPasswordField  (line, 20, 55, NORMAL_FIELD, (_MF_NUTCHAR)newLoginPassword, MAILFILTER_CONFIGURATION_LENGTH, NULL, true, NULL, '*', MF_NutInfo);
	line++;

	line++;

	// schedule
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)programMesgTable[EDIT_BWL_SCHEDULETIME], MF_NutInfo);
	strcpy (newScheduleTime, MF_GlobalConfiguration.BWLScheduleTime.c_str());
	NWSAppendScrollableStringField (line, 50, 25, NORMAL_FIELD, (_MF_NUTCHAR)newScheduleTime, MAILFILTER_CONFIGURATION_LENGTH, (_MF_NUTCHAR)"0..9-:,", EF_SET, NULL, MF_NutInfo); 
	line++;

	// other stuff
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"GWIA Mode/Version: (see manual)", MF_NutInfo);
	newGwiaVersion = (unsigned long)MF_GlobalConfiguration.GWIAVersion;
	NWSAppendUnsignedIntegerField (line, 50, NORMAL_FIELD, &newGwiaVersion, 550, 600, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Number of Mail Queues:", MF_NutInfo);
	newScanDirNum = (unsigned long)MF_GlobalConfiguration.MailscanDirNum;
	NWSAppendUnsignedIntegerField (line, 50, NORMAL_FIELD, &newScanDirNum, 0, 50, NULL, MF_NutInfo);
	line++;

/*	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"MailFilter Control/Config Password:", MF_NutInfo);
	strcpy (newControlPassword, MF_GlobalConfiguration.ControlPassword.c_str());
	NWSAppendPasswordField(line, 50, 20, NORMAL_FIELD, (_MF_NUTCHAR)newControlPassword, 50, NULL, true, NULL, '*', MF_NutInfo); 
	line++;
*/	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable PFA Functionality:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newEnablePFA, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable Incoming Recipient Rule Check:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newEnableIncomingRcptCheck, NULL, MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Drop Broken Messages:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newDropBrokenMessages, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Drop Partial Messages:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newDropPartialMessages, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable Netware Remote Manager Snap-In:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newEnableNRMThread, NULL, MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Enable Mail Restore in N. Remote Manager:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newEnableNRMRestore, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Notify Postmaster on Log-Cycle Error:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_AdminLogs, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Send Daily Status Report to Postmaster:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_AdminDailyReport, NULL, MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Virus Scanner Integration:", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Unpack Mails for Virus Scanner:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newEnableAttachmentDecoder, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Seconds to wait for Real Time Scan:", MF_NutInfo);
	newScanDirWait = (unsigned long)MF_GlobalConfiguration.MailscanTimeout;
	NWSAppendUnsignedIntegerField (line, 50, NORMAL_FIELD, &newScanDirWait, 0, 600, NULL, MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Problem Directory Cleanup:", MF_NutInfo);
	line++;
	
	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Maximum Total Size (kBytes):", MF_NutInfo);
	NWSAppendUnsignedIntegerField (line, 50, NORMAL_FIELD, &newProblemDir_MaxSize, 0, 1000000, NULL, MF_NutInfo);
	NWSAppendCommentField (line, 65, (_MF_NUTCHAR)"(0=disabled)", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Maximum File Age (Days):", MF_NutInfo);
	NWSAppendUnsignedIntegerField (line, 50, NORMAL_FIELD, &newProblemDir_MaxAge, 0, 10000, NULL, MF_NutInfo);
	NWSAppendCommentField (line, 65, (_MF_NUTCHAR)"(0=disabled)", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Notify Postmaster On Cleanup:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_AdminMailsKilled, NULL, MF_NutInfo);
	line++;

	line++;

	NWSAppendCommentField (line, 1, (_MF_NUTCHAR)"Default Notify On Filter Match:", MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Postmaster, Incoming:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_AdminIn, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Postmaster, Outgoing:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_AdminOut, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Internal Recipient:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_InternalRcpt, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"Internal Sender:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_InternalSndr, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"External Recipient:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_ExternalRcpt, NULL, MF_NutInfo);
	line++;

	NWSAppendCommentField (line, 3, (_MF_NUTCHAR)"External Sender:", MF_NutInfo);
	NWSAppendBoolField (line, 50, NORMAL_FIELD, &newNotification_ExternalSndr, NULL, MF_NutInfo);
	line++;



	formSaved = NWSEditForm (
		MSG_CONFIGAPP_EDITCONFIG_HDR,	//headernum
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
		MF_GlobalConfiguration.LoginUserName			= newLoginUsername;
		MF_GlobalConfiguration.LoginUserPassword		= newLoginPassword;

		// check if pass has changed, if yes encrypt ...
		if (strcmp(MF_GlobalConfiguration.ControlPassword.c_str(),newControlPassword) != 0)
			MFU_strxor((char*)newControlPassword,(char)18);
			
		MF_GlobalConfiguration.ControlPassword = 			newControlPassword;

		MF_GlobalConfiguration.DefaultNotification_AdminIncoming =(MailFilter_Configuration::Notification)newNotification_AdminIn;
		MF_GlobalConfiguration.DefaultNotification_AdminOutgoing =(MailFilter_Configuration::Notification)newNotification_AdminOut;
		
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
		MF_GlobalConfiguration.EnableIncomingRcptCheck		=(bool)newEnableIncomingRcptCheck;
		MF_GlobalConfiguration.DropBrokenMessages 			=(bool)newDropBrokenMessages;
		MF_GlobalConfiguration.DropPartialMessages 			=(bool)newDropPartialMessages;

		MF_GlobalConfiguration.GWIAVersion					=(unsigned int)newGwiaVersion;
		
		if (MF_GlobalConfiguration.GWIAVersion != 600)			// only allow 600 or 550
			MF_GlobalConfiguration.GWIAVersion = 550;

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
	MF_UI_ShowKeys(MFUI_KEYS_NONE);
	
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

		case MSG_MENU_MAIN_EDIT_CONFIGURATION:
			MF_UI_ShowKeys(MFUI_KEYS_SELECT|MFUI_KEYS_SAVE);
			MFConfig_EditConfig();
			break;

		case MSG_MENU_MAIN_EDIT_FILTERS:
			MF_UI_ShowKeys(MFUI_KEYS_IMPORT|MFUI_KEYS_EXPORT|MFUI_KEYS_NEW|MFUI_KEYS_DELETE|MFUI_KEYS_SELECT|MFUI_KEYS_SORT);
			MFConfig_EditFilters();
			break;

		case MSG_MENU_MAIN_LICENSEKEY:
			MF_UI_ShowKeys(MFUI_KEYS_CANCEL);
			MFConfig_EditLicense();
			break;

		case MSG_MENU_MAIN_SAVE_AND_EXIT:
			MF_UI_ShowKeys(MFUI_KEYS_CANCEL);
			if (NLM_VerifySaveExit())
			{
				MF_GlobalConfiguration.WriteToFile("");
				return(0);
			}
			break;
			
		case MSG_MENU_MAIN_SAVE_AND_RESTART:
			MF_UI_ShowKeys(MFUI_KEYS_CANCEL);
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
	MF_UI_ShowKeys(MFUI_KEYS_IMPORT);

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
	
	NWSAppendToMenu(MSG_MENU_MAIN_EDIT_CONFIGURATION, MSG_MENU_MAIN_EDIT_CONFIGURATION, MF_NutInfo);

	NWSAppendToMenu(MSG_MENU_MAIN_LICENSEKEY, MSG_MENU_MAIN_LICENSEKEY, MF_NutInfo);
	
	NWSAppendToMenu(MSG_MENU_MAIN_EDIT_FILTERS, MSG_MENU_MAIN_EDIT_FILTERS, MF_NutInfo);
	
	if (bStandalone)
		defaultOption = NWSAppendToMenu(MSG_MENU_MAIN_SAVE_AND_EXIT, MSG_MENU_MAIN_SAVE_AND_EXIT, MF_NutInfo);
	else
		defaultOption = NWSAppendToMenu(MSG_MENU_MAIN_SAVE_AND_RESTART, MSG_MENU_MAIN_SAVE_AND_RESTART, MF_NutInfo);

	NWSEnableInterruptKey	(	K_F8,	MFConfig_ImportLicenseKey,	MF_NutInfo	);
	MF_UI_ShowKeys(MFUI_KEYS_IMPORT|MFUI_KEYS_EXIT);

	/*------------------------------------------------------------------------
	**	Display the menu and allow user interaction.
	*/
	NWSMenu(
		/*	header				*/	MSG_CONFIGAPP_PROGRAMNAME,
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
	sprintf(szTemp,"  MailFilter Configuration Editor NLM/CUI Version %s",
									MAILFILTERVERNUM);
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
						MailFilter_Configuration::CurrentConfigVersion,MF_GlobalConfiguration.config_build);
	NWSShowLineAttribute ( 2 , 0 , (_MF_NUTCHAR)szTemp , VNORMAL , 80 , (struct ScreenStruct*)MF_NutInfo->screenID );

	NLM_MenuMain(bStandalone);

	if (!bStandalone)
		MF_NutDeinit();

	return 0;
}

/* eof */
