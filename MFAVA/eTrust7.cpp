#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netware.h>
#include <malloc.h>
#include <library.h>

#include "MFAVA.h"
#include "library.h"

typedef void *HMODULE;

typedef struct {
	long sig;
	char* szVirusName;
	int Debug;
	size_t iVirusNameLength;
	long lVirusType;
	long lErrorCode;
	long lExtErrorCode;
	long endSig;
} VirusData_t;


struct inoConfig
{
	long dwFileFilterType;
	char* pszExtList;
	char* pszExcludeExtList;
	long bScanArc;
	long bIsArcByExtension;		// scan arc by extension = 1, by contents = 0
	long dwArcTypesCount;
	long* pdwArcTypesList;
	long l3; // 1
	long l4; // 1
	long l5;
	long l6;
	long l7;
	long l8; // 1
	// 13x4 bytes.
} _inoConfig;
struct inoConfig2 {
	long l9; // 1
	long l10;
	long l11; // 2
	long l12; // 1
	long l13; // 2
	long dwSpecialMode; // Zusatz: 0x80 bei "Apply infection actions on archive file (excluding cure)"
	long l15;
	char* p1;
	char* p2;
	char* p3;
	char* p4;
	long lServiceState; // 0-4
	char* p5;
	char* p6;
	char* p7;
	char* p8;
	char* p9;
	long l17; // 0x00010000
	long l18;
	long dwShowDriveFlags;
	char AvEngineHome[260];			// sys:\etrustav\avengine
	char InoHome[260];				// sys:\etrustav
	char InoMove[260];				// sys:\etrustav\ino\move
	char InoMove2[260];				// sys:\etrustav\ino\move
	char RenameExtension[260];		// AVB
	char System[260];				// System
	

};

struct actionConfig{
	unsigned long action;
	char avEng[260];
	char avEng2[260];
	char padding[1000];
};

struct scanReturnData{
	unsigned long l1;			// immer 1?
	unsigned long lErrorCode;	// wenn ok = 0, sonst z.b. -200
	unsigned long lWin32ErrorCode;	// bei Virus 1, sonst Fehler Code (2 = 404)
	unsigned long lIsVirus;		// 0 = kein Virus, 1 = Virus
	unsigned long lVirusType;	// 0 = nix, 1 = Virus, 2 = Trojan, 3 = Macro Virus?, 4 = Worm
	char szFile[260];	//577
	char szUnknown[260];
	char szUnknown2[57];
	char szVirusName[64];
};




#ifdef __cplusplus
extern "C" {
#endif

	// ?
	void LoadAvDll();
	// Unload AVENGINE
	void UnLoadDll();

	// libwinc@
	HMODULE LoadLibrary(const char* szLibraryName);
	int FreeLibrary(HMODULE hDLLHandle);

	typedef HMODULE(*LoadLibrary_t)(const char*);
	typedef int(*FreeLibrary_t)(HMODULE);

	// memset 518 byte. with 0.

	// inoscan@
	int InoScanSetActionOption(struct actionConfig* cf);	// pass in 3
	int InoScanInit(void* ptr);
	int InoScanSetConfig(void* ptr, struct inoConfig* config, struct inoConfig2* config2, void* callbackFunc, unsigned char x);
	int InoScanScanIT(void* ptr, const char* szFileName, VirusData_t* virus);
	int InoScanDeInit(void* ptr);	// rc 0x137 = normal

	typedef int(*InoScanSetActionOption_t)(struct actionConfig*);
	typedef int(*InoScanInit_t)(void*);
	typedef int(*InoScanSetConfig_t)(void*, struct inoConfig*, struct inoConfig2*, void*, unsigned char);
	typedef int(*InoScanScanIT_t)(void*, const char* szFileName, VirusData_t* virus);
	typedef int(*InoScanDeInit_t)(void*);	// rc 0x137 = normal

	static LoadLibrary_t					LoadLibrary_sym = NULL;
	static FreeLibrary_t					FreeLibrary_sym = NULL;
	static InoScanSetActionOption_t			InoScanSetActionOption_sym = NULL;
	static InoScanInit_t					InoScanInit_sym = NULL;
	static InoScanSetConfig_t				InoScanSetConfig_sym = NULL;
	static InoScanScanIT_t					InoScanScanIT_sym = NULL;
	static InoScanDeInit_t					InoScanDeInit_sym = NULL;
	
#ifdef __cplusplus
}
#endif

