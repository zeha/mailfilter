/*
 +
 +		MFNLM-LibC.cpp
 +		
 +		NetWare Platform Specific Functions
 +		for LibC
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

#define _MFD_MODULE			"MFNLM-LibC.CPP"

// Include MailFilter.h
#include "MailFilter.h"

#if defined(__NOVELL_LIBC__)

#include <sys/utsname.h>
#include <nks/netware.h>
#include <nks/dirio.h>
#include <client.h>

static int (*dynaload_OpenCustomScreen) (const char *name, rtag_t rTag, scr_t *newScrID, int mode);
			
int DL_OpenCustomScreen (const char *name, rtag_t rTag, scr_t *newScrID, int mode)
{
	int dynaloadReturnValue;
	
	dynaload_OpenCustomScreen = (int(*)(const char*, rtag_t, scr_t*, int))
				ImportPublicObject(getnlmhandle(),"OpenCustomScreen");

	if (dynaload_OpenCustomScreen == NULL)
	{
		MF_DisplayCriticalError("MAILFILTER Notice: Reverting to OpenScreen instead of OpenCustomScreen.\n");
		return ENOSYS;
	}
	
	dynaloadReturnValue = (*dynaload_OpenCustomScreen) (name,rTag,newScrID,mode);
	
	UnImportPublicObject(getnlmhandle(),"OpenCustomScreen");
	
	return dynaloadReturnValue;
}


bool MF_NLM_OpenDebugScreen()
{
//	int rc;
	MFD_ScreenTag = AllocateResourceTag( (void*)MF_NLMHandle, "MailFilter Debug Screen", ScreenSignature);
//	if ((rc = DL_OpenCustomScreen ( "MailFilter Debug", MFD_ScreenTag, &MFD_ScreenID, SCREEN_MODE_80X25|SCREEN_MODE_SCROLLABLE )) != 0)
//	{
		if (OpenScreen ( "MailFilter Debug Screen", MFD_ScreenTag, &MFD_ScreenID ) )
			return false;
//	}
//	MF_DisplayCriticalError("MAILFILTER Notice: OpenCustomScreen returned %d.\n",rc);

	return true;
}

void _NonAppStop ( void )
{
	MF_StatusFree();
	MF_ExitProc();
	system("UNLOAD MFAVA.NLM");
}

extern bool MF_UI_PromptUsernamePassword(std::string prompt, std::string username, std::string password);

bool MF_LoginUser()
{
	std::string prompt = "";
	std::string user = "";
	std::string pass = "";
	MF_UI_PromptUsernamePassword(prompt,user,pass);

	return true;
}

extern "C" {

	void MF_DisplayCriticalError(const char* format, ...)
	{
		va_list	argList;
		va_start(argList, format);

		OutputToScreenWithPointer( 0, format, argList );

		va_end (argList);
	}

}

static NXPathCtx_t	MFT_PathCtxt_ServerConnection = 0;
static NXPathCtx_t	MFT_PathCtxt_UserConnection = 0;
static int			MFT_ServerIdentity = 0;
static bool			MFT_IdentitiesOkay = false;

bool MailFilterApp_Server_LoginToServer()
{	
	int            err; 

	if (MFT_IdentitiesOkay)
		return true;

	MFT_IdentitiesOkay = false;

	if (
		 (MF_GlobalConfiguration.LoginUserName == "") &&
		 (MF_GlobalConfiguration.LoginUserPassword == "")
		)
		return true;		// we shouldnt log in?

	err = create_identity ("", MF_GlobalConfiguration.LoginUserName.c_str(), MF_GlobalConfiguration.LoginUserPassword.c_str(),
								NULL, XPORT_WILD|USERNAME_ASCII, &MFT_ServerIdentity);
	if (err)
	{
		MF_DisplayCriticalError(" * Details: create_identity failed with rc=%d, errno: %d\n",err,errno);
		return false;
	}
	
	if (!is_valid_identity(MFT_ServerIdentity, &err))
	{
		MF_DisplayCriticalError(" * No Valid Identity! rc: %d\n",err);
		return false;
	}

	err = NXCreatePathContext(0, "SYS:\\SYSTEM", NX_PNF_DEFAULT, NULL, &MFT_PathCtxt_ServerConnection);
	if (err)	return false;

	err = NXCreatePathContext(0, "SYS:\\SYSTEM", NX_PNF_DEFAULT, (void *) MFT_ServerIdentity, &MFT_PathCtxt_UserConnection);
	if (err)	return false;

	MFT_IdentitiesOkay = true;

	return true;
}

bool MailFilterApp_Server_SelectServerConnection()
{
	if (!MFT_IdentitiesOkay)
		return true;

	int err = setcwd(MFT_PathCtxt_ServerConnection);
	if (err)	return false;
	
	return true;
}

bool MailFilterApp_Server_SelectUserConnection()
{
	if (!MFT_IdentitiesOkay)
		return true;

	int err = setcwd(MFT_PathCtxt_UserConnection);
	if (err)	return false;
	
	return true;
}

bool MailFilterApp_Server_LogoutFromServer()
{
	if (!MFT_IdentitiesOkay)
		return true;

	MFT_IdentitiesOkay = false;

	NXFreePathContext(MFT_PathCtxt_ServerConnection);
	MFT_PathCtxt_ServerConnection = 0;
	
	NXFreePathContext(MFT_PathCtxt_UserConnection);
	MFT_PathCtxt_UserConnection = 0;
	
	delete_identity(MFT_ServerIdentity);
	MFT_ServerIdentity = 0;
	return true;
}

#endif // N_PLAT_NLM && __NOVELL_LIBC__

/*
 *  --- eof ---
 */
