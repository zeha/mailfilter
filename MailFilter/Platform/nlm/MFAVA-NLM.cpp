/*
 +
 +   MFAVA-NLM.cpp
 +
 +   MFAVA Library Abstraction.
 +
 */

#include <screen.h>
#include <stdio.h>
#include <errno.h>
#include <netware.h>
#include <event.h>
#include <library.h>
#include <malloc.h> 
#include <dlfcn.h>
#include <nks/vm.h>
#include <ndkvers.h>
#include "MailFilter.h"

#include "../../MFAVA/MFAVA.h"
#include "MFAVA-NLM.h"

/*#undef MAX_PATH
#define MAX_PATH 260
*/

extern "C" {
	int LoadModuleInSpace( scr_t screen, const char *commandline, int flags, void *addrspaceid );
	int kGetThreadAddressSpaceID( void *ThreadHandle, void **AddressSpace );
	extern void* OSAddressSpaceID;

}

static MailFilter_AVA_Init_t MailFilter_AVA_Init_sym = NULL;
static MailFilter_AVA_DeInit_t MailFilter_AVA_DeInit_sym = NULL;
static MailFilter_AVA_ScanFile_t MailFilter_AVA_ScanFile_sym = NULL;
static MailFilter_AVA_Status_t MailFilter_AVA_Status_sym = NULL;

static int MailFilter_AVA_UnImportSymbols()
{
	MailFilter_AVA_Init_sym = NULL;
	MailFilter_AVA_DeInit_sym = NULL;
	MailFilter_AVA_ScanFile_sym = NULL;
	MailFilter_AVA_Status_sym = NULL;

	UnImportPublicObject(getnlmhandle(),"MailFilter_AVA_Init");
	UnImportPublicObject(getnlmhandle(),"MailFilter_AVA_DeInit");
	UnImportPublicObject(getnlmhandle(),"MailFilter_AVA_ScanFile");
	UnImportPublicObject(getnlmhandle(),"MailFilter_AVA_Status");
	
	return ESUCCESS;
}

static int MailFilter_AVA_ImportSymbols()
{
	MailFilter_AVA_UnImportSymbols();	// clean up before doing our new work

	MailFilter_AVA_Init_sym = (MailFilter_AVA_Init_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_Init");
	MailFilter_AVA_DeInit_sym = (MailFilter_AVA_DeInit_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_DeInit");
	MailFilter_AVA_ScanFile_sym = (MailFilter_AVA_ScanFile_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_ScanFile");
	MailFilter_AVA_Status_sym = (MailFilter_AVA_Status_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_Status");
	
	if (
		(MailFilter_AVA_Init_sym == NULL) ||
		(MailFilter_AVA_DeInit_sym == NULL) ||
		(MailFilter_AVA_ScanFile_sym == NULL) ||
		(MailFilter_AVA_Status_sym == NULL)
		)
	{
		MailFilter_AVA_UnImportSymbols();
		return ENOENT;
	}
	return ESUCCESS;
}

static MFAVA_HANDLE ghAgent = NULL;

int MailFilter_AV_Check()
{
	if (
		(MailFilter_AVA_Init_sym == NULL) ||
		(MailFilter_AVA_DeInit_sym == NULL) ||
		(MailFilter_AVA_ScanFile_sym == NULL) ||
		(MailFilter_AVA_Status_sym == NULL)
		)
	{
		return ENOENT;
	}
	if (ghAgent == NULL)
		return ENOCONTEXT;

	return ESUCCESS;
}

int MailFilter_AV_Init()
{
	int rc = 0;
	int scanRc = 0;
	int libthreshold = 0;
	
	if (MailFilter_AVA_ImportSymbols())
	{
		// symbols not there; so then, load ava ...
		system("LOAD MFAVA.NLM");
		ThreadSwitch();
		delay(1000);
	}
	
	if (MailFilter_AVA_ImportSymbols())
	{
		MFD_Out(MFD_SOURCE_VSCAN,"AVA: Could not get symbols.\n");
		return ENOTSUP;
	}

	ghAgent = (MFAVA_HANDLE)0xDEADBEEF;
	if (MailFilter_AV_Check())
	{
		MFD_Out(MFD_SOURCE_VSCAN,"AVA: not initialized correctly.\n");
		return ENOTSUP;
	}
	
	// 1 = eTrust AntiVirus 7
	// FALSE = No Debug Stuff (goes to logger screen)
	rc = MailFilter_AVA_Init_sym(1,MFT_AVAVerbose,ghAgent);
	MFD_Out(MFD_SOURCE_VSCAN,"%X Init rc: %d\n",ghAgent,rc);
	if (rc)
	{
		MFD_Out(MFD_SOURCE_VSCAN,"AVA: MFAVA could not talk to AV product NLMs.\n");
		return EINCONSIS;
	}
	
	return 0;
}

int MailFilter_AV_ScanFile(const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType)
{
	if (MailFilter_AV_Check())
		return ENOCONTEXT;
//	char* szVirusName = (char*)malloc(MAX_PATH);
//	size_t iVirusNameLength = MAX_PATH;
//	int iVirusType = 0;
//	const char* szFileName = "SYS:\\TMP\\EICAR.COM";
//	
	int scanRc = 0;
	szVirusName[0] = 0;
	
	
//	if (argc==2)
//		szFileName = argv[1];
	
//	printf("  File: \"%s\"\n",szFileName);
	
	ThreadSwitch();
	scanRc = MailFilter_AVA_ScanFile_sym(ghAgent, szFileName, szVirusName, iVirusNameLength, iVirusType);
//	printf("%X ScanFile rc: %d\n",hAgent,scanRc);
	MFD_Out(MFD_SOURCE_VSCAN,"AVA Scan: rc=%d, \"%s\" (size=%d); type=%d\n", scanRc, szVirusName, iVirusNameLength, iVirusType);

	return scanRc;
}

int MailFilter_AV_DeInit()
{	
	int rc = 0;
	rc = MailFilter_AV_Check();
	if ((rc) && (rc != ENOCONTEXT))
		return ENOCONTEXT;

	rc = MailFilter_AVA_DeInit_sym(ghAgent);
	MFD_Out(MFD_SOURCE_VSCAN,"AVA: %X DeInit rc: %d\n",ghAgent,rc);
	ghAgent = NULL;
	return 0;
}	


/*

	printf("\n");
	printf("Conclusion:\n");
	printf(" * The file \"%s\" ",szFileName);
	if (scanRc == 0)
	{
		if (iVirusType != 0)
		{
			const char* szType = "Virus";
			if (iVirusType == 2) szType = "Trojan";
			if (iVirusType == 3) szType = "Macro Virus";
			if (iVirusType == 4) szType = "Worm";
			printf("contains a \"%s\",\n",szType);
			printf("   named \"%s\".",szVirusName);
		} else {
			printf("is probably clean.");
		}
	} else {
		printf("could not be scanned successfully.");
	}
	printf("\n");
	
	if (szVirusName != NULL)
		free(szVirusName);
	
	pressenter();

	return ESUCCESS;
*/

// -eof-