static int eTrust7_UnImportSymbols()
{
	LoadLibrary_sym = NULL;
	FreeLibrary_sym = NULL;

	InoScanSetActionOption_sym = NULL;
	InoScanInit_sym = NULL;
	InoScanSetConfig_sym = NULL;
	InoScanScanIT_sym = NULL;
	InoScanDeInit_sym = NULL;

	UnImportPublicObject(getnlmhandle(),"libwinc@LoadLibrary");
	UnImportPublicObject(getnlmhandle(),"libwinc@FreeLibrary");

	UnImportPublicObject(getnlmhandle(),"inoscan@InoScanSetActionOption");
	UnImportPublicObject(getnlmhandle(),"inoscan@InoScanInit");
	UnImportPublicObject(getnlmhandle(),"inoscan@InoScanSetConfig");
	UnImportPublicObject(getnlmhandle(),"inoscan@InoScanScanIT");
	UnImportPublicObject(getnlmhandle(),"inoscan@InoScanDeInit");
	
	return ESUCCESS;
}
static int eTrust7_Check()
{
	if (
		(LoadLibrary_sym == NULL) ||
		(FreeLibrary_sym == NULL) ||
	
		(InoScanSetActionOption_sym == NULL) ||
		(InoScanInit_sym == NULL) ||
		(InoScanSetConfig_sym == NULL) ||
		(InoScanScanIT_sym == NULL) ||
		(InoScanDeInit_sym == NULL)
		)
	{
		return ENOENT;
	}
	return ESUCCESS;
}
static int eTrust7_ImportSymbols()
{
	eTrust7_UnImportSymbols();	// clean up before doing our new work

	LoadLibrary_sym = (LoadLibrary_t)ImportPublicObject(getnlmhandle(),"libwinc@LoadLibrary");
	FreeLibrary_sym = (FreeLibrary_t)ImportPublicObject(getnlmhandle(),"libwinc@FreeLibrary");

	InoScanSetActionOption_sym = (InoScanSetActionOption_t)ImportPublicObject(getnlmhandle(),"inoscan@InoScanSetActionOption");
	InoScanInit_sym = (InoScanInit_t)ImportPublicObject(getnlmhandle(),"inoscan@InoScanInit");
	InoScanSetConfig_sym = (InoScanSetConfig_t)ImportPublicObject(getnlmhandle(),"inoscan@InoScanSetConfig");
	InoScanScanIT_sym = (InoScanScanIT_t)ImportPublicObject(getnlmhandle(),"inoscan@InoScanScanIT");
	InoScanDeInit_sym = (InoScanDeInit_t)ImportPublicObject(getnlmhandle(),"inoscan@InoScanDeInit");
	
	if (eTrust7_Check())
	{
		eTrust7_UnImportSymbols();
		return ENOENT;
	}
	return ESUCCESS;
}


//VirusData_t* virus
void callback(struct scanReturnData* data, VirusData_t* virus, int i3, int iErrorCode, int i5, void* unknown)
{
	if (virus == NULL)
	{
		OutputToScreen(0,"MFAVA: virus* parameter in callback was NULL.\n");
		return;
	}

	if (virus->Debug)
	{
		printf("-*> %x %x %x %x | %x | %x\n",data,virus,i3,iErrorCode,i5,unknown);
		printf("-*> %x %x %x %x %x \n",data->l1,data->lErrorCode,data->lWin32ErrorCode,data->lIsVirus,data->lVirusType);
		printf("-*> %s = %s [%X]\n",data->szFile,data->szVirusName,data->szVirusName);
		
	}
	
	printf("virusnamelength = %d\n",virus->iVirusNameLength);
	
	strncpy(virus->szVirusName, data->szVirusName,virus->iVirusNameLength);
	virus->lVirusType = (long) data->lVirusType;
	virus->lErrorCode = (long) data->lErrorCode;
	virus->lExtErrorCode = (long) data->lWin32ErrorCode;
	
	printf("set virus name: %X '%s'\n",virus->szVirusName,virus->szVirusName);
}

int eTrust7_ScanFile(MFAVA_HANDLE hAVA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType)
{

	appdata_t* app = NULL;
	int rc;
	VirusData_t* virus = NULL;

	if ((app = GetOrSetAppData()) == NULL)
		return ENOMEM;

	if (hAVA != (MFAVA_HANDLE)ETRUST7_MAGIC)
	{
		if (app->Debug)
			printf("%X eTrust7: not doing ScanFile, magic wrong\n",hAVA);
		return ESUCCESS;
	}

	if (eTrust7_Check())
		return ENOTSUP;
	
	if (app->eTrust_InitComplete != 1)
		return ENOENT;

	if ((virus = (VirusData_t*)malloc(sizeof(VirusData_t))) == NULL)
		return ENOMEM;
		
	printf("%X iVirusNameLength = %d\n",iVirusNameLength);

	virus->sig = 0xBA7F0000;
	virus->Debug = app->Debug;
	virus->iVirusNameLength = iVirusNameLength;
	virus->szVirusName = szVirusName;
	virus->lVirusType = 0;
	virus->endSig=0xABABABAB;
	
	if ( (iVirusNameLength > 7) && (virus->szVirusName != NULL) )
	{
		virus->szVirusName[0] = 0;
		virus->szVirusName[1] = 0;
		virus->szVirusName[2] = 'V';
		virus->szVirusName[3] = 'R';
		virus->szVirusName[4] = 'S';
		virus->szVirusName[5] = 0;
	}

	if (app->Debug)
		printf("%X calling InoScanScanIT [%X,%X,%X]\n",hAVA,app->eTrust_hInoScan, szFileName, virus);

	rc = InoScanScanIT_sym(app->eTrust_hInoScan, szFileName, virus);
	printf("%X InoScanScanIT rc = %X %i\n",hAVA,rc,rc);
	printf("%X -> %X %X %X\n\t%s\n",hAVA,virus,virus->szVirusName,szVirusName,szVirusName);

	iVirusType = virus->lVirusType;
	rc = (int)virus->lErrorCode;

	EnterDebugger();

	free(virus);

	return rc;
}

