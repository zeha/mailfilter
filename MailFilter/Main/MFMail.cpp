/*
 +
 +		MFMail.cpp
 +		
 +		Mail Data Structure + Parsing Functions
 +		
 +		
 +		
 */

#define _MFD_MODULE		"MFMail.cpp"
#include <MailFilter.h>
#include "MFMail.h++"
#include "MFVirusScan.h"
#include "MFConfig-defines.h"

int MailFilter_MailWrite(const char* szOutFile, MailFilter_MailData* m) {

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
	_MFMW_PRINT_BOOL	(	bBrokenMessage		);
	_MFMW_PRINT_BOOL	(	bPartialMessage		);
	
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

MailFilter_MailData* MailFilter_MailRead(const char* szInFile) {

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
	_MFMW_READ_BOOL		(	bBrokenMessage		);
	_MFMW_READ_BOOL		(	bPartialMessage		);

	int iListCount, i;
	
	szTemp[0]=0; fgets(szTemp,80,fp); iListCount=atol(szTemp);				// read number of array entries
MFD_Out(MFD_SOURCE_MAIL,"mfmail: will read %d items... ",iListCount);

	for (i = 0; i < iListCount; i++)
	{
		fgets(szTemp,4000-2,fp);	szTemp[4000-1]=0; ilen=strlen(szTemp); if ((ilen>0)&&(szTemp[ilen-1] == '\n')) { ilen--; szTemp[ilen]=0; } if ((ilen>0)&&(szTemp[ilen-1] == '\r')) szTemp[ilen-1]=0;
		fgets(szTemp2,4000-2,fp);	szTemp2[4000-1]=0; ilen=strlen(szTemp2); if ((ilen>0)&&(szTemp2[ilen-1] == '\n')) { ilen--; szTemp2[ilen]=0; } if ((ilen>0)&&(szTemp2[ilen] == '\r')) szTemp2[ilen-1]=0;
		m->lstCopies->AddValueChar(szTemp,szTemp2);
	}

MFD_Out(MFD_SOURCE_MAIL,"done.\n");

	iXList_Storage* ixlist_storage = m->lstCopies->GetFirst();
	while (ixlist_storage != NULL) { MFD_Out(MFD_SOURCE_MAIL,"read: %s %s\n",ixlist_storage->name,ixlist_storage->data); ixlist_storage = ixlist_storage->next; }
	
#undef _MFMW_READ_STRING
#undef _MFMW_READ_INT
#undef _MFMW_READ_UL
#undef _MFMW_READ_BOOL

	fclose(fp);
	return m;
}


MailFilter_MailData* MailFilter_MailInit(const char* szFileName, int iMailSource)
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
	mail->bBrokenMessage			= false;
	mail->bPartialMessage			= false;
		
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
			MF_DisplayCriticalError("MAILFILTER: MEMORY ALLOCATION ERROR!\n\tGOOD ADVICE: SAVE AND LOG OUT NOW.\n");
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

/***************************************************************************************************************
   Mail Handling functions
 ***************************************************************************************************************/

//
// Examines Mail Files ...
//
int MF_ParseMail(MailFilter_MailData* m, bool bMiniMode)
{

	FILE *mailFile				= NULL;
	int q						= 255;
	int curChr					= 0;
	int curPos					= 0;
	int curCmpPos				= 0;
	int iFirstNonSpaceCharacter = 0;
	long mynewfilterhandle		= 0;
	char *pszFirstNonSpaceCharacter;
	
	int filterNotify			= MAILFILTER_NOTIFICATION_NONE;
	int filterAction			= MAILFILTER_MATCHACTION_NONE;

	char* szThisAttachment		= NULL;	
	char* szScanBuffer			= NULL;
	char* szCmpBuffer			= NULL;
	bool  bNextLineStartSpace	= false;
	char  cNextLineStart		= 0;

	long iThisAttSize = 0;

	bool bDecodeDone = false;
	bool bDecodeExec = false;
	bool bInMimeAttachment = false;
	bool bRelayChecked = false;
//	MimeDecoderData* mimeDecoder = NULL;
	
	mailFile = fopen(m->szFileWork,"rb");
	if (mailFile == NULL)
	{
		MFD_Out(MFD_SOURCE_ERROR,"fopen for %s failed.\n",m->szFileWork);
		MF_StatusText("Examine: Could not open file!");
		return 240;
	}

#define _EXAMINEFILE_INITSTRING(szString,size)		if (szString == NULL) { szString=(char*)_mfd_malloc(size+2,"ExaSBuf"); if (szString == NULL) return 239; } memset(szString,0,size);
	_EXAMINEFILE_INITSTRING (	szScanBuffer 		,	2002 );
	_EXAMINEFILE_INITSTRING (	szCmpBuffer			,	2502 );
	_EXAMINEFILE_INITSTRING (	szThisAttachment 	,	300  );

	sprintf(szScanBuffer,"STRING INIT");
	// go to top of file
	fseek(mailFile,0,SEEK_SET);

#undef _EXAMINEFILE_INITSTRING

	while (q == 255)
	{
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		if ( feof(mailFile) )
		{	q=0;	break;	}

		// reinit variables
		szScanBuffer[0]=0;
		iFirstNonSpaceCharacter = 0;

		// Get Line ...
		curPos = -1;
		do {
			curChr = 0;
			szScanBuffer[2000] = 0;
			
			if (bNextLineStartSpace)
			{
//			MFD_Out(MFD_SOURCE_MAIL,"bNLSS ");
				while ( (curChr = fgetc(mailFile) ) < 20 )
				{
					if (curChr == 0x9)
						curChr = 0x20;

					if (curChr == -1)
					{
						curChr = 0;
						break;
					}
						
					if (curChr != 0x20)
						curChr = 0;

					ThreadSwitch();
				}
								
				if ( !feof(mailFile) )
				if (curChr != 0)
					ungetc(curChr,mailFile);
				
				ungetc(0x20,mailFile); // push back in a space!
					
//				MFD_Out(MFD_SOURCE_MAIL,"got %d\n",curChr);
			} else {
				if (bInMimeAttachment)
				{
					curChr = fgetc(mailFile);
					ungetc(curChr,mailFile);
					if (curChr != 'C')
					{
						if ((MF_GlobalConfiguration->EnableAttachmentDecoder) && (!bMiniMode))
						{
							if ( !feof(mailFile) )
								ungetc('\n',mailFile);	// go one back for decode()
							// Extract Attachment
							iThisAttSize = 0;
							m->iNumOfAttachments++;
							
							char szNamFile[MAX_PATH]; sprintf(szNamFile,"%s%i.mfn",m->szScanDirectory,m->iNumOfAttachments);
							char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments);
							char szZipFile[MAX_PATH]; sprintf(szZipFile,"%sFiles.zip",m->szScanDirectory);

							
							FILE* namFile = fopen(szNamFile,"wt");
							if (namFile != NULL)
							{
								fprintf(namFile,"%s\n",szThisAttachment);
								fclose(namFile);
							} else {
								MFD_Out(MFD_SOURCE_MAIL, "Unable (%d) to fOpen %s!\n",errno,szNamFile);
								MF_StatusText("Error while decompressing file.");
							}
							
							iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingBase64);
#ifndef MAILFILTER_VERSION_YESITCRASHES
							if (iThisAttSize == 0)
							{	// try with uuencode
								iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingUUEncode);
							}
							if (iThisAttSize == 0)
							{	// try with qp
								iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingQuotedPrintable);
							}
#endif

							MFD_Out(MFD_SOURCE_VSCAN," * Attachment done\n");

							if (iThisAttSize > 0)
							{
								m->iTotalAttachmentSize += iThisAttSize;
								MFVS_CheckAttachment(szAttFile,szThisAttachment,m);
							}
								
							MFD_Out(MFD_SOURCE_VSCAN,"New Total Size: %d (increased by %d)\n",m->iTotalAttachmentSize,iThisAttSize);

#ifdef MF_WITH_ZIP
							MFZip* zip = new MFZip( szZipFile , 9 , 0 );
							zip->AddFile( szThisAttachment , szAttFile );
							delete(zip);							
#endif
						}
								
						bInMimeAttachment = false;
					}
				}
			}
