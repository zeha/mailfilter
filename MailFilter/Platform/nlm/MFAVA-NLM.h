//
//  MFAVA-NLM.h
//

int MailFilter_AV_Init();
int MailFilter_AV_ScanFile(const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType);
int MailFilter_AV_DeInit();
int MailFilter_AV_Check();

// -eof-

