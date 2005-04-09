#include <string>
#include <fstream>

#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netware.h>
#include <malloc.h>
#include <library.h>
#include <nks/vm.h>

#include "MFAVA.h"
#include "library.h"

#ifdef __cplusplus
extern "C" {
#endif

/*SAVICoInitialize,
SAVICoUninitialize,
SAVICoCreateInstance
*/
/*
	int InoScanScanIT(void* ptr, const char* szFileName, VirusData_t* virus); */
	typedef int(*SAVICoInitialize_t)(void*, const char* szFileName);
	typedef int(*SAVICoUninitialize_t)(void*, const char* szFileName);
	typedef int(*SAVICoCreateInstance_t)(void*, const char* szFileName);
	
	static SAVICoInitialize_t			SAVICoInitialize_sym = NULL;
	static SAVICoUninitialize_t			SAVICoUninitialize_sym = NULL;
	static SAVICoCreateInstance_t		SAVICoCreateInstance_sym = NULL;
#ifdef __cplusplus
}
#endif

static int Sophos_UnImportSymbols()
{
	SAVICoInitialize_sym = NULL;
	SAVICoUninitialize_sym = NULL;
	SAVICoCreateInstance_sym = NULL;

	UnImportPublicObject(gModuleHandle,"SAVICoInitialize");
	UnImportPublicObject(gModuleHandle,"SAVICoUninitialize");
	UnImportPublicObject(gModuleHandle,"SAVICoCreateInstance");
	
	return ESUCCESS;
}
static int Sophos_Check()
{
	if (
		(SAVICoInitialize_sym == NULL) ||
		(SAVICoUninitialize_sym == NULL) ||
		(SAVICoCreateInstance_sym == NULL)
		)
	{
		return ENOENT;
	}
	return ESUCCESS;
}
static int Sophos_ImportSymbols()
{
	Sophos_UnImportSymbols();	// clean up before doing our new work

/*	LoadLibrary_sym = (LoadLibrary_t)ImportPublicObject(gModuleHandle,"libwinc@LoadLibrary");
	if ((!LoadLibrary_sym) && gDebugSetCmd) printf("MFAVA: Could not get symbol EA-01.\n");
	FreeLibrary_sym = (FreeLibrary_t)ImportPublicObject(gModuleHandle,"libwinc@FreeLibrary");
	if ((!FreeLibrary_sym) && gDebugSetCmd) printf("MFAVA: Could not get symbol EA-02.\n");

	InoScanSetActionOption_sym = (InoScanSetActionOption_t)ImportPublicObject(gModuleHandle,"inoscan@InoScanSetActionOption");
	if ((!InoScanSetActionOption_sym) && gDebugSetCmd) printf("MFAVA: Could not get symbol EA-03.\n");
	InoScanInit_sym = (InoScanInit_t)ImportPublicObject(gModuleHandle,"inoscan@InoScanInit");
	if ((!InoScanInit_sym) && gDebugSetCmd) printf("MFAVA: Could not get symbol EA-04.\n");
	InoScanSetConfig_sym = (InoScanSetConfig_t)ImportPublicObject(gModuleHandle,"inoscan@InoScanSetConfig");
	if ((!InoScanSetConfig_sym) && gDebugSetCmd) printf("MFAVA: Could not get symbol EA-05.\n");
	InoScanScanIT_sym = (InoScanScanIT_t)ImportPublicObject(gModuleHandle,"inoscan@InoScanScanIT");
	if ((!InoScanScanIT_sym) && gDebugSetCmd) printf("MFAVA: Could not get symbol EA-06.\n");
	InoScanDeInit_sym = (InoScanDeInit_t)ImportPublicObject(gModuleHandle,"inoscan@InoScanDeInit");
	if ((!InoScanDeInit_sym) && gDebugSetCmd) printf("MFAVA: Could not get symbol EA-07.\n");
*/	
	if (Sophos_Check())
	{
		if (gDebugSetCmd) printf("MFAVA: Sophos AntiVirus will not be used.\n");
		Sophos_UnImportSymbols();
		return ENOENT;
	}
	return ESUCCESS;
}

