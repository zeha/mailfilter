// MFConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MFConfig.h"
#include "MFConfigDlg.h"
#include "../MailFilter/Config/MFConfig.h"
#include "mf_c.h"
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int MF_ConfigRead();
extern int MF_ConfigWrite();
extern void MF_ConfigFree();

// CMFConfigApp

BEGIN_MESSAGE_MAP(CMFConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFConfigApp construction

CMFConfigApp::CMFConfigApp()
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFConfigApp object

CMFConfigApp theApp;


CString GetRegValue( HKEY hRoot , LPTSTR lpszSubKey , LPTSTR lpszValue , LPTSTR lpszDefault )
{
	LONG rc;
	HKEY hKey;
	CString szBuf;
	char szCharArray[2*MAX_PATH];
    DWORD dwBufLen = 2*MAX_PATH;

	memset(szCharArray,0,dwBufLen-1);

	rc = RegCreateKey(hRoot,
					lpszSubKey,
                    &hKey);
	

	rc = RegQueryValueEx(hKey,
					lpszValue,
                    NULL,
                    NULL,
					(LPBYTE)szCharArray,
                    &dwBufLen);

	rc = RegCloseKey( hKey );
	
	szBuf.Empty();
	szBuf.Append(szCharArray);

	if (szBuf.GetLength() == 0) { szBuf.SetString(lpszDefault); }
	szBuf = szBuf.Trim();
	return szBuf;

}
void SetRegValue( HKEY hRoot , LPTSTR lpszSubKey , LPTSTR lpszValue , LPTSTR lpszData )
{
	LONG rc;
	HKEY hKey;

	rc = RegCreateKey(hRoot,
					lpszSubKey,
                    &hKey);

	rc = RegSetValueEx(hKey,lpszValue,NULL,REG_SZ,(LPBYTE)lpszData,(DWORD)strlen(lpszData)); 

	rc = RegCloseKey( hKey );

	return;

}
// CMFConfigApp initialization
BOOL CMFConfigApp::DoInitConfig(CString ConfigPath)
{
	sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"%s\\CONFIG.BIN",ConfigPath);
	sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"%s\\CONFIG.BAK",ConfigPath);
	sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"%s\\CONFIG.ERR",ConfigPath);								
	
	if (strlen(ConfigPath) < 5)
	{	struct stat st;
		if (stat(MFC_ConfigFile,&st))
		{
			ConfigPath.Format("%c:\\ETC\\MAILFLT",ConfigPath.GetAt(0) );
			sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"%s\\CONFIG.BIN",ConfigPath);
			sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"%s\\CONFIG.BAK",ConfigPath);
			sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"%s\\CONFIG.ERR",ConfigPath);								
		} else {
			AfxMessageBox(st.st_dev ,MB_OK,0);

		}
	}

	if (!MF_ConfigRead())
	{
		CString szMessage = "Error reading the configuration at \"";
		szMessage.Append(ConfigPath);
		szMessage.Append("\", do you want to select another installation?");
		if (AfxMessageBox(szMessage,MB_YESNO,NULL) == IDYES)
		{
			CString szBuffer;
			
			BROWSEINFO bi;
			bi.hwndOwner = NULL;
			bi.pidlRoot = NULL;
			bi.lpfn = NULL;		// callback function pointer
			bi.lpszTitle = "Select MailFilter/ax Installation Directory:";
			bi.ulFlags = BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
			bi.pszDisplayName = szBuffer.GetBufferSetLength(MAX_PATH);

			LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
			long ret = SHGetPathFromIDList(pidl, szBuffer.GetBufferSetLength(MAX_PATH));

			CoTaskMemFree(pidl);
			CoTaskMemFree((void*)bi.pidlRoot);

			return this->DoInitConfig(szBuffer);
		} else
			return FALSE;
	} else {
		SetRegValue( HKEY_LOCAL_MACHINE , "SOFTWARE\\Hofstaedtler IE GmbH\\MailFilter", "InstallDir" , ConfigPath.GetBuffer() );
		this->MF_ConfigPath = ConfigPath;
		return TRUE;
	}
}

BOOL CMFConfigApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	this->szVersion = "Version 1.5 (1118)";

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	CWinApp::InitInstance();

	MF_ConfigPath = GetRegValue( HKEY_LOCAL_MACHINE , "SOFTWARE\\Hofstaedtler IE GmbH\\MailFilter", "InstallDir" , "" );
	if (!this->DoInitConfig(MF_ConfigPath))
		return FALSE;

	int ci = 0;
	CString tmp;

#define ADDVALUE(name,value)		m_CMFC.AddValueChar(name, value);
#define ADDVALUEI(name,value)		m_CMFC.AddValueInt(name,value);
#define ADDVALUEB(name,value)		m_CMFC.AddValueBool(name,((bool)(value)));
#define ADDHEADER(name)				m_CMFC.AddHeader(name);

	ADDVALUE ( "GWIA Root" ,						MFC_GWIA_ROOT						);
	ADDVALUE ( "MailFilter Root" ,					MFC_MFLT_ROOT						);
	ADDVALUE ( "Domain Name" ,						MFC_DOMAIN_Name						);
	ADDVALUE ( "Hostname FQDN" ,					MFC_DOMAIN_Hostname					);
	ADDVALUE ( "Postmaster E-Mail" ,				MFC_DOMAIN_EMail_PostMaster			);
	ADDVALUE ( "MailFilter E-Mail" ,				MFC_DOMAIN_EMail_MailFilter			);
	ADDVALUE ( "Multi2One Address" ,				MFC_Multi2One						);
	ADDVALUE ( "Scheduling Times",					MFC_BWL_ScheduleTime				);
	ADDVALUEB( "PFA Single POP Functionality",		MFC_EnablePFAFunctionality			);
	ADDVALUEI( "GWIA Version Mode" ,				MFC_GWIA_Version					);
	ADDVALUEB( "Incoming Rcpt Rule Check",			MFC_EnableIncomingRcptCheck			);
	ADDHEADER( "Virus Scan:"															);
	ADDVALUEI( "  Queue Count",						MFC_MAILSCAN_Scan_DirNum			);
	ADDVALUEI( "  Queue Time",						MFC_MAILSCAN_Scan_DirWait			);
	ADDVALUEB( "  Decode Attachments",				MFC_EnableAttachmentDecoder			);
	ADDHEADER( "Default Notify:"														);
	ADDVALUEB( "  Internal Recipient",				MFC_Notification_EnableInternalRcpt	);
	ADDVALUEB( "  Internal Sender",					MFC_Notification_EnableInternalSndr	);
	ADDVALUEB( "  External Recipient",				MFC_Notification_EnableExternalRcpt	);
	ADDVALUEB( "  External Sender",					MFC_Notification_EnableExternalSndr	);
	ADDHEADER( "Logging:"																);
	ADDVALUEB( "  Notify on Cycle Error",			MFC_Notification_AdminLogs			);
	ADDVALUEB( "  Send Daily Status Report",		MFC_Notification_AdminDailyReport	);
	ADDHEADER( "Problem Directory Cleanup:"												);
	ADDVALUEI( "  Maximum Total Size (kB)",			MFC_ProblemDir_MaxSize				);
	ADDVALUEI( "  Maximum File Age (Days)",			MFC_ProblemDir_MaxAge				);
	ADDVALUEB( "  Notify Admin",					MFC_Notification_AdminMailsKilled	);

	CMFConfigDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		MF_ConfigWrite();
	}/*	do nothing
	else if (nResponse == IDCANCEL)
	{
		//  dismissed with Cancel
	}
	*/

	MF_ConfigFree();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
