#include <screen.h>
#include <stdio.h>
#include <errno.h>
#include <netware.h>
#include <windows.h>
#include <event.h>
#include <library.h>
#include <nks/netware.h> 
#include <ringx.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "MFAVA.h"		// in reality, nothing from it ever gets used in MFAVA itself.
#include "library.h"


int					 gLibId        = -1;
void				*gModuleHandle = (void *) NULL;
scr_t				 gLibScreen     = NULL;
rtag_t				 gLibAllocRTag  = NULL,
					 gDownEventRTag = NULL,
					 gDebugSetCmdRTag = NULL; 
event_handle_t		 gDownEventHandle = 0;
int					 gDebugSetCmd	= FALSE;
settableparms_t		 gDebugSetCmdStruct;


void   *gRef_MFAVA_Init = (void *) NULL;
void   *gRef_MFAVA_DeInit = (void *) NULL;
void   *gRef_MFAVA_ScanFile = (void *) NULL;
void   *gRef_MFAVA_Status = (void *) NULL;

MailFilter_AVA_Init_t MailFilter_AVA_Init_sym = NULL;
MailFilter_AVA_DeInit_t MailFilter_AVA_DeInit_sym = NULL;
MailFilter_AVA_ScanFile_t MailFilter_AVA_ScanFile_sym = NULL;
MailFilter_AVA_Status_t MailFilter_AVA_Status_sym = NULL;

static int  sMarshallCodeHandle = 0; 

// **
// ** OS support stuff
// **
int DownWarning
(
	void	*printf,
	void	*parm,
	void	*userParm
)
{
#pragma unused(printf,parm,userParm)

	OutputToScreen(0, "MFAVA: Closing handles...\n");
	
	eTrust7_DeInit(NULL);

	KillMe(gModuleHandle);
	
	return 0;
}

void *__my_AllocSleepOK ( 
   size_t   size, 
   rtag_t   rTag, 
   int     *slept)
{
	
	void* p = NULL;


	for (int cnt = 0; cnt<20; cnt++)
	{
		p = AllocSleepOK(size,rTag,slept);
		if ((!p)&&(cnt==0))
			printf("MFAVA: AllocSleepOK returned NULL memory.\n");

		if (p)
			return p;
	}
	printf("MFAVA: Out of memory!\n");
	return NULL;
}


void* __my_malloc(size_t length)
{
	void* p = NULL;

	for (int cnt = 0; cnt<20; cnt++)
	{
		p = malloc(length);
		if ((!p)&&(cnt==0))
			printf("MFAVA: malloc returned NULL memory.\n");

		if (p)
			return p;
	}
	printf("MFAVA: out of memory.\n");
	return NULL;
}

// **
// ** Library support stuff
// **
appdata_t *GetOrSetAppData( void )
{
	rtag_t		rtag;
	appdata_t	*app = NULL;
	int rc;

/*
** If the caller already has instance data, just return it. If not, then
** it's a new caller so allocate some, but attempt to do it on their tag.
** That failing, we'll use our own tag 'cause it will get cleaned up anyway
** when they (or we) unload.
*/
	if ( (app = (appdata_t*)get_app_data(gLibId)) != NULL )
		return app;

	if (!(rtag = getallocresourcetag()))
		rtag = gLibAllocRTag;


	if ( (app = (appdata_t*)__my_AllocSleepOK(sizeof(appdata_t), rtag, NULL)) != NULL )
	{
		app->Debug = 0;
		app->eTrust_InitComplete = 0;
		app->eTrust_hInoScan = NULL;
		app->eTrust_hLibInoScan = NULL;
		app->eTrust_hLibLibWinC = NULL;
	}

	if ((rc = set_app_data(gLibId, app)) != ESUCCESS)
	{
		printf("MFAVA: error %d (%d) in set_app_data.\n",rc,errno);
		Free(app);
		return (appdata_t *) NULL;
	}
	
	return app;
}