//MF_DisplayCriticalError((" XNLX");
			while ((curChr != '\n') && (curChr != -1))
			{
				if (curPos >= 2000) break;
				curChr = fgetc(mailFile);

//	MF_DisplayCriticalError((" %c[%d/%d] ",curChr,curChr,curPos);
				curPos++;

				if ((curChr == '\n') || (curChr == -1))
					break;
				if ( curChr == 0x9 )				// Tab to Single Space
					curChr = 0x20;
				if ( (curChr != '\r') && (curChr != '\n'))
					szScanBuffer[curPos]=(char)curChr;

				ThreadSwitch();
			}
			
			if (curPos>0)
				curPos--;
				else
				curPos=0;

			szScanBuffer[curPos]=0;
			szScanBuffer[2000]=0;

			
			do
			{
				cNextLineStart = (char)fgetc(mailFile);
					
				if (cNextLineStart == -1)
					break;
					
					
			} while ( ( cNextLineStart == '\n' ) || ( cNextLineStart == '\r' ) );

if ( feof(mailFile) )
{	q=0;	break;	}

			// put char back...
			if (cNextLineStart != -1)
				ungetc(cNextLineStart,mailFile);

			switch (cNextLineStart)
			{
			case 0x9:
				bNextLineStartSpace = true;
				break;
			case 0x20:
				bNextLineStartSpace = true;
				break;
			case -1:	// fall through
			default:
				bNextLineStartSpace = false;
				break;	// end
			}

