/*+
 +		MFWin32.cpp
 +		
 +		Win32/Program Handling Routines
 +		
 +		This is copyrighted work! Copyright 2001, 2002 Christian Hofstädtler.
 +		
 +		
 +		- Sep/2001 ; ch   ; Initial Code
 +		
 +		
+*/

#ifdef WIN32

#define _MFD_MODULE			"MFWin32.CPP"

// Using _MAIN_ to get variables defined here.
#define _MAILFILTER_MAIN_

// Include MF.h
#include "..\..\Main\MailFilter.h"

#undef _MAILFILTER_MAIN_
// And undef it ... ;)

// Thread Group ID
static int mainThread_ThreadGroupID = 1;
// Event Handle for Server ShutDown event
static LONG eventHandleShutDown = (LONG)-1;
// Time ...
static tm tmp_TimeTM;
static time_t tmp_Time;

HINSTANCE MFT_Win32_HInstance; 
HDC MFT_Win32_WindowDC;
HWND MFT_Win32_Window = NULL; 
LONG MFT_Win32_Status_LastTop = -1;

void MF_StatusUI_Update(int newText)
{
	MF_StatusUI_Update(MF_Msg(newText));
}
void MF_StatusUI_Update(const char* newText)
{
	if (strlen(newText) > 0)
	{	printf(newText);
		printf("\n");
	}
}

void MF_StatusUI_UpdateLog(const char* newText)
{
	MF_StatusUI_Update(newText);
}


#ifndef _MF_CLEANBUILD
void MFD_Out_func(int attr, const char* format, ...) {

	va_list	argList;
	bool show = ( chkFlag(MFT_Debug,attr) == 1 );

	if (!MFT_Verbose)
		return;

	if (MFT_NLM_Exiting)
		return;

	switch (attr)
	{
		case MFD_SOURCE_GENERIC:	{	attr = COLOR_SILVER;	break;	}
		case MFD_SOURCE_WORKER:		{	attr = COLOR_CYAN;		break;	}
		case MFD_SOURCE_CONFIG:		{	attr = COLOR_GREEN;		break;	}
		case MFD_SOURCE_VSCAN:		{	attr = COLOR_BLUE;		break;  }
		case MFD_SOURCE_ZIP:		{	attr = COLOR_TEAL;		break;  }
		case MFD_SOURCE_ERROR:		{	attr = COLOR_RED;		break;  }
		case MFD_SOURCE_RULE:		{	attr = COLOR_YELLOW;	break;	}
		case MFD_SOURCE_MAIL:		{	attr = COLOR_PURPLE;	break;	}
		case MFD_SOURCE_SMTP:		{	attr = COLOR_WHITE;		break;	}
		default:					{	attr = COLOR_GREY;		break;  }
	}

	if (!show)
		return;

	va_start(argList, format);

/*#ifndef __NOVELL_LIBC__

	int oldScreen;    
	oldScreen=SetCurrentScreen(MFD_ScreenID);   
	
	vprintf(format, argList);
	SetCurrentScreen(oldScreen);

#else

	char buffer[5000];
	vsnprintf(buffer,4999,format,argList);
	
	OutputToScreenWithAttribute(MFD_ScreenID,(unsigned char)attr,buffer);
	
#endif*/
	vprintf(format,argList);

	va_end (argList);
}
#endif	//!_MF_CLEANBUILD

int __cdecl ConsolePrintf(const char* format, ... )
{

	va_list marker;
	va_start( marker, format );

	TCHAR* buf = (TCHAR*)malloc(8192*2);

	int rc = vsprintf( buf , format, marker ); 

	va_end( marker );

	MF_StatusUI_Update(buf);
	MF_StatusLog(buf);

	free(buf);

	return rc;
}



#ifndef _MF_CLEANBUILD
void MFD_Out_func(const char* format, ...) {

/*	int oldScreen;    
*/
	if (!MFT_Verbose)
		return;

	va_list	argList;
	va_start(argList, format);
/*
	oldScreen=SetCurrentScreen(MFD_ScreenID);   
	
	*/vprintf(format, argList);/*
//	
//	ConsolePrintf(format,argList);

	SetCurrentScreen(oldScreen);

	*/
	va_end (argList);
}
#endif	//!_MF_CLEANBUILD



void MFL_VerInfo()
{
	MF_StatusText(MF_Msg(MSG_EVAL_LINE1));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE2));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE3));
	MF_StatusText(MF_Msg(MSG_EVAL_LINE4));
	MF_StatusNothing();
}

/*void Win32_SignalHandler(int sig)
{
	int handlerThreadGroupID;

	switch(sig)
	{
		case SIGTERM:
			{
				MFT_NLM_Exiting = 255;
				
				if (MF_Thread_Run > 0)
					ResumeThread(MF_Thread_Run);

				MF_StatusText("Unload will continue when all Threads have terminated ...");

			}
			break;
		case SIGINT:
			{
				signal(SIGINT, NLM_SignalHandler);
				break;
			}
	}
	return;
}
*/


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
	ConsolePrintf("\nThis is MailFilter/ax professional for Win32.\n  Copyright 2001-2003 Christian Hofstaedtler.\n  All Rights Reserved.\n");
	ConsolePrintf(MF_Msg(CONMSG_MAIN_MODULEINFO));

extern int MF_ParseCommandLine( int argc, char **argv );

	// Parse Command Line and build some vars...
	if (!MF_ParseCommandLine(argc,argv))
		goto MF_WIN32_MAIN_TERMINATE;
	
	// Read Configuration from File
	if (!MF_ConfigRead())
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
		ConsolePrintf(MF_Msg(CONMSG_MAIN_ERRTHREADSTART));
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
		ConsolePrintf("***FATAL ERROR***  -- Error Code: %d\n",rc);
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

