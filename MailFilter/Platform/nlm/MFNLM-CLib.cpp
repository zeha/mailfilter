/*
 +
 +		MFNLM-CLib.cpp
 +		
 +		NetWare Platform Specific Functions
 +		for CLib
 +
 +		
 +		Copyright 2001-2004 Christian Hofstädtler.
 +		
 +		
 +
 +
 +
 +		
 */

#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)

#define _MFD_MODULE			"MFNLM-CLib.CPP"

// Include MailFilter.h
#include "MailFilter.h"

bool MF_NLM_OpenDebugScreen()
{
	MFD_ScreenID = CreateScreen ( "MailFilter Debug Screen", DONT_AUTO_ACTIVATE | DONT_CHECK_CTRL_CHARS | AUTO_DESTROY_SCREEN );
	return true;
}

bool MF_LoginUser()
{
	return false;
}

extern "C" {

	void MF_DisplayCriticalError(const char* format, ...)
	{
		va_list	argList;
		va_start(argList, format);

		char buffer[5000];
		vsprintf(buffer,format,argList);
		buffer[4999] = 0;

		int oldScreen = 0;
		oldScreen=SetCurrentScreen(0);
		
		ConsolePrintf(buffer);
//		vfprintf(stderr,format,argList);

		if (oldScreen)		SetCurrentScreen(oldScreen);

		va_end (argList);
	}
}

bool MailFilterApp_Server_LoginToServer()
{	
	if (
		 (MF_GlobalConfiguration.LoginUserName == "") &&
		 (MF_GlobalConfiguration.LoginUserPassword == "")
		)
		return true;		// dont login

	MF_DisplayCriticalError("MAILFILTER WARNING: This MailFilter has limited functionality\n\tand CAN NOT log into the server.\n\tPlease upgrade to a newer NetWare OS and MailFilter Version!\n");

	return true;
}

bool MailFilterApp_Server_SelectServerConnection()
{
	return true;
}

bool MailFilterApp_Server_SelectUserConnection()
{
	return true;
}

bool MailFilterApp_Server_LogoutFromServer()
{
	return true;
}

#endif // N_PLAT_NLM && !__NOVELL_LIBC__

/*
 *  --- eof ---
 */
