
typedef void* HANDLE;

typedef struct {
		int Debug;
		int eTrust_InitComplete;
		HANDLE eTrust_hInoScan;
		HANDLE eTrust_hLibInoScan;
		HANDLE eTrust_hLibLibWinC;
} appdata_t;

extern "C" {
	appdata_t *GetOrSetAppData( void );
	int DisposeAppData(void* );
}


#define ETRUST7_MAGIC 0xE17051

int eTrust7_Init(MFAVA_HANDLE &hAVA);
int eTrust7_ScanFile(MFAVA_HANDLE hAVA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType);
int eTrust7_DeInit(MFAVA_HANDLE hAVA);
int eTrust7_PreInit();


extern void				*gModuleHandle;
extern int				gDebugSetCmd;