int eTrust7_DeInit(MFAVA_HANDLE hAVA)
{

	appdata_t* app = NULL;
	int rc;

	if ((app = GetOrSetAppData()) == NULL)
		return ENOMEM;

	if ((hAVA != NULL) && (hAVA != (MFAVA_HANDLE)ETRUST7_MAGIC))
	{
		if (app->Debug)
			printf("%X eTrust7: not doing DeInit, magic wrong\n",hAVA);
		return ESUCCESS;
	}
	
	if (eTrust7_Check())
		return ENOTSUP;
	
	if (app->eTrust_InitComplete != 1)
		return ENOENT;

	rc = InoScanDeInit_sym(app->eTrust_hInoScan);
	
	if (app->Debug)
		printf("%X InoScanDeInit rc = %X %i\n",hAVA,rc,rc);
	
	rc = FreeLibrary_sym(app->eTrust_hLibInoScan);
	if (app->Debug)
		printf("%X FreeLibrary INOSCAN: %x\n",hAVA,rc);
		
	rc = FreeLibrary_sym(app->eTrust_hLibLibWinC);
	if (app->Debug)
		printf("%X FreeLibrary LIBWINC: %x\n",hAVA,rc);
	
	return ESUCCESS;
}


int eTrust7_Init(MFAVA_HANDLE &hAVA)
{
	appdata_t* app = NULL;
	int rc;
	
	if ((app = GetOrSetAppData()) == NULL)
		return ENOMEM;
	
	if (eTrust7_Check())
		if (eTrust7_ImportSymbols())
			return ENOTSUP;
	
	hAVA = (MFAVA_HANDLE)ETRUST7_MAGIC;
		
//	long arcTypes[20];
	struct actionConfig actionConf = { 3, "SYS:eTrustAV\AVEngine", "SYS:eTrustAV\AVEngine", 0 };
	struct inoConfig2 cf2 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	struct inoConfig cf = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
//	memset(arcTypes,0,20*sizeof(long));
//	arcTypes[0] = 0x9;
//	cf.pdwArcTypesList = arcTypes;
	cf.dwArcTypesCount = 0; // 1
	cf.bScanArc = 0;
	
	memset(&cf2,0,sizeof(struct inoConfig2));
	memset(&cf,0,sizeof(struct inoConfig));
	cf2.l9 = 2;
	cf2.l10 = 2;

	cf2.p1 = (char*)malloc(0x20);
	if (cf2.p1 == NULL)
		return ENOMEM;
		
	memset(cf2.p1,0,0x20);
	cf2.p1[0] = (char)0x1E;
	cf2.p1[1] = (char)0xBD;
	cf2.p1[2] = (char)0xFA;
	cf2.p1[3] = (char)0xCA;
	cf2.p1[4] = (char)0x0C;

	cf2.p2 = (char*)malloc(0x20);
	if (cf2.p2 == NULL)
	{
		free(cf2.p1);
		return ENOMEM;
	}
	
	memset(cf2.p2,0,0x20);
	cf2.p2[0] = (char)0x1E;
	cf2.p2[1] = (char)0xBD;
	cf2.p2[2] = (char)0xFA;
	cf2.p2[3] = (char)0xCA;
	cf2.p2[4] = (char)0x0C;
	
	app->eTrust_hLibLibWinC = LoadLibrary_sym("LIBWINC");
	if (app->Debug)
		printf("LoadLibrary LIBWINC: %x.\n",app->eTrust_hLibLibWinC);

	app->eTrust_hLibInoScan = LoadLibrary_sym("INOSCAN");
	if (app->Debug)
		printf("LoadLibrary INOSCAN: %x.\n",app->eTrust_hLibInoScan);
	
	// set up config etc
	
	memset(actionConf.padding,0,999);
	rc = InoScanSetActionOption_sym(&actionConf);
	if (app->Debug)
		printf("InoScanSetActionOption rc = %X\n",rc);

	rc = InoScanInit_sym(&app->eTrust_hInoScan);
	if (app->Debug)
	{
		printf("InoScanInit rc = %X, hIno = %X\n",rc,app->eTrust_hInoScan);
//		printf(" %c %c %c %c\n");
	}
	
	rc = InoScanSetConfig_sym(app->eTrust_hInoScan, &cf, &cf2, callback, 0);
	if (app->Debug)
		printf("InoScanSetConfig rc = 0x%X %i\n",rc,rc);
	
	app->eTrust_InitComplete = 1;
	
	free(cf2.p1);
	free(cf2.p2);
	
	return ESUCCESS;
}

// -eof-