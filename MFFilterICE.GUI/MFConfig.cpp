// MFConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MFConfig.h"
//#include "MFConfigDlg.h"
#include "FilterlistDlg.h"
#include <sys/stat.h>
#include "../MailFilter/Main/MFVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CMFConfigApp

BEGIN_MESSAGE_MAP(CMFConfigApp, CWinApp)
//	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMFConfigApp construction

CMFConfigApp::CMFConfigApp()
{
}


// The one and only CMFConfigApp object

CMFConfigApp theApp;


// CMFConfigApp initialization

BOOL CMFConfigApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	this->szVersion = "Version "MAILFILTERVERNUM;

	AfxEnableControlContainer();

	CWinApp::InitInstance();

	int ci = 0;
	CString tmp;

	CFilterlistDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	/*	do nothing
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
		//  dismissed with Cancel
	}
	*/


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