int Sophos_PreInit()
{
	return Sophos_ImportSymbols();
}

/*
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
	
	if (gDebugSetCmd) printf("MFAVADebug: virusnamelength = %d\n",virus->iVirusNameLength);
	
	strncpy(virus->szVirusName, data->szVirusName,virus->iVirusNameLength);
	virus->lVirusType = (long) data->lVirusType;
	virus->lErrorCode = (long) data->lErrorCode;
	virus->lExtErrorCode = (long) data->lWin32ErrorCode;
	
	if (gDebugSetCmd) printf("MFAVADebug: set virus name: %X '%s'\n",virus->szVirusName,virus->szVirusName);
}*/

int Sophos_ScanFile(MFAVA_HANDLE hAVA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType)
{
	appdata_t* app = NULL;
	int rc;
//	VirusData_t* virus = NULL;

	if ((app = GetOrSetAppData()) == NULL)
		return ENOMEM;

	if (hAVA != (MFAVA_HANDLE)SOPHOS_MAGIC)
	{
		if (app->Debug)
			printf("MFAVADebug: %X Sophos: not doing ScanFile, magic wrong\n",hAVA);
		return ESUCCESS;
	}

	if (Sophos_Check())
		return ENOTSUP;
	
	if (app->Sophos_InitComplete != 1)
		return ENOENT;

/*
	if ((virus = (VirusData_t*)__my_malloc(sizeof(VirusData_t))) == NULL)
		return ENOMEM;
		
	if (gDebugSetCmd) printf("MFAVADebug: %X VirusNameLength = %d\n",hAVA,iVirusNameLength);

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
		printf("MFAVADebug: %X calling InoScanScanIT [%X,%X,%X]\n",hAVA,app->eTrust_hInoScan, szFileName, virus);

	rc = InoScanScanIT_sym(app->eTrust_hInoScan, szFileName, virus);
	if (app->Debug)
	{
		printf("MFAVADebug: %X InoScanScanIT rc = %X %i\n",hAVA,rc,rc);
		printf("MFAVADebug: %X -> %X %X %X\n\t%s\n",hAVA,virus,virus->szVirusName,szVirusName,szVirusName);
	}
	
	iVirusType = virus->lVirusType;
	rc = (int)virus->lErrorCode;

	free(virus);
*/
	return rc;
}

int Sophos_DeInit(MFAVA_HANDLE hAVA)
{

	appdata_t* app = NULL;
	int rc;

	if ((app = GetOrSetAppData()) == NULL)
		return ENOMEM;

	if (app->Sophos_InitComplete != 1)
		return ENOENT;

	app->Sophos_InitComplete = 0;

	if (hAVA != NULL)
	{
		if (gDebugSetCmd) printf("MFAVA Error: AVA Handle NULL!\n");
		return ENOMEM;
	}

	if (hAVA != (MFAVA_HANDLE)SOPHOS_MAGIC)
	{
		if (app->Debug)
			printf("MFAVADebug: %X Sophos: not doing DeInit, magic wrong\n",hAVA);
		return ESUCCESS;
	}
	
	if (Sophos_Check())
		return ENOTSUP;

/*	
	rc = InoScanDeInit_sym(app->eTrust_hInoScan);
	
	if (app->Debug)
		printf("MFAVADebug: %X InoScanDeInit rc = %X %i\n",hAVA,rc,rc);
	
	rc = FreeLibrary_sym(app->eTrust_hLibInoScan);
	if (app->Debug)
		printf("MFAVADebug: %X FreeLibrary INOSCAN: %x\n",hAVA,rc);
		
	rc = FreeLibrary_sym(app->eTrust_hLibLibWinC);
	if (app->Debug)
		printf("MFAVADebug: %X FreeLibrary LIBWINC: %x\n",hAVA,rc);
	
	if (gDebugSetCmd) printf("MFAVA: Terminated!\n");
*/	
	return ESUCCESS;
}


