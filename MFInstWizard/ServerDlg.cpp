// ServerDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "ServerDlg.h"
#include "netwareapi.h"
#include "serverdlg.h"
#include ".\serverdlg.h"

// ServerDlg-Dialogfeld

IMPLEMENT_DYNAMIC(ServerDlg, CPropertyPage)
ServerDlg::ServerDlg()
	: CPropertyPage(ServerDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->dwFlags &= ~PSP_HASHELP;
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
	DDX_Control(pDX, IDC_SERVERLIST, m_ServerListCtrl);
	DDX_LBString(pDX, IDC_SERVERLIST, app->mf_ServerName);
}

BEGIN_MESSAGE_MAP(ServerDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_REFRESHLIST, OnBnRefreshList)
	ON_LBN_SELCHANGE(IDC_SERVERLIST, OnLbnSelChangeServerList)
END_MESSAGE_MAP()


// ServerDlg-Meldungshandler

LRESULT ServerDlg::OnWizardNext(void)
{
	this->UpdateData(TRUE);

	BeginWaitCursor();

	CInstApp* app = ((CInstApp*)AfxGetApp());

	if (app->mf_ServerName != "")
	{
		NetwareApi api;
		if (!api.SelectServerByName(app->mf_ServerName))
		{
			EndWaitCursor();
			MessageBox("A valid connection to the specified server could not be found.\nPlease make sure you are logged into the server.",0,MB_ICONEXCLAMATION);
			((CEdit*)this->GetDlgItem(IDC_SERVERLIST))->SetFocus();
			return -1;
		}
		if (!api.CheckConsoleOperatorRight())
		{
			EndWaitCursor();
			MessageBox("You do not have Console Operator privileges on the specified server.\nPlease make sure you are logged in as Admin or equivalent.",0,MB_ICONEXCLAMATION);
			((CEdit*)this->GetDlgItem(IDC_SERVERLIST))->SetFocus();
			return -1;
		}
	} else {
		EndWaitCursor();
		MessageBox("You have not selected a NetWare Server. Please select a Server as the target for the MailFilter Installation.",0,MB_ICONEXCLAMATION);
		((CEdit*)this->GetDlgItem(IDC_SERVERLIST))->SetFocus();
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
	EndWaitCursor();

	return CPropertyPage::OnWizardNext();
}

static void FillServerList(CExtendedListBox &listCtrl)
{
	CStringArray serverList;

	NetwareApi api;
	api.GetServerList(serverList);

	listCtrl.ResetContent();

	for (int i = 0; i < serverList.GetSize(); i++)
	{
		int nItem = listCtrl.AddString(serverList[i]);
		api.SelectServerByName(serverList[i]);
		if (api.CheckConsoleOperatorRight())
			listCtrl.SetItemData(nItem,1);
		else
			listCtrl.SetItemData(nItem,0);
	}
}

BOOL ServerDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_NEXT|PSWIZB_BACK);

	FillServerList(this->m_ServerListCtrl);

	return CPropertyPage::OnSetActive();
}

void ServerDlg::OnBnRefreshList()
{
	FillServerList(this->m_ServerListCtrl);
}

void ServerDlg::OnLbnSelChangeServerList()
{
	this->UpdateData(TRUE);
	CInstApp* app = ((CInstApp*)AfxGetApp());

	NetwareApi api;
	api.SelectServerByName(app->mf_ServerName);
	unsigned int mj; unsigned int mi; unsigned int rev;
	api.GetServerVersion(mj,mi,rev);
	CString szVersion;
	szVersion.Format("Version %d.%d.%d",mj,mi,rev);
	this->SetDlgItemText(IDC_SERVERVERSION,szVersion);
}
