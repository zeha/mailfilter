/*
 +
 + MFVirusScan.h
 + Copyright 2004 Christian Hofstaedtler.
 */
 
#define _MFD_MODULE "MFVirusScan.cpp"
#include "MailFilter.h"
#include "MFMail.h++"
#include "MFUnZip.h"
#include "MFVirusScan.h"

#include "../Included/mime/modmimee.h"
#include <tnef-lib.h>

long MFVS_MakeScanPath (char* buffer, unsigned int buflen)
{
	char szScanDir[MAX_PATH];
	char szScanFile[MAX_PATH];

	int iScanDir;
	bool bScanDir = false;
	for (iScanDir = 0; iScanDir < MF_GlobalConfiguration.MailscanDirNum; iScanDir++)
	{
		// Check if Scanner Status File exists...
		sprintf(szScanFile,"%s"IX_DIRECTORY_SEPARATOR_STR"MFSCAN"IX_DIRECTORY_SEPARATOR_STR"%04i"IX_DIRECTORY_SEPARATOR_STR"MAILFLT.MFS",MF_GlobalConfiguration.MFLTRoot.c_str(),iScanDir);
		if (access(szScanFile,F_OK))
		{
			// OK, Scanner Status File NOT found
			sprintf(szScanDir,"%s"IX_DIRECTORY_SEPARATOR_STR"MFSCAN"IX_DIRECTORY_SEPARATOR_STR"%04i"IX_DIRECTORY_SEPARATOR_STR,MF_GlobalConfiguration.MFLTRoot.c_str(),iScanDir);
			
			// Check if Lock file exists...
			sprintf(szScanFile,"%sMAILFLT.LCK",szScanDir);
			if (access(szScanFile,F_OK))
			{
				// OK, ScanDir is free for use
				bScanDir = true;
				break;
			}
		}
	}
	if (bScanDir)
	{
		strncpy(buffer,szScanDir,buflen);
		return 0;
	}
		else
		return 1;
}

long MFVS_CheckAttachment(const char* szAttFile, const char* szAttachmentFilename, MailFilter_MailData* m)
{
	long iNumAdditionalAttachments = 0;
	std::string szAttachmentFilename2;

	szAttachmentFilename2 = szAttachmentFilename;
	transform (szAttachmentFilename2.begin(),szAttachmentFilename2.end(), szAttachmentFilename2.begin(), (char(*)(int))tolower);

	iNumAdditionalAttachments = MFVS_CheckWinmailDat(szAttFile, szAttachmentFilename2, m);
	if (iNumAdditionalAttachments != 0)
		return iNumAdditionalAttachments;

	iNumAdditionalAttachments = MFVS_CheckZIPFile(szAttFile, szAttachmentFilename2, m);
	if (iNumAdditionalAttachments != 0)
		return iNumAdditionalAttachments;
		
	return 0;
}

long MFVS_CheckZIPFile(const char* szAttFile, std::string szZipFilename, MailFilter_MailData* m)
{
	std::string szZipExt;
	szZipExt = szZipFilename.substr(szZipFilename.length()-4);
	long bytes = 0;
	
	if (szZipExt == ".zip")
	{
		MFUnZip unzip(szAttFile);
	
		iXList* zipAttachments = unzip.ReadZipContents();
		if (zipAttachments != NULL)
		{
			MFD_Out(MFD_SOURCE_MAIL,"MFVS: ZIP: ");
		
			iXList_Storage* att = zipAttachments->GetFirst();
			while (att != NULL)
			{
				if (att->name[0]==0)
					break;
					
				// weird stuff.
				m->lstArchiveContents->AddValueBool(att->name,(bool)att->data);
				MFD_Out(MFD_SOURCE_MAIL, "%s ", att->name );
				
				// try to extract the file.
				char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments+1);
				bytes = unzip.ExtractFile(att->name,szAttFile);
				if (bytes>0)
				{
					m->iNumOfAttachments++;
					m->iTotalAttachmentSize += bytes;
					
					// write mfn
					sprintf(szAttFile,"%s%i.mfn",m->szScanDirectory,m->iNumOfAttachments);
					FILE* f = fopen(szAttFile,"w");
					if (f)
					{
						fprintf(f,"%s\\%s\n",szZipFilename.c_str(),att->name);
						fclose(f);
					} else {
						MFD_Out(MFD_SOURCE_ERROR,"MFVS: ZIP: Error writing %d.mfn\n",m->iNumOfAttachments);
					}

				} else {
					MFD_Out(MFD_SOURCE_MAIL," --> Decoding failed for unknown reason (%d)\n",bytes);
				}
				
				att = zipAttachments->GetNext(att);
			}
			MFD_Out(MFD_SOURCE_MAIL,"\n");
			delete(zipAttachments);
		}
	}
	
	return 0;
}