int Sophos_Init(MFAVA_HANDLE &hAVA)
{
	appdata_t* app = NULL;
	int rc;
	
	if ((app = GetOrSetAppData()) == NULL)
		return ENOMEM;
	
	app->eTrust_InitComplete = 0;

	if (Sophos_Check())
		return ENOTSUP;
	/*
	char szAvEnginePath[1000];
	szAvEnginePath[0] = 0;
	{
		std::string line;
		unsigned int pos;
		std::string param;
		std::string value;
		
		std::ifstream cfgFile("SYS:\\SYSTEM\\CAENV.INI");
		
		if (!cfgFile.good())
		{
			return ENOTSUP;
		} else {
		
			while (cfgFile >> line)
			{
				if (line.at(0) != ';')
				{
					pos = line.find("=");
					if (pos != -1)
					{
						param = line.substr(0,pos);
						if (line.size() > pos+1)
							value = line.substr(pos+1);
						else
							value = "";
						
						if (app->Debug)
							printf("MFAVADebug: CAENV: ('%s') '%s'='%s'\n",line.at(0),param.c_str(),value.c_str());

						if (param == "AVENGINE_LOC")
							strncpy(szAvEnginePath,value.c_str(),260);
						
					}
				}
				NXThreadYield();
			}
			
			cfgFile.close();
		}
	}
	szAvEnginePath[259] = 0;
	if (szAvEnginePath[0] == 0)
		return ENOTSUP;
	
	hAVA = (MFAVA_HANDLE)Sophos_MAGIC;
		
	struct actionConfig actionConf = { 3, "", "", 0 };
	struct inoConfig2 cf2 = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	struct inoConfig cf = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	strcpy(actionConf.avEng,szAvEnginePath);
	strcpy(actionConf.avEng2,szAvEnginePath);
	
//	memset(arcTypes,0,20*sizeof(long));
//	arcTypes[0] = 0x9;
//	cf.pdwArcTypesList = arcTypes;
	cf.dwArcTypesCount = 0; // 1
	cf.bScanArc = 0;
	
	memset(&cf2,0,sizeof(struct inoConfig2));
	memset(&cf,0,sizeof(struct inoConfig));
	cf2.l9 = 2;
	cf2.l10 = 2;

	cf2.p1 = (char*)__my_malloc(0x20);
	if (cf2.p1 == NULL)
		return ENOMEM;
		
	memset(cf2.p1,0,0x20);
	cf2.p1[0] = (char)0x1E;
	cf2.p1[1] = (char)0xBD;
	cf2.p1[2] = (char)0xFA;
	cf2.p1[3] = (char)0xCA;
	cf2.p1[4] = (char)0x0C;

	cf2.p2 = (char*)__my_malloc(0x20);
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
		printf("MFAVADebug: LoadLibrary LIBWINC: %x.\n",app->eTrust_hLibLibWinC);

	app->eTrust_hLibInoScan = LoadLibrary_sym("INOSCAN");
	if (app->Debug)
		printf("MFAVADebug: LoadLibrary INOSCAN: %x.\n",app->eTrust_hLibInoScan);
	
	// set up config etc
	
	memset(actionConf.padding,0,999);
	rc = InoScanSetActionOption_sym(&actionConf);
	if (app->Debug)
		printf("MFAVADebug: InoScanSetActionOption rc = %X\n",rc);

	rc = InoScanInit_sym(&app->eTrust_hInoScan);
	if (app->Debug)
	{
		printf("MFAVADebug: InoScanInit rc = %X, hIno = %X\n",rc,app->eTrust_hInoScan);
//		printf("MFAVADebug:  %c %c %c %c\n");
	}
	
	rc = InoScanSetConfig_sym(app->eTrust_hInoScan, &cf, &cf2, callback, 0);
	if (app->Debug)
		printf("MFAVADebug: InoScanSetConfig rc = 0x%X %i\n",rc,rc);
	
	free(cf2.p1);
	free(cf2.p2);
	
	app->eTrust_InitComplete = 1;
	
	return ESUCCESS; */
	
	return ENOENT;
}

// -eof-
