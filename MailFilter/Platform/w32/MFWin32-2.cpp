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

#define _MFD_MODULE			"MFWin32-2.CPP"

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
	int col;
	bool show = ( chkFlag(MFT_Debug,attr) == 1 );

	if (!MFT_Debug)
		return;

	if (MFT_NLM_Exiting)
		return;

	switch (attr)
	{
		case MFD_SOURCE_GENERIC:	{	col = FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED;	break;	}
		case MFD_SOURCE_WORKER:		{	col = FOREGROUND_GREEN|FOREGROUND_INTENSITY;	break;	}
		case MFD_SOURCE_CONFIG:		{	col = FOREGROUND_GREEN;							break;	}
		case MFD_SOURCE_VSCAN:		{	col = FOREGROUND_BLUE;							break;  }
		case MFD_SOURCE_ZIP:		{	col = FOREGROUND_BLUE|FOREGROUND_INTENSITY;		break;  }
		case MFD_SOURCE_ERROR:		{	col = FOREGROUND_RED|FOREGROUND_INTENSITY;		break;  }
		case MFD_SOURCE_RULE:		{	col = FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY;		break;	}
		case MFD_SOURCE_MAIL:		{	col = FOREGROUND_RED;							break;	}
		case MFD_SOURCE_SMTP:		{	col = FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;	break;	}
		default:					{	col = FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED;	break;  }
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

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, col);

	vprintf(format,argList);

	SetConsoleTextAttribute(hStdOut, (FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED));

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


int MailFilter_AV_Check()	{ return ENOENT; }
int MailFilter_AV_Init()	{ return ENOENT; }
int MailFilter_AV_ScanFile(const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType)
{
	return ENOENT;
}
int MailFilter_AV_DeInit() 	{ return 0; }
void MF_UI_ShowConfiguration(void)	{	return;	}
bool MailFilterApp_Server_SelectServerConnection(void)	{	return true;	}
bool MailFilterApp_Server_SelectUserConnection(void)	{	return true;	}


extern "C" {

	void MF_DisplayCriticalError(const char* format, ...)
	{
		va_list	argList;
		va_start(argList, format);
		vprintf(format,argList);
		va_end (argList);
	}

}

#endif // WIN32

/*
 *
 *
 *  --- eof ---
 *
 *
 */