long MFVS_CheckWinmailDat(const char* szAttFile, std::string szTNEFFilename, MailFilter_MailData* m)
{
	struct LibTNEF_ClientData data;
	int iAttachments = 0;
	int type = 0;
	
	if (szTNEFFilename == "winmail.dat")
	{
	
		data.STRUCTSIZE = sizeof(struct LibTNEF_ClientData);
		data.tnef_stream = NULL;
		type = LibTNEF_Decode(szAttFile,&data);
		if (type < 0)
		{	MFD_Out(MFD_SOURCE_MAIL," -> decode() failed: %d\n",type);
			LibTNEF_Free(&data);
			return 0;
		}
		type = 0;

		type = LibTNEF_ReadNextAttribute(&data);
		while (type > 0)
		{
			switch (type)
			{
				case LIBTNEF_TYPE_ATTACHMENTNAME:
					MFD_Out(MFD_SOURCE_MAIL,"TNEF Decoder: AttName '%s'\n",data.szAttachmentName);
					m->lstAttachments->AddValueChar("MS-TNEF", data.szAttachmentName);
					iAttachments++;
					break;
				case LIBTNEF_TYPE_ATTACHMENTDATA:

					MFD_Out(MFD_SOURCE_MAIL," -> TNEF found attachment data\n");
					if (MF_GlobalConfiguration.EnableAttachmentDecoder)
					{
						MFD_Out(MFD_SOURCE_MAIL," -> TNEF decoding attachment\n");
						// Extract Attachment
						char szAttFile[MAX_PATH]; sprintf(szAttFile,"%s%i.att",m->szScanDirectory,m->iNumOfAttachments+1);
						if (LibTNEF_SaveAttachmentData(&data,szAttFile) == 0)
						{
							m->iNumOfAttachments++;
							if (data.lAttachmentSize > 0)
								m->iTotalAttachmentSize += data.lAttachmentSize;

/*
		TODO:
				get the attachment name from libtned and
				write our mfn file
				hm.
				
				
							// write mfn
							sprintf(szAttFile,"%s%i.mfn",m->szScanDirectory,m->iNumOfAttachments);
							FILE* f = fopen(szAttFile,"w");
							if (f)
							{
								fwrite(f,"%s\\%s\n",szZipFilename.c_str(),att->name);
								fclose(f);
							} else {
								MFD_Out(MFD_SOURCE_ERROR,"MFVS: ZIP: Error writing %d.mfn\n",m->iNumOfAttachments);
							}
*/
						} else {
							MFD_Out(MFD_SOURCE_MAIL," --> Decoding failed for unknown reason\n");
						}
					}

					break;
				default:
					break;
			}
			type = LibTNEF_ReadNextAttribute(&data);
		}
		LibTNEF_Free(&data);
		
		return iAttachments;
	
	} else {
		return 0;
	}
}

/*
	Decoder Callback Function:
	int (*output_fn) (const char *buf,  int size,  void *closure)
*/
static int MFVS_WriteOutAttachment ( const char *buf, int size, void *closure)
{
	if (closure == NULL)
		return 1;

	FILE* fp = (FILE*)closure;

	for (int cPos = 0; cPos<size; cPos++)
		fputc(buf[cPos],fp);

	return -size;
}

/*
	Encoder Callback Function:
	int (*output_fn) (const char *buf,  int size,  void *closure)
*/
static int MFVS_WriteOutEncodedStream ( const char *buf, int size, void *closure)
{
	if (closure == NULL)
		return 1;

	FILE* fp = (FILE*)closure;

	for (int cPos = 0; cPos<size; cPos++)
		fputc(buf[cPos],fp);

	return 0;
}


