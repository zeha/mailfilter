/*
 +
 + MFVirusScan.h
 + Copyright 2004 Christian Hofstaedtler.
 */

typedef enum mimeEncodingType {
  mimeEncodingBase64, mimeEncodingQuotedPrintable, mimeEncodingUUEncode
} mimeEncodingType;


long MFVS_DecodeAttachment(char* szScanFile, FILE* mailFile, mimeEncodingType encodingType);
long MFUtil_EncodeFile(const char* szInFile, const char* szOutFile, mimeEncodingType encodingType);
long MFVS_MakeScanPath (char* buffer, unsigned int buflen);
long MFVS_CheckWinmailDat(char* szAttFile, char* szTNEFFilename, MailFilter_MailData* m);

/* eof */