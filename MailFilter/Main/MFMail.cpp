/*
 +
 +		MFMail.cpp
 +		
 +		Mail Data Structure Functions
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

#define _MFD_MODULE		"MFMail.cpp"
#include <MailFilter.h>
#include "MFMail.h++"

int MailFilter_MailWrite(char* szOutFile, MailFilter_MailData* m) {

#define _MFMW_PRINT_STRING(szString)	if (m->szString == NULL) fprintf(fp,"\n"); else fprintf(fp,"%s\n",m->szString);
#define _MFMW_PRINT_INT(iInt)			fprintf(fp,"%d\n",m->iInt);
#define _MFMW_PRINT_UL(ulULong)			fprintf(fp,"%u\n",m->ulLong);
#define _MFMW_PRINT_BOOL(bBool)			fprintf(fp,"%d\n",m->bBool);

	FILE* fp = fopen(szOutFile,"wt");
	if (fp == NULL) return -1;
	
	_MFMW_PRINT_STRING	(	szFileName			);
	_MFMW_PRINT_STRING	(	szFileIn			);
	_MFMW_PRINT_STRING	(	szFileOut			);
	_MFMW_PRINT_STRING	(	szFileWork			);
	_MFMW_PRINT_STRING	(	szScanDirectory		);
	_MFMW_PRINT_INT		(	iMailSource			);
	_MFMW_PRINT_INT		(	iMailSize			);
	_MFMW_PRINT_INT		(	iMailTimestamp		);
	_MFMW_PRINT_INT		(	iNumOfAttachments	);
	_MFMW_PRINT_INT		(	iTotalAttachmentSize);
	_MFMW_PRINT_STRING	(	szProblemMailDestination	);
	_MFMW_PRINT_STRING	(	szEnvelopeFrom		);
	_MFMW_PRINT_STRING	(	szEnvelopeRcpt		);
	_MFMW_PRINT_STRING	(	szMailFrom			);
	_MFMW_PRINT_STRING	(	szMailRcpt			);
	_MFMW_PRINT_STRING	(	szMailCC			);
	_MFMW_PRINT_STRING	(	szMailSubject		);
	_MFMW_PRINT_BOOL	(	bHaveFrom			);
	_MFMW_PRINT_BOOL	(	bHaveRcpt			);
	_MFMW_PRINT_BOOL	(	bHaveCC				);
	_MFMW_PRINT_BOOL	(	bHaveSubject		);
	_MFMW_PRINT_STRING	(	szErrorMessage		);
	_MFMW_PRINT_BOOL	(	bFilterMatched		);
	_MFMW_PRINT_INT		(	iFilterNotify		);
	_MFMW_PRINT_INT		(	iFilterAction		);
	_MFMW_PRINT_INT		(	iFilterHandle		);
	_MFMW_PRINT_BOOL	(	bSchedule			);
	_MFMW_PRINT_INT		(	iPriority			);
//	_MFMW_PRINT_INT		(	iBlacklistResults	);
	_MFMW_PRINT_STRING	(	szReceivedFrom		);
	_MFMW_PRINT_BOOL	(	bHaveReceivedFrom	);
	_MFMW_PRINT_BOOL	(	bCopy				);
	
	int iListCount;
	iXList_Storage* ixlist_storage;

	iListCount = 0;
	ixlist_storage = m->lstCopies->GetFirst();
	while (ixlist_storage != NULL) { ++iListCount; ixlist_storage = ixlist_storage->next; } 	// now write every single entry out
	fprintf(fp,"%d\n",iListCount);																// write it
	ixlist_storage = m->lstCopies->GetFirst();
	while (ixlist_storage != NULL) { fprintf(fp,"%s\n%s\n",ixlist_storage->name,ixlist_storage->data); ixlist_storage = ixlist_storage->next; } // now write every single entry out


MFD_Out(MFD_SOURCE_MAIL,"wrote %d items\n",iListCount);

#undef _MFMW_PRINT_STRING
#undef _MFMW_PRINT_INT
#undef _MFMW_PRINT_BOOL

	fclose(fp);

	return 0;
}

MailFilter_MailData* MailFilter_MailRead(char* szInFile) {

	char szTemp[4000];
	char szTemp2[4000];
	unsigned int ilen;
	FILE* fp = fopen(szInFile,"rt");
	if (fp == NULL) return NULL;

	MailFilter_MailData* m = MailFilter_MailInit(NULL,0);
	if (m == NULL)
	{
		MF_DisplayCriticalError("MAILFILTER: EEEK! MailInit returned NULL! TRACECHECK 4001\n");
		fclose(fp);
		return NULL;
	}

#define _MFMW_READ_STRING(szString,iSize)	m->szString[0]=0; if(fgets(m->szString,iSize-2,fp) != NULL) { ilen=strlen(m->szString); if ((ilen>0)&&(m->szString[ilen-1] == '\n')) { ilen--; m->szString[ilen]=0; } if ((ilen>0)&&(m->szString[ilen-1] == '\r')) m->szString[ilen-1]=0; }
#define _MFMW_READ_INT(iInt)				szTemp[0]=0; szTemp[1]=0; fgets(szTemp,80,fp); m->iInt=atol(szTemp);
#define _MFMW_READ_UL(ulULong)				szTemp[0]=0; szTemp[1]=0; fgets(szTemp,80,fp); m->ulULong=(unsigned long)atol(szTemp);
#define _MFMW_READ_BOOL(bBool)				szTemp[0]=0; szTemp[1]=0; fgets(szTemp,10,fp); atoi(szTemp) == 0 ? m->bBool = false : m->bBool = true;

	_MFMW_READ_STRING	(	szFileName			,MAX_PATH);
	_MFMW_READ_STRING	(	szFileIn			,MAX_PATH);
	_MFMW_READ_STRING	(	szFileOut			,MAX_PATH);
	_MFMW_READ_STRING	(	szFileWork			,MAX_PATH);
	_MFMW_READ_STRING	(	szScanDirectory		,MAX_PATH);
	_MFMW_READ_INT		(	iMailSource			);
	_MFMW_READ_INT		(	iMailSize			);
	_MFMW_READ_UL		(	iMailTimestamp		);
	_MFMW_READ_INT		(	iNumOfAttachments	);
	_MFMW_READ_INT		(	iTotalAttachmentSize);
	_MFMW_READ_STRING	(	szProblemMailDestination	,250);
	_MFMW_READ_STRING	(	szEnvelopeFrom		,250);
	_MFMW_READ_STRING	(	szEnvelopeRcpt		,250);
	_MFMW_READ_STRING	(	szMailFrom			,250);
	_MFMW_READ_STRING	(	szMailRcpt			,250);
	_MFMW_READ_STRING	(	szMailCC			,250);
	_MFMW_READ_STRING	(	szMailSubject		,2000);
	_MFMW_READ_BOOL		(	bHaveFrom			);
	_MFMW_READ_BOOL		(	bHaveRcpt			);
	_MFMW_READ_BOOL		(	bHaveCC				);
	_MFMW_READ_BOOL		(	bHaveSubject		);
	_MFMW_READ_STRING	(	szErrorMessage		,2000);
	_MFMW_READ_BOOL		(	bFilterMatched		);
	_MFMW_READ_INT		(	iFilterNotify		);
	_MFMW_READ_INT		(	iFilterAction		);
	_MFMW_READ_INT		(	iFilterHandle		);
	_MFMW_READ_BOOL		(	bSchedule			);
	_MFMW_READ_INT		(	iPriority			);
//	_MFMW_READ_INT		(	iBlacklistResults	);
	_MFMW_READ_STRING	(	szReceivedFrom		,250);
	_MFMW_READ_BOOL		(	bHaveReceivedFrom	);
	_MFMW_READ_BOOL		(	bCopy				);

	int iListCount, i;
	
	szTemp[0]=0; fgets(szTemp,80,fp); iListCount=atol(szTemp);				// read number of array entries
MFD_Out(MFD_SOURCE_MAIL,"mfmail: will read %d items\n",iListCount);

	for (i = 0; i < iListCount; i++)
	{
		fgets(szTemp,4000-2,fp);	szTemp[4000-1]=0; ilen=strlen(szTemp); if ((ilen>0)&&(szTemp[ilen-1] == '\n')) { ilen--; szTemp[ilen]=0; } if ((ilen>0)&&(szTemp[ilen-1] == '\r')) szTemp[ilen-1]=0;
		fgets(szTemp2,4000-2,fp);	szTemp2[4000-1]=0; ilen=strlen(szTemp2); if ((ilen>0)&&(szTemp2[ilen-1] == '\n')) { ilen--; szTemp2[ilen]=0; } if ((ilen>0)&&(szTemp2[ilen] == '\r')) szTemp2[ilen-1]=0;
		m->lstCopies->AddValueChar(szTemp,szTemp2);
	}

MFD_Out(MFD_SOURCE_MAIL,"mfmail: done reading items\n");

	iXList_Storage* ixlist_storage = m->lstCopies->GetFirst();
	while (ixlist_storage != NULL) { MFD_Out(MFD_SOURCE_MAIL,"read: %s %s\n",ixlist_storage->name,ixlist_storage->data); ixlist_storage = ixlist_storage->next; }
	
#undef _MFMW_READ_STRING
#undef _MFMW_READ_INT
#undef _MFMW_READ_UL
#undef _MFMW_READ_BOOL
MFD_Out(MFD_SOURCE_MAIL,"Read iTotalAttachmentSize = %u from MFS.\n",m->iTotalAttachmentSize);

	fclose(fp);
	return m;
}


MailFilter_MailData* MailFilter_MailInit(char* szFileName, int iMailSource)
{
	MailFilter_MailData* mail = new(MailFilter_MailData);
	
	mail->iMailSource				= iMailSource;
	mail->szFileName				= (char*)_mfd_malloc(MAX_PATH+1,"Init");	mail->szFileName				[0] = 0;
	mail->szFileIn					= (char*)_mfd_malloc(MAX_PATH+1,"Init");	mail->szFileIn					[0] = 0;
	mail->szFileOut					= (char*)_mfd_malloc(MAX_PATH+1,"Init");	mail->szFileOut					[0] = 0;
	mail->szFileWork				= (char*)_mfd_malloc(MAX_PATH+1,"Init");	mail->szFileWork				[0] = 0;
	mail->szScanDirectory			= (char*)_mfd_malloc(MAX_PATH+1,"Init");	mail->szScanDirectory			[0] = 0;
	mail->iMailSize					= 0;
	mail->iMailTimestamp			= 0;
	mail->iTotalAttachmentSize		= 0;
	mail->iNumOfAttachments			= 0;
	mail->szProblemMailDestination	= (char*)_mfd_malloc(252,"Init");			mail->szProblemMailDestination	[0] = 0;
	mail->szEnvelopeFrom			= (char*)_mfd_malloc(252,"Init");			mail->szEnvelopeFrom			[0] = 0;
	mail->szEnvelopeRcpt			= (char*)_mfd_malloc(252,"Init");			mail->szEnvelopeRcpt			[0] = 0;
	mail->szMailFrom				= (char*)_mfd_malloc(252,"Init");			mail->szMailFrom				[0] = 0;
	mail->szMailRcpt				= (char*)_mfd_malloc(252,"Init");			mail->szMailRcpt				[0] = 0;
	mail->szMailCC					= (char*)_mfd_malloc(252,"Init");			mail->szMailCC					[0] = 0;
	mail->szMailSubject				= (char*)_mfd_malloc(2002,"Init");			mail->szMailSubject				[0] = 0;
	mail->bHaveFrom					= false;
	mail->bHaveRcpt					= false;
	mail->bHaveCC					= false;
	mail->bHaveSubject				= false;
	mail->szErrorMessage			= (char*)_mfd_malloc(2002,"Init");			mail->szErrorMessage			[0] = 0;
	mail->bFilterMatched			= false;
	mail->iFilterNotify				= 0;
	mail->iFilterAction				= 0;
	mail->iFilterHandle				= 0;
	mail->bSchedule					= false;
	mail->iPriority					= 0;
//	mail->iBlacklistResults			= 0;
	mail->szReceivedFrom			= (char*)_mfd_malloc(252,"Init");			mail->szReceivedFrom			[0] = 0;
	mail->bHaveReceivedFrom			= false;
	mail->bCopy						= false;
		
	if (szFileName != NULL)
		strncpy(mail->szFileName,szFileName,MAX_PATH);
		
	if (
		(mail->szFileName == NULL) ||
		(mail->szFileIn == NULL) ||
		(mail->szFileOut == NULL) ||
		(mail->szFileWork == NULL) ||
		(mail->szScanDirectory == NULL) ||
		(mail->szProblemMailDestination == NULL) ||
		(mail->szEnvelopeFrom == NULL) ||
		(mail->szEnvelopeRcpt == NULL) ||
		(mail->szMailFrom == NULL) ||
		(mail->szMailRcpt == NULL) ||
		(mail->szMailCC == NULL) ||
		(mail->szMailSubject == NULL) ||
		(mail->szErrorMessage == NULL) ||
		(mail->szReceivedFrom == NULL)
		)
		{
			MF_DisplayCriticalError("MAILFILTER: MEMORY ALLOCATION ERROR!\n\tABEND IN A FEW SECONDS...\n\tGOOD ADVICE: SAVE AND LOG OUT NOW.\n");
			return NULL;
		}
		
	mail->lstAttachments = new iXList;
	mail->lstArchiveContents = new iXList;
	mail->lstCopies = new iXList;
	
	return mail;
}

int MailFilter_MailDestroy(MailFilter_MailData* mail)
{
	// free this in reverse order!
	delete(mail->lstCopies);							mail->lstCopies = NULL;
	delete(mail->lstArchiveContents);					mail->lstArchiveContents = NULL;
	delete(mail->lstAttachments);						mail->lstAttachments = NULL;

	if ( NULL != mail->szReceivedFrom			 )		_mfd_free ( mail->szReceivedFrom			,"Destroy"	);
	if ( NULL != mail->szErrorMessage			 )		_mfd_free ( mail->szErrorMessage			,"Destroy"	);
	if ( NULL != mail->szMailSubject			 )		_mfd_free ( mail->szMailSubject				,"Destroy"	);
	if ( NULL != mail->szMailCC					 )		_mfd_free ( mail->szMailCC					,"Destroy"	);
	if ( NULL != mail->szMailRcpt				 )		_mfd_free ( mail->szMailRcpt				,"Destroy"	);
	if ( NULL != mail->szMailFrom				 )		_mfd_free ( mail->szMailFrom				,"Destroy"	);
	if ( NULL != mail->szEnvelopeRcpt			 )		_mfd_free ( mail->szEnvelopeRcpt			,"Destroy"	);
	if ( NULL != mail->szEnvelopeFrom			 )		_mfd_free ( mail->szEnvelopeFrom			,"Destroy"	);
	if ( NULL != mail->szProblemMailDestination	 )		_mfd_free ( mail->szProblemMailDestination	,"Destroy"	);
	if ( NULL != mail->szScanDirectory			 )		_mfd_free ( mail->szScanDirectory			,"Destroy"	);
	if ( NULL != mail->szFileWork				 )		_mfd_free ( mail->szFileWork				,"Destroy"	);
	if ( NULL != mail->szFileOut				 )		_mfd_free ( mail->szFileOut					,"Destroy"	);
	if ( NULL != mail->szFileIn					 )		_mfd_free ( mail->szFileIn					,"Destroy"	);
	if ( NULL != mail->szFileName				 )		_mfd_free ( mail->szFileName				,"Destroy"	);
	
	delete(mail);
	
	return 0;
}

/*++eof++*/
