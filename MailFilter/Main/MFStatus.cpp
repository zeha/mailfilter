/*+
 +		MFStatus.cpp
 +		
 +		Status/Logging Functions
 +		
 +		Copyright 2001-2004 Christian Hofstädtler.
 +		
 +
 +		
 +		
+*/

#define _MFD_MODULE			"MFSTATUS.CPP"
#include "MailFilter.h"

// Include "Auto-Generated" version header
#include "..\..\Main\MFVersion.h"

static FILE* MF_Log_FileHandle			= NULL;
static bool MF_Status_Initialized		= false;
static char *tmp_MF_LogText				= NULL;

static int MFT_StatusLogCycledOnDay		= 0;

// Init Logging/Status
bool MF_StatusInit()
{
	char* LogFile = (char*)_mfd_malloc(MAX_PATH,"StatusInit");
	if (LogFile == NULL)	{ MF_OutOfMemoryHandler();	return false; }

#ifndef __NOVELL_LIBC__
	atexit(MF_StatusFree);
#endif

	sprintf(LogFile, "%sMailFlt.log",MF_GlobalConfiguration->LogDirectory.c_str());
	MF_Log_FileHandle = NULL;
	// Open LogFile   ascii (=t), append //, commit
	MF_Log_FileHandle = fopen(LogFile,"a+");	//atc

	_mfd_free(LogFile,"StatusInit");
	
	if (MF_Log_FileHandle == NULL)
	{
		MF_DisplayCriticalErrorId(MSG_LOGGING_ERROR_INIT);
		return false;
	}
	
	if (tmp_MF_LogText == NULL)
		tmp_MF_LogText = (char*)_mfd_malloc(4162,"StatusLogTmp");	// 4096+2+26+reserve	-- 4096==max.

	if (tmp_MF_LogText == NULL)	{ MF_OutOfMemoryHandler();	return false; }


	MF_Status_Initialized = true;

#ifdef MAILFILTER_VERSION_BETA
	MF_StatusText("__WARNING: This is a BETA version! Use at your own risk.__");
#endif // MAILFITLER_VERSION_BETA
#ifdef _TRACE
	MF_StatusText("__WARNING: INTERNAL TRACE VERSION - Type G to continue in Debugger__");
#endif // _TRACE

	return true;
}

// Init Logging/Status
void MF_StatusFree()
{
	if (!MF_Status_Initialized)
		return;

	MF_Status_Initialized = false;

	if (MF_Log_FileHandle != NULL)
	{
//		MF_DisplayCriticalError(programMesgTable[CONMSG_LOGGING_DISABLED]);
		fclose( MF_Log_FileHandle );
		MF_Log_FileHandle = NULL;
	}

	if (tmp_MF_LogText != NULL)
	{
		_mfd_free(tmp_MF_LogText,"StatusLogTmp");	
		tmp_MF_LogText = NULL;	
	}
}

void MF_StatusSendDailyReport(const char* szLogFile)
{
	//
	// If requested, send the admin the daily status report
	//
	if (MF_GlobalConfiguration->NotificationAdminDailyReport)
	{
		char szTemp[500];
		char szEmail[8000];

		sprintf(szEmail,"Statistics:\n");
			
		sprintf(szTemp,"  Input: Total: %d, Failed: %d\n",MFS_MF_MailsInputTotal,MFS_MF_MailsInputFailed);
		strcat(szEmail,szTemp);
		
		sprintf(szTemp,"  Ouput: Total: %d, Failed: %d\n",MFS_MF_MailsOutputTotal,MFS_MF_MailsOutputFailed);
		strcat(szEmail,szTemp);

		long clck = (long)(clock() / CLOCKS_PER_SEC / 60);
		long days = clck / 24 / 60;
		long hours = (clck - (days * 24 * 60)) / 60;
		long minutes = clck - (days * 24 * 60) - (hours * 60);
		
		sprintf(szTemp,"  MailFilter run-time: %d d %d h %d min.",days,hours,minutes);
		strcat(szEmail,szTemp);
		
		strcat(szEmail,"\n\n");

		MF_EMailPostmasterGeneric("Daily Status Report",szEmail,szLogFile,"MailFilter Log.txt");
	}
}

