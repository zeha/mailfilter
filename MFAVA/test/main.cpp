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

#include <MFAVA.h>

#define MAX_PATH 260

extern "C" {
	int LoadModuleInSpace( scr_t screen, const char *commandline, int flags, void *addrspaceid );
	int kGetThreadAddressSpaceID( void *ThreadHandle, void **AddressSpace );
	extern void* OSAddressSpaceID;

}

MailFilter_AVA_Init_t MailFilter_AVA_Init_sym = NULL;
MailFilter_AVA_DeInit_t MailFilter_AVA_DeInit_sym = NULL;
MailFilter_AVA_ScanFile_t MailFilter_AVA_ScanFile_sym = NULL;
MailFilter_AVA_Status_t MailFilter_AVA_Status_sym = NULL;

int MailFilter_AVA_UnImportSymbols()
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

int MailFilter_AVA_ImportSymbols()
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

int MailFilter_AVA_Check()
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
	return ESUCCESS;
}

int main(int argc, char* argv[])
{
	MFAVA_HANDLE hAgent = NULL;
	int rc = 0;
	int scanRc = 0;
	int libthreshold = 0;
	
	if (MailFilter_AVA_ImportSymbols())
		printf("There was an error getting MFAVA Symbols.\n");

	if (MailFilter_AVA_Check())
	{
		printf("MFAVA is not initialized correctly. Terminating.\n");
		pressenter();
		return 1;
	}
	
	rc = MailFilter_AVA_Init_sym(1,TRUE,hAgent);
	printf("%X Init rc: %d\n",hAgent,rc);
	if (rc)
	{
		printf("MFAVA could not talk to the AntiVirus product.\n");
		return 2;
	}

	char* szVirusName = (char*)malloc(MAX_PATH);
	size_t iVirusNameLength = MAX_PATH;
	int iVirusType = 0;
	const char* szFileName = "SYS:\\TMP\\EICAR.COM";
	
	szVirusName[0] = 0;
	
	if (argc==2)
		szFileName = argv[1];
	
	printf("  File: \"%s\"\n",szFileName);
	
	scanRc = MailFilter_AVA_ScanFile_sym(hAgent, szFileName, szVirusName, iVirusNameLength, iVirusType);
	printf("%X ScanFile rc: %d\n",hAgent,scanRc);
	printf("     > \"%s\" (size=%d); type=%d\n", szVirusName, iVirusNameLength, iVirusType);
	
	rc = MailFilter_AVA_DeInit_sym(hAgent);
	printf("%X DeInit rc: %d\n",hAgent,rc);
	
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
}

// -eof-