//MF_DisplayCriticalError((" XNLS:%d ",bNextLineStartSpace);
			
			if (bNextLineStartSpace)
				curPos--;
			
		} while (bNextLineStartSpace && (curPos<2000));
		szScanBuffer[2000]=0;
		//MFD_Out(MFD_SOURCE_MAIL,"(%d):%s\n",strlen(szScanBuffer),szScanBuffer);
		//for (int i = 0; i < 10; i++)
		//	MFD_Out(MFD_SOURCE_SMTP,"[%d/%d] ",szScanBuffer[i],i);

		ThreadSwitch();

		// looks confusing, it is! some things below check the first character of the next line...
		cNextLineStart = (char)fgetc(mailFile);
		ungetc(cNextLineStart,mailFile);

		char *szMimeDest;
#define _EXAMINEFILE_DOMIMEHDRDECODE(szBuffer,iLen) szMimeDest = MIME_DecodeMimeHeader(szBuffer); if (szMimeDest != NULL) { strncpy(szBuffer,szMimeDest,iLen); _mfd_free(szMimeDest,"ExaMimeDecode"); szMimeDest = NULL; szBuffer[iLen-1]=0; }

		switch (szScanBuffer[0])
		{
			case '-':
					bInMimeAttachment = false;
				break;
			case 'm':
			case 'M':
//				if (!m->bHaveFrom)	/* MAIL FROM: Filter - bHaveFrom wird nur =true gesetzt, wenn der Filter matcht, da dieses Feld nicht */
//				{					/* immer vorhanden ist, bzw. gültigen Inhalt hat (MAX!->admin@domain.com) */
					if ( memicmp(szScanBuffer,"mail from:",10) == 0 )
					{
						m->szEnvelopeFrom[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+10);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szEnvelopeFrom		, pszFirstNonSpaceCharacter , 245 );
						m->szEnvelopeFrom[245]=0;
						
						_EXAMINEFILE_DOMIMEHDRDECODE(m->szEnvelopeFrom,249);

					}
//				}
				break;
			case 'f':
			case 'F':
				if (!m->bHaveFrom)
				{
					if ( memicmp(szScanBuffer,"from:",5) == 0 )
					{
						m->bHaveFrom = true;
						m->szMailFrom[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+5);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailFrom , pszFirstNonSpaceCharacter , 245 );
						m->szMailFrom[245]=0;
						
						_EXAMINEFILE_DOMIMEHDRDECODE(m->szMailFrom,249);

					}
				}
				break;
			case 'r':
			case 'R':