long MFVS_DecodeAttachment(const char* szScanFile, FILE* mailFile, mimeEncodingType encodingType)
{
//	if (!MFC_MAILSCAN_Enabled)
//		return 0;

	MimeDecoderData* mimeDecoder=	NULL; 
	bool bDecodeDone			=	false;
	bool bDecodeExec			=	false;
	bool bHaveDecoder			=	false;
	int curChr					=	0;
	int curPos					=	0;
	int lastPos					=	0;
	long rc						=	0;
	FILE* fAttFile				= 	NULL;	
	long iSize					= 	0;
	fAttFile = fopen(szScanFile,"wb");

	char* szScanBuffer;
	szScanBuffer				=	(char*)_mfd_malloc(2002,"Decode");
	if (szScanBuffer == NULL)		{ fclose(fAttFile); return -239; }
	
	MFD_Out(MFD_SOURCE_VSCAN,"Decoding to %s. ",szScanFile);
	
	if (encodingType == mimeEncodingBase64)
	{	// Initialize Base64 Decoder
		mimeDecoder = MimeB64DecoderInit(MFVS_WriteOutAttachment,(void*)fAttFile);
		if (mimeDecoder != NULL)	bHaveDecoder = true;
	}
	if (encodingType == mimeEncodingQuotedPrintable)
	{	// Initialize QuotedPrintable Decoder
		mimeDecoder = MimeQPDecoderInit(MFVS_WriteOutAttachment,(void*)fAttFile);
		if (mimeDecoder != NULL)	bHaveDecoder = true;
	}
	if (encodingType == mimeEncodingUUEncode)
	{	// Initialize UUEncode Decoder
		mimeDecoder = MimeUUDecoderInit(MFVS_WriteOutAttachment,(void*)fAttFile);
		if (mimeDecoder != NULL)	bHaveDecoder = true;
	}
	
	if (!bHaveDecoder)
	{	_mfd_free(szScanBuffer,"Decode");
		fclose(fAttFile);
		return -237;
	}
	
	// Read In lines and decode ...
	do {
		curPos = -1; curChr = 0;
		szScanBuffer[0] = 0; szScanBuffer[2000] = 0;
		while ((curChr != '\n') && (curChr != -1))
		{
			if (curPos >= 2000) break;
			curChr = fgetc(mailFile);
			curPos++;

			if (curChr == '\n')									break;
			if ((curChr != '\r') && (curChr != '\n'))			szScanBuffer[curPos]=(char)curChr;
			if (curChr == -1)									{ bDecodeDone = true; break; }
			if (feof(mailFile))									{ bDecodeDone = true; break; }
		}
		/* we do -1, because we want +1 in the mimedecoderwrite() call */
		if (curPos>0)	szScanBuffer[curPos-1]=0;
			else		{ curPos=0; szScanBuffer[0]=0; }
			
		if ( (encodingType == mimeEncodingBase64) && (curPos == 73) && (lastPos == 0) )
			bDecodeExec = true;

		for(rc=2001; rc>curPos; rc--)
			szScanBuffer[rc]=0;
			
		szScanBuffer[2000]=0;

		if (bDecodeExec)
		{
			
			if (MimeDecoderWrite (mimeDecoder, szScanBuffer, curPos) == 1)
				bDecodeDone = true;

			if ( (encodingType == mimeEncodingBase64) && (curPos < lastPos) )
				break;

			lastPos = curPos;
		}
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		if (szScanBuffer[0] == 0)
			bDecodeExec = true;
	} while (bDecodeDone == false);
	
	// Destroy Decoder
	MimeDecoderDestroy(mimeDecoder,false);
	mimeDecoder = NULL;

	iSize = ftell(fAttFile);
MFD_Out(MFD_SOURCE_VSCAN,"=> %d bytes\n",iSize);
	fclose(fAttFile);

	/*if (szScanBuffer != NULL)	*/_mfd_free(szScanBuffer,"Decode");

	if (iSize > 0)
		return iSize;
		else
		return 0;
}


