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
//	int   iBlacklistResults;
	char* szReceivedFrom;
	bool  bHaveReceivedFrom;
	bool  bCopy;
	iXList*	lstAttachments;	// should be the latest entry
	iXList* lstCopies;
} MailFilter_MailData;

MailFilter_MailData* MailFilter_MailInit(char* szFileName, int iMailSource);
int MailFilter_MailDestroy(MailFilter_MailData* mail);
int MailFilter_MailWrite(char* szOutFile, MailFilter_MailData* m);
MailFilter_MailData* MailFilter_MailRead(char* szInFile);

extern int MF_MailProblemReport(MailFilter_MailData* mMailInfo);
extern int MF_Notification_Send2(const char messageType, const char* bounceRcpt, MailFilter_MailData* mMailInfo);

#endif

#endif
/*++eof++*/