//				if (!m->bHaveRcpt)		/* RCPT TO: Filter - bHaveRcpt wird nur =true gesetzt, wenn der Filter matcht, da dieses Feld nicht */
//				{					/* immer vorhanden ist, bzw. gültigen Inhalt hat */
					if ( memicmp(szScanBuffer,"rcpt to:",8) == 0 )
					{
						m->szEnvelopeRcpt[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+8);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szEnvelopeRcpt		, pszFirstNonSpaceCharacter , 245 );
						m->szEnvelopeRcpt[245]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(m->szEnvelopeRcpt,249);

					}
//				}

				/* also copy the (last) received: from header over */
				if ( ( memicmp(szScanBuffer,"received: from ",15) == 0 ) && ((!m->bHaveReceivedFrom) || (MF_GlobalConfiguration->EnablePFAFunctionality)) )
				{
//TODO
					// look for ip first
					// "agw.kmsv.de (asdf [213.69.145.18])"
					bool bFound = false;
					int iPos = 15;	// 15 = len("received from")
					// find [
					for (; iPos < 245; iPos++)
					{
						if (szScanBuffer[iPos] == '[')
						{
							++iPos;
							bFound = true;
							break;
						}
					}
	
					if (!bFound)
						iPos = 15; 		// ok, we didn't find an ip in [brackets], but we still can try the
										// reported name.
					
					// copy next 60 chars
					strncpy(szCmpBuffer,szScanBuffer+iPos,60);
					szCmpBuffer[59] = 0;

					// find ] and terminate string there.
					iFirstNonSpaceCharacter = 0;
					while ( szCmpBuffer[iFirstNonSpaceCharacter] != 0 )
					{
						if ( 
							(szCmpBuffer[iFirstNonSpaceCharacter] == ']') ||
							(szCmpBuffer[iFirstNonSpaceCharacter] == ')') ||
							(iscntrl(szCmpBuffer[iFirstNonSpaceCharacter])) || 
							(isspace(szCmpBuffer[iFirstNonSpaceCharacter]))
							)
						{
							szCmpBuffer[iFirstNonSpaceCharacter] = 0;
							break;
						}
						iFirstNonSpaceCharacter++;
						if (iFirstNonSpaceCharacter>59)
							break;
					}

					if (szCmpBuffer[0])
					{
						
						if (MF_GlobalConfiguration->EnablePFAFunctionality)
						{	
							/*
							 * Ok, now this is just for the f*cking Novell PFA
							 * It always writes: Received from: pfAgent blah...
							 * into the mail. this just suxxs for our DNS-RBL checking
							 * ==> skip over it (well, skip the first received from)
							 */
							if (!m->bHaveReceivedFrom)
							{
								m->bHaveReceivedFrom = true;
								strcpy(m->szReceivedFrom,"PFA");	// mark that we are entering this the first time
							} else {
								if (strncmp("PFA",m->szReceivedFrom,3) == 0)
								{
									strncpy( m->szReceivedFrom		, szCmpBuffer , 245 );					
									m->bHaveReceivedFrom = true;
									
									MFD_Out(MFD_SOURCE_MAIL,"Received from '%s'\n",m->szReceivedFrom);
								}
							}
						} else {
						
							strncpy( m->szReceivedFrom		, szCmpBuffer , 245 );					
							m->bHaveReceivedFrom = true;
							
							MFD_Out(MFD_SOURCE_MAIL,"Received from '%s'\n",m->szReceivedFrom);
						}						
					}
				}
				break;
			case 't':
			case 'T':
				if (!m->bHaveRcpt)
				{
					if ( memicmp(szScanBuffer,"to:",3) == 0 )
					{
						m->bHaveRcpt = true;
						m->szMailRcpt[0]=0;
						
						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+3);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailRcpt , pszFirstNonSpaceCharacter , 245 );
						m->szMailRcpt[245]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(m->szMailRcpt,249);
					}
				}
				break;