int DisposeAppData(void	*data_area)
{
	appdata_t	*app = (appdata_t*)data_area;

	if (app)
	{
		Free(app);
	}

	set_app_data(gLibId, NULL);

	return 0;
}

// **
// ** MFAVA exports. (exported under different names, though.)
// **
static int MFAVA_Init(int iVirusScanner, int iDebug, MFAVA_HANDLE &hReturn)
{
	appdata_t* app;
	int iError = ESUCCESS;
	
	if ((app = GetOrSetAppData()) == NULL)
		return ENOMEM;
	
	// set up handle
	hReturn = 0;
	
	app->Debug = iDebug;
	
	switch (iVirusScanner)
	{
		default:
			iError = ENOENT;
			break;
			
		case 1:
			iError = eTrust7_Init(hReturn);
			break;
			
		case 2:
			iError = Sophos_Init(hReturn);
			break;
	}

	return iError;
}

static int MFAVA_DeInit(MFAVA_HANDLE hAVA)
{
	int iError = ESUCCESS;
	
	// call all _DeInit functions in a row...
	
	// eTrust 7.x
	iError = eTrust7_DeInit(hAVA);
	if (iError)		goto FinishIt;
	
	iError = Sophos_DeInit(hAVA);
	if (iError)		goto FinishIt;

FinishIt:
	
	DisposeAppData(GetOrSetAppData());
		
	return iError;
}

static int MFAVA_ScanFile(MFAVA_HANDLE hAVA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType)
{
	int iError = ESUCCESS;
	
	switch ((long)hAVA)
	{
		default:
			iError = ENOENT;
			break;
		case ETRUST7_MAGIC:
			iError = eTrust7_ScanFile(hAVA,szFileName,szVirusName,iVirusNameLength,iVirusType);
			break;
		case SOPHOS_MAGIC:
			iError = Sophos_ScanFile(hAVA,szFileName,szVirusName,iVirusNameLength,iVirusType);
			break;
	}
	return iError;
}

static int MFAVA_Status(MFAVA_HANDLE hAVA, long whatToGet)
{	// pseudo for now.
#pragma unused(hAVA,whatToGet)
	return ESUCCESS;
}


// **
// ** Marshalling interfaces
// **
void StartOfMarshallingCode( void ) { } 

int M_MFAVA_Init(int iVirusScanner, int iDebug, MFAVA_HANDLE &hReturn)
{
	if (MailFilter_AVA_Init_sym == NULL)
		return EINCONSIS;

//	RX_CHECK_BUFFER(hReturn, sizeof(void*));
//	RxLockMemory(hReturn, sizeof(void*));
	MFAVA_HANDLE _hReturn = hReturn;
	
	int rc;
	
	rc = MailFilter_AVA_Init_sym(iVirusScanner,iDebug,_hReturn);
	
	hReturn = _hReturn;
	
	return rc;
	
}
int M_MFAVA_DeInit(MFAVA_HANDLE hAVA)
{
	if (MailFilter_AVA_DeInit_sym == NULL)
		return EINCONSIS;
	return MailFilter_AVA_DeInit_sym(hAVA);
}
int M_MFAVA_Status(MFAVA_HANDLE hAVA, long whatToGet)
{
	if (MailFilter_AVA_Status_sym == NULL)
		return EINCONSIS;
	return MailFilter_AVA_Status_sym(hAVA, whatToGet);
}
int M_MFAVA_ScanFile(MFAVA_HANDLE hAVA, const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType)
{
	pthread_t   thread;
	size_t		fileNameLen;
	int _iVirusType = iVirusType;
	size_t _iVirusNameLength = iVirusNameLength;
	
	int err;
	
	if (MailFilter_AVA_ScanFile_sym == NULL)
		return EINCONSIS;

	if (szFileName)
	{
		RX_CHECK_STRLEN(szFileName,fileNameLen);
		RxLockMemory((void*)szFileName,fileNameLen);
	}
	
	if (szVirusName)
	{
		RX_CHECK_STRLEN(szVirusName,iVirusNameLength);
		RxLockMemory((void*)szVirusName,iVirusNameLength);
	}

	thread = pthread_self();
	
/*	if (err = RxRegisterThreadResource(thread, CleanUpFooStuff))
	{
		if (szFileName)
			RxUnlockMemory((void*)szFileName,fileNameLen);
		if (szVirusName)
			RxUnlockMemory((void*)szVirusName,iVirusNameLength);
		return err;
	} */
	
	err = MailFilter_AVA_ScanFile_sym(hAVA, szFileName, szVirusName, _iVirusNameLength, _iVirusType);


	if (szFileName)
		RxUnlockMemory((void*)szFileName,fileNameLen);
	if (szVirusName)
		RxUnlockMemory((void*)szVirusName,iVirusNameLength);
	
	iVirusType = _iVirusType;
	
	return err;
}