long MFUtil_EncodeFile(const char* szInFile, const char* szOutFile, mimeEncodingType encodingType)
{

	MimeEncoderData* mimeCoder	=	NULL; 
	bool bCodeDone				=	false;
	bool bCodeExec				=	false;
	bool bHaveCoder				=	false;
	int curChr					=	0;
	int curPos					=	0;
	int lastPos					=	0;
	long rc						=	0;
	FILE* fInFile				= 	NULL;	
	FILE* fOutFile				= 	NULL;	
	long iSize					= 	0;
	
	
	fInFile  = fopen(szInFile, "rb");
	fOutFile = fopen(szOutFile,"wt");

	char* szScanBuffer;
	szScanBuffer				=	(char*)_mfd_malloc(2002,"Encode");
	if (szScanBuffer == NULL)		{ fclose(fInFile); fclose(fOutFile); return -239; }
	
	MFD_Out(MFD_SOURCE_VSCAN,"Encoding to %s\n",szOutFile);
	
	if (encodingType == mimeEncodingBase64)
	{	// Initialize Base64 Decoder
		mimeCoder = MimeB64EncoderInit(MFVS_WriteOutEncodedStream,(void*)fOutFile);
		if (mimeCoder != NULL)	bHaveCoder = true;
	}
	if (encodingType == mimeEncodingQuotedPrintable)
	{	// Initialize QuotedPrintable Decoder
		mimeCoder = MimeQPEncoderInit(MFVS_WriteOutEncodedStream,(void*)fOutFile);
		if (mimeCoder != NULL)	bHaveCoder = true;
	}
	if (encodingType == mimeEncodingUUEncode)
	{	// Initialize UUEncode Decoder
		mimeCoder = MimeUUEncoderInit("attach.uue",MFVS_WriteOutEncodedStream,(void*)fOutFile);
		if (mimeCoder != NULL)	bHaveCoder = true;
	}
	
	if (!bHaveCoder)
	{	_mfd_free(szScanBuffer,"Encode");
		fclose(fInFile); fclose(fOutFile);
		return -237;
	}
	
	// Read In lines and decode ...
	do {
		curPos = 0; curChr = 0;
		szScanBuffer[0] = 0; szScanBuffer[2000] = 0;

		curPos = (int)fread(szScanBuffer,sizeof(char),1999,fInFile);

		if (curPos == 0)									{ bCodeDone = true; }	/* do not break here */
		if (ferror(fInFile))								{ bCodeDone = true; }	/* do not break here */
		if (feof(fInFile))									{ bCodeDone = true; }	/* do not break here */

		/* we do -1, because we want +1 in the mimedecoderwrite() call */
/*		if (curPos>0)	szScanBuffer[curPos-1]=0;
			else		{ curPos=0; szScanBuffer[0]=0; }
			
		if ( (encodingType == mimeEncodingBase64) && (curPos == 73) && (lastPos == 0) )
			bDecodeExec = true;

		for(rc=2001; rc>curPos; rc--)
			szScanBuffer[rc]=0;
*/			
		szScanBuffer[2000]=0;

/*		if (bCodeExec)
		{
*/		
		int iWrote = 0;
//MF_DisplayCriticalError("write: %d chars, curPos=%d\n", strlen(szScanBuffer),curPos);
			iWrote = MimeEncoderWrite (mimeCoder, szScanBuffer, curPos);
//MF_DisplayCriticalError("-> %d\n",iWrote);
			
			if (iWrote == 1)
				bCodeDone = true;

			if(bCodeDone)
				MFD_Out(MFD_SOURCE_MAIL,"Done!?");
				
		
/*			if ( (encodingType == mimeEncodingBase64) && (curPos < lastPos) )
				break;

			lastPos = curPos;*/
//		}
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
	} while (bCodeDone == false);

	MFD_Out(MFD_SOURCE_MAIL,"\n");
MFD_Out(MFD_SOURCE_VSCAN,"** Done...\n");
	
	// Destroy Decoder
	MimeEncoderDestroy(mimeCoder,false);
	mimeCoder = NULL;

	iSize = ftell(fOutFile);
MFD_Out(MFD_SOURCE_VSCAN,"Encode made %d bytes\n",iSize);
	fclose(fInFile);
	fclose(fOutFile);

	/*if (szScanBuffer != NULL)	*/_mfd_free(szScanBuffer,"Encode");

	if (iSize > 0)
		return iSize;
		else
		return 0;
}

/*
static bool
MimeUntypedText_uu_begin_line_p(const char *line, int length, char **name_ret)
{
  const char *s;
  char *name = 0;

  if (name_ret) *name_ret = 0;

  if (strncmp (line, "begin ", 6)) return false;
  // ...then three or four octal digits. 
  s = line + 6;
  if (*s < '0' || *s > '7') return false;
  s++;
  if (*s < '0' || *s > '7') return false;
  s++;
  if (*s < '0' || *s > '7') return false;
  s++;
  if (*s == ' ')
	s++;
  else
	{
	  if (*s < '0' || *s > '7') return false;
	  s++;
	  if (*s != ' ') return false;
	}

  while (isspace(*s))
	s++;

  name = (char *) malloc(((line+length)-s) + 1);
  if (!name) return false; // grr... 
  memcpy(name, s, (line+length)-s);
  name[(line+length)-s] = 0;

  // take off newline. 
  if (name[strlen(name)-1] == IX_LF) name[strlen(name)-1] = 0;
  if (name[strlen(name)-1] == IX_CR) name[strlen(name)-1] = 0;

  if (name_ret)
	*name_ret = name;
  else
	free(name);

  return true;
}
*/

/* eof */
