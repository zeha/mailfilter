/*
 +
 +		MFMail.h++
 +		
 +		Mail Data Structures
 +
 +		This is MailFilter!
 +		www.mailfilter.cc
 +		
 +		Copyright 2002 Christian Hofstädtler.
 +		
 +		
 +		- Aug/2002 ; ch   ; Initial Code
 +		
 +		
 */

//#include <string.h>
#include "MailFilter.h"
#include <iXList.h>

#ifndef _MFMAIL_HPP_
#define _MFMAIL_HPP_

// This header is not for generic C!
#ifdef __cplusplus

typedef struct MailFilter_MailData {
	char* szFileName;
	char* szFileIn;
	char* szFileOut;
	char* szFileWork;
	char* szScanDirectory;
	int iMailSource;
	long iMailSize;		// dont change for 64bit unless all functions are able to handle 64bit!
	long iTotalAttachmentSize;
	unsigned long iMailTimestamp;
	int iNumOfAttachments;
	char* szProblemMailDestination;
	char* szEnvelopeFrom;
	char* szEnvelopeRcpt;
	char* szMailFrom;
	char* szMailRcpt;
	char* szMailCC;
	char* szMailSubject;
	bool  bHaveFrom;
	bool  bHaveRcpt;
	bool  bHaveCC;
	bool  bHaveSubject;
	char* szErrorMessage;
	bool  bFilterMatched;
	int   iFilterNotify;
	int   iFilterAction;
	int	  iFilterHandle;
	bool  bSchedule;
	int	  iPriority;
	char* szReceivedFrom;
	bool  bHaveReceivedFrom;
	bool  bCopy;
	bool  bBrokenMessage;
	bool  bPartialMessage;
	// below only things we cannot copy to the MFS file
	iXList*	lstAttachments;
	iXList*	lstArchiveContents;
	iXList* lstCopies;
} MailFilter_MailData;

MailFilter_MailData* MailFilter_MailInit(const char* szFileName, int iMailSource);
int MailFilter_MailDestroy(MailFilter_MailData* mail);
int MailFilter_MailWrite(const char* szOutFile, MailFilter_MailData* m);
MailFilter_MailData* MailFilter_MailRead(const char* szInFile);

extern int MF_MailProblemReport(MailFilter_MailData* mMailInfo);
extern int MF_Notification_Send2(const char messageType, const char* bounceRcpt, MailFilter_MailData* mMailInfo);

extern int MF_ParseMail(MailFilter_MailData* m, bool bMiniMode);

#endif

#endif
/*++eof++*/