/*			case 'c':
			case 'C':
				// CC: is handled below...
				break;*/
			case 's':
			case 'S':
				if (!m->bHaveSubject)
				{
					if ( memicmp(szScanBuffer,"subject:",8) == 0 )
					{
						m->bHaveSubject = true;
						m->szMailSubject[0]=0;

						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+8);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailSubject , pszFirstNonSpaceCharacter , 1998 );
						m->szMailSubject[1999]=0;

						_EXAMINEFILE_DOMIMEHDRDECODE(m->szMailSubject,1999);
					}
				}
				break;
			case '*':
				if (memicmp(szScanBuffer,"** high priority **",19) == 0)
					m->iPriority = 1;
					else if (memicmp(szScanBuffer,"** low priority **",18) == 0)
						m->iPriority = -1;
				break;
			case 'b':
			case 'B':
				if ((memicmp(szScanBuffer,"begin",5) == 0) && (strlen(szScanBuffer) > 10))
				{
					szCmpBuffer[0]=0;
					
					// trim string ...
					iFirstNonSpaceCharacter = 0;	pszFirstNonSpaceCharacter = (szScanBuffer+9);
					while ( isspace(*pszFirstNonSpaceCharacter) )	pszFirstNonSpaceCharacter++;

					strncpy(szCmpBuffer,pszFirstNonSpaceCharacter,245);
					szCmpBuffer[245]=0;
MFD_Out(MFD_SOURCE_MAIL,"UU Attachment: '%s'\n",szCmpBuffer);
		
					strncpy(szThisAttachment,szCmpBuffer,250);
					m->lstAttachments->AddValueChar("UUEncode", szCmpBuffer);

/*
	MFVS_WriteOutAttachment
	(int (*output_fn) (const char *buf,  int size,  void *closure),
									void *closure);	
*/								
					if (MF_GlobalConfiguration->EnableAttachmentDecoder && (!bMiniMode))
					{
						// Extract Attachment
						m->iNumOfAttachments++;
						char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments);
/*						ungetc('b',mailFile);
						ungetc('e',mailFile);
						ungetc('g',mailFile);
	BUG TODO			ungetc('i',mailFile);
						ungetc('n',mailFile);		//stupid.
						ungetc(' ',mailFile);
						ungetc('6',mailFile);
						ungetc('0',mailFile);
						ungetc('0',mailFile);
						ungetc(' ',mailFile);
						ungetc('x',mailFile);
						ungetc(IX_CR,mailFile);
						ungetc(IX_LF,mailFile); */
						iThisAttSize = MFVS_DecodeAttachment(szAttFile,mailFile,mimeEncodingUUEncode);
						if (iThisAttSize > 0)
						{
//CHANGE
							m->iTotalAttachmentSize += iThisAttSize;
							MFVS_CheckAttachment(szAttFile,szCmpBuffer,m);
						}
					}
				}
				break;
			case 'c':
			case 'C':
				if (!m->bHaveCC)
				{
					if ( memicmp(szScanBuffer,"cc:",3) == 0 )
					{
						m->bHaveCC = true;
						m->szMailCC[0]=0;
						
						// trim string ...
						iFirstNonSpaceCharacter = 0;
						pszFirstNonSpaceCharacter = (szScanBuffer+3);

						while ( isspace(*pszFirstNonSpaceCharacter) )
							pszFirstNonSpaceCharacter++;
						
						strncpy( m->szMailCC , pszFirstNonSpaceCharacter , 245 );
						m->szMailCC[245]=0;

						break;
					}
				}
				if (memicmp(szScanBuffer,"content-",8) == 0)
				{
	
					curChr = 0;
					curPos = 8;
					if (memicmp(szScanBuffer+curPos,"disposition: attachment;",23) == 0)
					{

	MFD_Out(MFD_SOURCE_MAIL,"content-disposition: attachment\n");
							curPos = curPos+24;
					
							// example: Disposition: attachment; filename="xxxxxx.exe"
							// Liest die Zeichen bis 'name=' ( also bis '=' )...
							curChr = 1;
							while ((!feof(mailFile)) && (curChr != '=') && (curChr != '\r') && (curChr != '\n') && (curPos < 2000) && (curChr != '\0'))
							{
								curChr = szScanBuffer[curPos];
								curPos++;
							}

#ifdef N_PLAT_NLM
							ThreadSwitch();
#endif
							if ( (curChr == '\r') || (curChr == '\n') )
							{	
								MFD_Out(MFD_SOURCE_MAIL,"Break on \\r | \\n \n");
								break;
							}

							// is this is a filename ?
							if (memicmp(szScanBuffer+curPos-5,"name=",5) == 0)
							{

								szCmpBuffer[0] = 1;
								szCmpBuffer[1] = 0;
								curCmpPos = 0;

								if ( szScanBuffer[curPos] == '"' )	 //"
									curPos++;

#ifdef N_PLAT_NLM
								ThreadSwitch();
#endif
								do
								{

									szCmpBuffer[curCmpPos] = szScanBuffer[curPos];
									if (curPos > 2000)
									{
										// Abbruch wenn Attachment Name > Buffer
										strcpy(m->szErrorMessage,"Attachment Name was too long to fit into buffer.");
										m->iFilterNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
										m->iFilterAction = MAILFILTER_MATCHACTION_DROP_MAIL;
										m->bFilterMatched = true;
										break;
									}			
									//  || ( szCmpBuffer[curCmpPos] == '=' )
									if ((szCmpBuffer[curCmpPos] == '\r') || (szCmpBuffer[curCmpPos] == '\n') || ( szCmpBuffer[curCmpPos] == '\0' ))
										break;

									curCmpPos++;
									curPos++;

									szCmpBuffer[curCmpPos] = '*';
									szCmpBuffer[curCmpPos+1] = 0;

								}
								while ((szCmpBuffer[curCmpPos] != '\n') && (szCmpBuffer[curCmpPos] != '\r')  && (curPos < 2000) && (szCmpBuffer[curCmpPos] != '\0'));
								// && (szCmpBuffer[curCmpPos] != '=')

								if (curCmpPos > 0)
								{
								
									curCmpPos--;
									curPos--;

									// Now we have a full attachment name.
									szCmpBuffer[curCmpPos+1]=0;
									if ( szCmpBuffer[curCmpPos] == '"' )	  //"
										szCmpBuffer[curCmpPos]=0;

	MFD_Out(MFD_SOURCE_MAIL,"--> ATT '%s' ",szCmpBuffer);
									strncpy(szThisAttachment,szCmpBuffer,250);
									szThisAttachment[250] = 0;

									_EXAMINEFILE_DOMIMEHDRDECODE(szThisAttachment,249);
	MFD_Out(MFD_SOURCE_MAIL,"-=> ATT '%s'\n",szThisAttachment);
									if (strlen(szThisAttachment) > 0)
										m->lstAttachments->AddValueChar("Base64", szThisAttachment);
										else
										m->lstAttachments->AddValueChar("Base64", szCmpBuffer);



	/*
		MFVS_WriteOutAttachment
		(int (*output_fn) (const char *buf,  int size,  void *closure),
										void *closure);	
	*/								
									bInMimeAttachment = true;
								} else {
									MFD_Out(MFD_SOURCE_GENERIC,"WAH: could not decode c-d:a filename= filename correctly. maybe empty or something like that.\n");
									MF_StatusText("  WARNING: could not decode attachment filename [A]");
								}
								
							} // name= matching

					} else {		// Disposition , Type

						if (memicmp(szScanBuffer+curPos,"type",4) == 0)
						{
							{
								// see if this could be Content-Type: message/partial; total=...
								iFirstNonSpaceCharacter = 0;	pszFirstNonSpaceCharacter = (szScanBuffer+13);
								while ( isspace(*pszFirstNonSpaceCharacter) )	pszFirstNonSpaceCharacter++;

								MFD_Out(MFD_SOURCE_MAIL,"%s\n",pszFirstNonSpaceCharacter);
								if (memicmp(pszFirstNonSpaceCharacter,"message/partial",15) == 0)
									m->bPartialMessage = true;
							}
						
							// ex.: ": application/zip; name=\""
							curPos = 12;
							// we're looking for '='
							while ((!feof(mailFile)) && (curChr != '=') && (curChr != '\r') && (curChr != '\n') && (curPos < 2000))
							{
								curChr = szScanBuffer[curPos];
								curPos++;
							}
							ThreadSwitch();

							// is this is a filename ?
							if (memicmp(szScanBuffer+curPos-5,"name=",5) == 0)
							{
	MFD_Out(MFD_SOURCE_MAIL,"content-type name=\n");
	
								szCmpBuffer[0] = 0;
								curChr = 0;
								curCmpPos = 0;

								if ( szScanBuffer[curPos] == '"' )	// "
									curPos++;

								ThreadSwitch();
								while ((szCmpBuffer[curCmpPos] != '\n') && (szCmpBuffer[curCmpPos] != '\r') && (szCmpBuffer[curCmpPos] != '"')  && (szCmpBuffer[curCmpPos] != '=') && (curPos < 2000)) //"
								{
									szCmpBuffer[curCmpPos] = szScanBuffer[curPos];
									
									if (curPos > (MAX_PATH-2))
									{
										// Abbruch wenn Attachment Name > Buffer
										strcpy(m->szErrorMessage,"Attachment Name was too long to fit into buffer.");
										m->iFilterNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
										m->iFilterAction = MAILFILTER_MATCHACTION_DROP_MAIL;
										m->bFilterMatched = true;
										break;
									}
									
									if ((szCmpBuffer[curCmpPos] == '\0') || (szCmpBuffer[curCmpPos] == '\r') || (szCmpBuffer[curCmpPos] == '\n') || ( szCmpBuffer[curCmpPos] == '"' ))
										break;
// " //  <-- stupid highlighting in the IDE
									curCmpPos++;
									curPos++;

								}
								// Now we have a full attachment name.
								szCmpBuffer[curCmpPos]=0;
MFD_Out(MFD_SOURCE_MAIL,"--> TYPE '%s' ",szCmpBuffer);

								strncpy(szThisAttachment,szCmpBuffer,250);
								szThisAttachment[250] = 0;
								_EXAMINEFILE_DOMIMEHDRDECODE(szThisAttachment,249);
MFD_Out(MFD_SOURCE_MAIL,"-=> TYPE '%s'\n",szThisAttachment);
								if (strlen(szThisAttachment) > 0)
									m->lstAttachments->AddValueChar("Base64", szThisAttachment);
									else
									m->lstAttachments->AddValueChar("Base64", szCmpBuffer);
									

								bInMimeAttachment = true;
								
							}	// name=
						}

						ThreadSwitch();
					}
				}

				// 'c' 'C'
				break;

			default:
				// Nothing ... 
				break;

		}	// end of switch()
		
		/* Break Codes (q):
		
		   q = 0 		...		EOF == Fertig!
		   q = 1		... 	Filter Match
		   q > 1 < 255  ...		Fehler !?
		
		*/
		
		// Terminate While Loop if q>0			** FEHLER **
		if ( ( q>0 ) && ( q<255 ) )
		{
			break;
		}
		
/*		if (m->bFilterMatched && m->bHaveSubject && m->bHaveRcpt && m->bHaveFrom)
		{
			q = 1;
			break;
		}*/

	}	// end of while

	ThreadSwitch();

	fseek(mailFile,0,SEEK_END);
	m->iMailSize = ftell(mailFile);
	fclose(mailFile);
	
	if (szThisAttachment != NULL) _mfd_free(szThisAttachment,"ExaSBuf1");
	if (szScanBuffer != NULL) _mfd_free(szScanBuffer,"ExaSBuf2");
	if (szCmpBuffer  != NULL) _mfd_free(szCmpBuffer, "ExaSBuf3");

	return q;

}

/*++eof++*/


