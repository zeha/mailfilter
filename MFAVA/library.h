
typedef void* HANDLE;

typedef struct {
		int Debug;

		// eTrust support
		int eTrust_InitComplete;
		HANDLE eTrust_hInoScan;
		HANDLE eTrust_hLibInoScan;
		HANDLE eTrust_hLibLibWinC;

		// Sophos support
		int Sophos_InitComplete;
		
} appdata_t;

extern "C" {
	appdata_t *GetOrSetAppData( void );
	int DisposeAppData(void* );
}

extern void				*gModuleHandle;
extern int				gDebugSetCmd;


#define ETRUST7_MAGIC 0xE17051

int eTrust7_Init(MFAVA_HANDLE &hAVA);
int eTrust7_ScanFile(MFAVA_HANDLE hAVA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType);
int eTrust7_DeInit(MFAVA_HANDLE hAVA);
int eTrust7_PreInit();

#define SOPHOS_MAGIC 0x508605

int Sophos_Init(MFAVA_HANDLE &hAVA);
int Sophos_ScanFile(MFAVA_HANDLE hAVA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType);
int Sophos_DeInit(MFAVA_HANDLE hAVA);
int Sophos_PreInit();

