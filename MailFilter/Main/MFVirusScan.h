/*
 +
 + MFVirusScan.h
 + Copyright 2004 Christian Hofstaedtler.
 */

typedef enum mimeEncodingType {
  mimeEncodingBase64, mimeEncodingQuotedPrintable, mimeEncodingUUEncode
} mimeEncodingType;


long MFVS_DecodeAttachment(const char* szScanFile, FILE* mailFile, mimeEncodingType encodingType);
long MFUtil_EncodeFile(const char* szInFile, const char* szOutFile, mimeEncodingType encodingType);
long MFVS_MakeScanPath (char* buffer, unsigned int buflen);

long MFVS_CheckAttachment(const char* szAttFile, const char* szAttachmentFilename, MailFilter_MailData* m);
long MFVS_CheckZIPFile(const char* szAttFile, std::string szZipFilename, MailFilter_MailData* m);
long MFVS_CheckWinmailDat(const char* szAttFile, std::string szTNEFFilename, MailFilter_MailData* m);

/* eof */

