typedef void *MFAVA_HANDLE;

#ifndef ESUCCESS
#define ESUCCESS		0
#endif

#ifdef __cplusplus
extern "C" {
#endif

int MailFilter_AVA_Init(int iVirusScanner, int iDebug, MFAVA_HANDLE &hReturn);
int MailFilter_AVA_DeInit(MFAVA_HANDLE hVSA);
int MailFilter_AVA_ScanFile(MFAVA_HANDLE hVSA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType);
int MailFilter_AVA_Status(MFAVA_HANDLE hAVA, long whatToGet);

typedef int(*MailFilter_AVA_Init_t)(int, int, MFAVA_HANDLE&);
typedef int(*MailFilter_AVA_DeInit_t)(MFAVA_HANDLE);
typedef int(*MailFilter_AVA_ScanFile_t)(MFAVA_HANDLE,const char*,char*,size_t,int&);
typedef int(*MailFilter_AVA_Status_t)(MFAVA_HANDLE, long);



#ifdef __cplusplus
}

void* __my_malloc(size_t length);

#endif