void EndOfMarshallingCode( void ) { } 

int RegisterMarshalledInterfaces()
{
	RxIdentifyCode((void*)StartOfMarshallingCode, (void*)EndOfMarshallingCode, &sMarshallCodeHandle);

	if (RxRegisterSysCall((void*)M_MFAVA_Init, "MFAVA@MailFilter_AVA_Init", 3))
		return EINVAL;
	if (RxRegisterSysCall((void*)M_MFAVA_DeInit, "MFAVA@MailFilter_AVA_DeInit", 1))
		return EINVAL;
	if (RxRegisterSysCall((void*)M_MFAVA_ScanFile, "MFAVA@MailFilter_AVA_ScanFile", 5))
		return EINVAL;
	if (RxRegisterSysCall((void*)M_MFAVA_Status, "MFAVA@MailFilter_AVA_Status", 2))
		return EINVAL;

	return ESUCCESS;
}

int UnRegisterMarshalledInterfaces()
{
	if (sMarshallCodeHandle) 
		RxUnidentifyCode(sMarshallCodeHandle);
		
	RxUnregisterSysCall("MFAVA@MailFilter_AVA_Init");
	RxUnregisterSysCall("MFAVA@MailFilter_AVA_DeInit");
	RxUnregisterSysCall("MFAVA@MailFilter_AVA_ScanFile");
	RxUnregisterSysCall("MFAVA@MailFilter_AVA_Status");

	return ESUCCESS;
}

extern "C" {
	int __init_malloc();
	int __deinit_malloc();
}

