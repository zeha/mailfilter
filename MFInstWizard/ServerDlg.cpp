// ServerDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "ServerDlg.h"
#include "netwareapi.h"
#include "serverdlg.h"


// ServerDlg-Dialogfeld

IMPLEMENT_DYNAMIC(ServerDlg, CPropertyPage)
ServerDlg::ServerDlg()
	: CPropertyPage(ServerDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->pszHeaderTitle = "Server Selection";
	m_pPSP->pszHeaderSubTitle = "Please select the destination server.";
}

ServerDlg::~ServerDlg()
{
}

void ServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	CInstApp* app = ((CInstApp*)AfxGetApp());
	DDX_Text(pDX, IDC_SERVERNAME, app->mf_ServerName);
	DDV_MaxChars(pDX, app->mf_ServerName, 128);
	DDX_Text(pDX, IDC_LICENSEKEY, app->mf_LicenseKey);
	DDV_MaxChars(pDX, app->mf_LicenseKey, 128);
}

BEGIN_MESSAGE_MAP(ServerDlg, CPropertyPage)
END_MESSAGE_MAP()


// ServerDlg-Meldungshandler

LRESULT ServerDlg::OnWizardNext(void)
{
	this->UpdateData(TRUE);

	CInstApp* app = ((CInstApp*)AfxGetApp());

	if (app->mf_ServerName != "")
	{
		NetwareApi api;
		api.SelectServerByName(app->mf_ServerName);
		if (!api.CheckConsoleOperatorRight())
		{
			MessageBox("You do not have Console Operator privileges on the specified server.\nPlease make sure you are logged in as Admin or equivalent.",0,MB_ICONEXCLAMATION);
			((CEdit*)this->GetDlgItem(IDC_SERVERNAME))->SetFocus();
			return -1;
		}
	} else {
		MessageBox("You have not entered a NetWare Server Name. Please enter a Server Name as the target for the MailFilter Installation.",0,MB_ICONEXCLAMATION);
		((CEdit*)this->GetDlgItem(IDC_SERVERNAME))->SetFocus();
		return -1;
	}

	CString newGwiaPath = "\\\\";
	newGwiaPath += app->mf_ServerName;
	newGwiaPath += "\\SYS\\SYSTEM\\gwia.cfg";
	CFile file;
	if (file.Open(newGwiaPath,file.modeRead) == TRUE)
	{
		app->mf_GwiaCfgPath = newGwiaPath;
	}

	return CPropertyPage::OnWizardNext();
}

BOOL ServerDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}
