/****************************************************************************
**	File:	MFCONFIG.H
**
**	Desc:	MailFilter Configuration Abstraction
**
**  (C) Copyright 2001-2003 Christian Hofstädtler. All Rights Reserved.
**							
*/

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

#include "../MailFilter/Config/MFConfig-defines.h"

typedef struct
{
	char matchfield;
	char expression[500];
	char name[60];
	char enabled;
	char enabledIncoming;
	char enabledOutgoing;
	char type;
	char notify;
	char action;
} MailFilter_Filter;


#ifdef _MAILFILTER_MFCONFIG_STORAGE
#define _MFCS(_mfcs_type,_mfcs_name,_mfcs_default)	_mfcs_type _mfcs_name = _mfcs_default
#else
#define _MFCS(_mfcs_type,_mfcs_name,_mfcs_default)	extern _mfcs_type _mfcs_name
#endif

	/*------------------------------------------------------------------------
	**	MailFilter Specific things
	*/
	// --- These are *MAILFILTER* constants! They have to be the same in ALL MailFilter programs. ---
	#undef MAX_BUF
	#define MAX_BUF 4096
	#undef MAX_PATH
	#define MAX_PATH _MAX_SERVER+_MAX_VOLUME+_MAX_PATH+10

	// DON'T TOUCH THESE UNLESS YOU ***REALLY*** KNOW WHAT YOU ARE DOING
	#define MAILFILTER_CONFIGURATION_BUILDCHECK			MFC_ConfigBuild != MAILFILTER_CONFIGURATION_THISBUILD
	#define MAILFILTER_CONFIGURATIONFILE_STRING			(MAILFILTER_CONFIGURATION_LENGTH+1)

	/*------------------------------------------------------------------------
	**	MailFilter Configuration Storage
	*/
	_MFCS(char,		MFC_ConfigDirectory			[MAX_PATH], ""		);
	_MFCS(char,		MFC_ConfigFile				[MAX_PATH], ""		);
	_MFCS(char,		MFC_ConfigFileBackup		[MAX_PATH], ""		);
	_MFCS(char,		MFC_ConfigFileVoid			[MAX_PATH], ""		);
	_MFCS(int,		MFC_ConfigBuild,						0		);
	_MFCS(char*,	MFC_GWIA_ROOT,							NULL	);
	_MFCS(char*,	MFC_MFLT_ROOT,							NULL	);
	_MFCS(char*,	MFC_DOMAIN_Name,						NULL	);
	_MFCS(char*,	MFC_DOMAIN_Hostname,					NULL	);
	_MFCS(char*,	MFC_DOMAIN_EMail_PostMaster,			NULL	);
	_MFCS(char*,	MFC_DOMAIN_EMail_MailFilter,			NULL	);
	_MFCS(char*,	MFC_License_Key,						NULL	);
	_MFCS(char*,	MFC_ControlPassword,					NULL	);
	_MFCS(int,	 	MFC_Notification_EnableInternalRcpt,	0		);
	_MFCS(int,	 	MFC_Notification_EnableInternalSndr,	0		);
	_MFCS(int,	 	MFC_Notification_EnableExternalRcpt,	0		);
	_MFCS(int,	 	MFC_Notification_EnableExternalSndr,	0		);
	_MFCS(int,		MFC_MAILSCAN_Scan_DirNum,				10		);
	_MFCS(int,		MFC_MAILSCAN_Scan_DirWait,				180		);
	_MFCS(int,		MFC_Notification_AdminLogs,				0		);
	_MFCS(int,		MFC_ProblemDir_MaxSize,					0		);
	_MFCS(int,		MFC_ProblemDir_MaxAge,					0		);
	_MFCS(int,		MFC_Notification_AdminMailsKilled,		1		);
	_MFCS(int,		MFC_Notification_AdminDailyReport,		0		);
	_MFCS(int,		MFC_GWIA_Version,						600		);
	_MFCS(int,		MFC_EnableIncomingRcptCheck,			0		);
	_MFCS(int,		MFC_EnableAttachmentDecoder,			1		);
	_MFCS(char*,	MFC_Message_FooterText,					NULL	);
	_MFCS(char*,	MFC_BWL_ScheduleTime,					NULL	);
//	_MFCS(char*,	MFC_DNSBL_Zonename,						NULL	);
	_MFCS(int,		MFC_EnablePFAFunctionality,				0		);
	_MFCS(char*,	MFC_Multi2One,							NULL	);
//	_MFCS(int,		MFC_DropUnresolvableRelayHosts,			0		);


	
	#define MFT_Filter_ListAttachments_MAX 150
	#define MFT_Filter_ListSubjects_MAX 1000
	#define MFT_Filter_ListSenders_MAX 150
	#define MFT_ModifyInt_ListFrom_MAX 1
	#define MailFilter_MaxFilters		2000
	#define MAILFILTER_CONFIGURATION_MAILFILTER		MFC_ConfigFile
	#define MAILFILTER_CONFIGBACKUP__MAILFILTER		MFC_ConfigFileBackup
	#define MAILFILTER_CONFIGERROR___MAILFILTER		MFC_ConfigFileVoid

#undef _MFCS