// **
// ** Library Init
// **
int DllMain								// returns TRUE (things okay), FALSE (failure)
(
	void				*hinstDLL,		// equivalent to return from register_library()
	unsigned long		fdwReason,		// our attach/detach, etc. message
	void				*lvpReserved	// library per-VM data, place to put error...
)											// ...return or NLM handle
{
	int rc = 0;
	
	switch (fdwReason)
	{
		default:						// bogus message: don't process it!
			return FALSE;

		case DLL_PROCESS_ATTACH :	// (have per-NLM data)
			return TRUE;
		case DLL_PROCESS_DETACH :	// (ibid)
//			appdata_t* app = GetOrSetAppData();
//			DisposeAppData((void*)app);
			return TRUE;
			
		case DLL_THREAD_ATTACH :	// (don't have per-thread data)
		case DLL_THREAD_DETACH :	// (ibid)
			return TRUE;
			
		case DLL_ACTUAL_DLLMAIN :	// (say that we're a library)
			return TRUE;

		case DLL_NLM_STARTUP:		// this is our start-up initialization
		
			// DO THIS AS SOON AS POSSIBLE.
			__init_malloc();
			//
		
			gLibId        = (int) hinstDLL;
			gModuleHandle = lvpReserved;
			gLibScreen    = getconsolehandle();
			
			if (nlmisloadedprotected())
			{
				SetAutoUnloadFlag(getnlmhandle());
//				system("LOAD MFAVA.NLM");
				delay(1000);
				return TRUE;
			}

			
			if (!(gDownEventRTag = AllocateResourceTag(gModuleHandle, "NetWare Shutdown Event Notification", EventSignature)))
			{
				OutputToScreen(0, "MFAVA: Could not register Shutdown Notification!\n\tTerminating!\n");
				return FALSE;
			}


			gDownEventHandle = RegisterForEventNotification(gDownEventRTag,
											EVENT_PRE_DOWN_SERVER, EVENT_PRIORITY_APPLICATION,
											DownWarning, (Report_t) NULL, (void *) NULL);
											
			if (!(gLibAllocRTag = AllocateResourceTag(gModuleHandle, "LibC Interface Working Memory", AllocSignature)))
			{
				OutputToScreen(0, "MFAVA: Could not allocate resource tag!\n\tTerminating!\n");
				return FALSE;
			}

			gDebugSetCmdRTag = AllocateResourceTag(getnlmhandle(), "MFAVA Debug", SettableParameterSignature);
			memset(&gDebugSetCmdStruct, 0, sizeof(gDebugSetCmdStruct));
			gDebugSetCmdStruct.value = &gDebugSetCmd;
			gDebugSetCmdStruct.rTag = gDebugSetCmdRTag;
			gDebugSetCmdStruct.name = "MFAVA Debug";
			gDebugSetCmdStruct.type = SP_TYPE_BOOLEAN;
			gDebugSetCmdStruct.flags = SP_HIDE;
			gDebugSetCmdStruct.category = SP_MISCELLANEOUS;
			gDebugSetCmdStruct.callback = NULL;
			gDebugSetCmdStruct.description = "Enable Debug Messages for MFAVA";
			RegisterSettableParameter(&gDebugSetCmdStruct);

			register_destructor((int) hinstDLL, DisposeAppData);
			
						
			rc = nxExportInterfaceWrapped ( (void*)MFAVA_Init , 3, "MFAVA@MailFilter_AVA_Init", &gRef_MFAVA_Init );
			rc = nxExportInterfaceWrapped ( (void*)MFAVA_DeInit , 1, "MFAVA@MailFilter_AVA_DeInit", &gRef_MFAVA_DeInit );
			rc = nxExportInterfaceWrapped ( (void*)MFAVA_ScanFile , 5, "MFAVA@MailFilter_AVA_ScanFile", &gRef_MFAVA_ScanFile );
			rc = nxExportInterfaceWrapped ( (void*)MFAVA_Status , 2, "MFAVA@MailFilter_AVA_Status", &gRef_MFAVA_Status );

			MailFilter_AVA_Init_sym = (MailFilter_AVA_Init_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_Init");
			MailFilter_AVA_DeInit_sym = (MailFilter_AVA_DeInit_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_DeInit");
			MailFilter_AVA_ScanFile_sym = (MailFilter_AVA_ScanFile_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_ScanFile");
			MailFilter_AVA_Status_sym = (MailFilter_AVA_Status_t)ImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_Status");

			if (RegisterMarshalledInterfaces())
				return FALSE;

			// pre-init all AV NLMs.
			eTrust7_PreInit();

			// done.

			return TRUE;

		case DLL_NLM_SHUTDOWN:
			if (nlmisloadedprotected())
				return TRUE;
			
			DeRegisterSettableParameter(&gDebugSetCmdStruct);
			
			UnRegisterEventNotification(gDownEventHandle);

			UnImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_Init");
			UnImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_DeInit");
			UnImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_ScanFile");
			UnImportPublicObject(getnlmhandle(),"MFAVA@MailFilter_AVA_Status");

			if (gRef_MFAVA_Init)
				nxUnexportInterfaceWrapped ( gRef_MFAVA_Init );
				
			if (gRef_MFAVA_DeInit)
				nxUnexportInterfaceWrapped ( gRef_MFAVA_DeInit );
			
			if (gRef_MFAVA_ScanFile)
				nxUnexportInterfaceWrapped ( gRef_MFAVA_ScanFile );

			if (gRef_MFAVA_Status)
				nxUnexportInterfaceWrapped ( gRef_MFAVA_Status );
			
			UnRegisterMarshalledInterfaces();
				
			__deinit_malloc();
			return TRUE;

	}
}

// -eof-