static bool MF_StatusCycleLog2()
{
	bool	failed = false;

	MF_StatusLog("Switching Log File.");

	//
	// Cycle Log File
	//
	MF_StatusFree();

	char* LogFileOld = (char*)_mfd_malloc(MAX_PATH,"LogFileOld");
	char* LogFileNew = (char*)_mfd_malloc(MAX_PATH,"LogFileNew");
	if (LogFileOld == NULL)	{ MF_OutOfMemoryHandler();	MF_StatusUI_Update ( "*** ERROR CYCLING LOG *** (Alloc Failed)" );	return false; }
	if (LogFileNew == NULL)	{ MF_OutOfMemoryHandler();	MF_StatusUI_Update ( "*** ERROR CYCLING LOG *** (Alloc Failed)" );	return false; }

	sprintf( LogFileOld , "%sMailFlt.log"  , MF_GlobalConfiguration->LogDirectory.c_str());
	sprintf( LogFileNew , "%sMailFlt.%03i" , MF_GlobalConfiguration->LogDirectory.c_str() , MFT_StatusLogCycledOnDay);

	unlink ( LogFileNew );

	// send the logfile while it is closed down ...
	MF_StatusSendDailyReport(LogFileOld);	
	
	if ( rename ( LogFileOld , LogFileNew ) )
	{
		MF_StatusUI_Update ( MF_Msg(MSG_LOGGING_ERROR_CYCLE) );
		failed = true;
	}

	// free in reverse order.
	_mfd_free(LogFileNew,"LogFileNew");
	_mfd_free(LogFileOld,"LogFileOld");

	if ( failed && MF_GlobalConfiguration->NotificationAdminLogs )
		MF_EMailPostmasterGeneric("Log Cycling Failed","An error occoured while cycling the log files.","","");

	return MF_StatusInit();
}

// Cycle Log File at ~ 23:00 ...
bool MF_StatusCycleLog()
{
	bool rc = false;
	time_t curTime;
	tm     curTimeTM;

	time ( &curTime );
	curTimeTM = *localtime ( &curTime );

	if (MFT_StatusLogCycledOnDay == curTimeTM.tm_mday)
		return true;

	if (curTimeTM.tm_hour < 23)		// this is 23:xx
		return true;

	MFT_StatusLogCycledOnDay = curTimeTM.tm_mday;

	rc = MF_StatusCycleLog2();
	
	MFUtil_CheckCurrentVersion();
	
	return rc;
}

// Write to log file
void MF_StatusLog( const char *newText )
{
	int tmp_sLen = 0;
	time_t tmp_ltime;

	if (!MF_Status_Initialized)
		return;

	if (MF_Log_FileHandle == NULL)
		return;

	tmp_sLen = (int)strlen(newText);
	if (tmp_sLen > 4096)	tmp_sLen=4096;
											
	if ( tmp_sLen != 0 )
	{
		//  >> Get Current Time 
		tmp_ltime = time(NULL); // &tmp_ltime );
		//  >> Build log string
		sprintf(tmp_MF_LogText , "%.24s ", ctime( &tmp_ltime ) );
		strncat(tmp_MF_LogText , newText , (unsigned int)tmp_sLen );
		tmp_MF_LogText[4160]=0;	// ZERO-TERMINATE STRING
		if ( tmp_MF_LogText[strlen(tmp_MF_LogText)-2] != '\n' )
			strcat(tmp_MF_LogText , "\n" );

		tmp_MF_LogText[4160]=0;	// ZERO-TERMINATE STRING

		//  >> Write to log.
		fputs( tmp_MF_LogText , MF_Log_FileHandle );
		
	}

	// Commit Log Data to disk.
	fflush(MF_Log_FileHandle);
}

// Set Status Text ...
void MF_StatusText( const char *newText )
{
	char statusText[82];

	if (!MF_Status_Initialized)
		return;			// Silent

	if (strlen(newText)>0)
	{	
		strncpy(statusText,newText,79);
		statusText[79]=0;
#ifndef WIN32
		MF_StatusLog( newText );
		MF_StatusUI_UpdateLog(newText);
#else
		MF_StatusLog( newText );
#endif
	} else {
		strcpy(statusText,MF_Msg(STATUS_MSG_IDLE));
	}

	statusText[79]=0;

	MF_StatusUI_Update("");

}

/*
 *
 *
 *  --- eof ---
 *
 *
 */
