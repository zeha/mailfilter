/*+
 +		MFWin32.cpp
 +		
 +		Win32/Main
 +		
 +		
 +		
 +		
 +		
+*/

#ifdef WIN32

#define _MFD_MODULE			"MFWin32.CPP"

// Using _MAIN_ to get variables defined here.
#define _MAILFILTER_MAIN_

// Include MF.h
#include "..\..\Main\MailFilter.h"

//
//  Main Program Function
//
//   * Startup
//   * Read Configuration
//   * Open Semaphore(s) ...
//   * Start Thread(s) ...
//   * Close Main Thread.
//
//int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, 
//    LPSTR lpCmdLine, int nCmdShow) 
int main( int argc, char **argv )
{

	DWORD dwThreadId, dwThrdParam = 1;
	
	MFT_NLM_ThreadCount++;

	// Print url etc. to console
	printf("\nThis is MailFilter professional for Win32.\n  Copyright 2001-2004 Christian Hofstaedtler.\n  All Rights Reserved.\n");
	printf(MF_Msg(CONMSG_MAIN_MODULEINFO));

extern int MF_ParseCommandLine( int argc, char **argv );

	// Parse Command Line and build some vars...
	if (!MF_ParseCommandLine(argc,argv))
		goto MF_WIN32_MAIN_TERMINATE;
	
	// Read Configuration from File
	MF_GlobalConfiguration->config_mode_strict = true;
	if (!MF_GlobalConfiguration->ReadFromFile(""))
		goto MF_WIN32_MAIN_TERMINATE;

	// Init Status
	if (!MF_StatusInit())
		goto MF_WIN32_MAIN_TERMINATE;

	MF_UI_ShowConfiguration();

	// Start Thread
	MF_Thread_Work = CreateThread(NULL,0,MF_Work_Startup,&dwThrdParam,0,&dwThreadId);
	sleep(1);
	if( MF_Thread_Work == NULL )
	{
		printf(MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
		MF_Thread_Work = 0;
		goto MF_WIN32_MAIN_TERMINATE;
	} else {
		MF_StatusText("Thread OK!");
//		RenameThread(MF_Thread_Run,programMesgTable[THREADNAME_WORK]);		// 15 (16?) Chars max.
	}

/*	MFD_Out("Starting SMTP Thread...\n");

	// Start Thread: ** SMTP **
	MF_Thread_SMTP = CreateThread(NULL,0,MF_SMTP_Startup,&dwThrdParam,0,&dwThreadId);			// Set 48k Stack for new thread
	if( MF_Thread_SMTP == NULL )
	{
		ConsolePrintf(MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
		MF_Thread_SMTP = 0;
		goto MF_WIN32_MAIN_TERMINATE;
	} else {
		MF_StatusText("SMTP Thread Startup Complete.");
	}
*/
	//MFT_NLM_ThreadCount--;

	while(MFT_NLM_ThreadCount)
	{
		sleep(1);
	}

	// Begin the operation and continue until it is complete 
	// or until the user clicks the mouse or presses a key. 
 
	// Pretend that we've already exited, as we are only waiting for the other threads to exit ...
	//MFT_NLM_ThreadCount--;
	

	GetLastError();

MF_WIN32_MAIN_TERMINATE:
	DWORD rc = GetLastError();
	char xy[1024]; xy[0]=0;
	if (rc != 0)
	{
//		itoa(rc,xy,10);
//		MessageBox(NULL,xy,"Error Number:",MB_OK);
//		MessageBox(NULL,"Error during Execution.\nMailFilter is terminating.","MailFilter",MB_ICONSTOP);
		printf("***FATAL ERROR***  -- Error Code: %d\n",rc);
	}
	return 0;
}


#endif // WIN32

/*
 *
 *
 *  --- eof ---
 *
 *
 */

