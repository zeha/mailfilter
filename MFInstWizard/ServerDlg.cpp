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
	m_pPSP->pszHeaderTitle = "Choose Server";
	m_pPSP->pszHeaderSubTitle = "Select the destination server.";
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

	// upgrade check
	app->mf_IsUpgrade = FALSE;

	CString mfpathCfg = "\\\\" + app->mf_ServerName + "\\SYS\\ETC\\MFPATH.CFG";
	CString oldMFPath = "";
	FILE* mfpathFile = fopen(mfpathCfg,"rt");
	if (mfpathFile != NULL)
	{
		char szTemp[MAX_PATH+1];
		memset(szTemp,0,MAX_PATH);
		fgets(szTemp,MAX_PATH,mfpathFile);
		oldMFPath = szTemp;
		fclose(mfpathFile);
	}
	if (oldMFPath == "")
	{
		CString oldSharedPath = "\\\\" + app->mf_ServerName + "\\SYS\\ETC\\MAILFLT\\CONFIG.BIN";
		CFileStatus status;
		if (CFile::GetStatus(oldMFPath + "\\CONFIG.BIN", status) == TRUE)
			oldMFPath = "\\\\" + app->mf_ServerName + "\\SYS\\ETC\\MAILFLT";

	}
	if (oldMFPath != "")
	{
		CFileStatus status;
		if (CFile::GetStatus(oldMFPath + "\\CONFIG.BIN", status) == TRUE)
		{
			if (AfxMessageBox("The Wizard detected a previous installation of MailFilter. Do you want to upgrade this installation?\n\nConfig Location: " + oldMFPath + "\n\nYes - Upgrade previous installation\nNo - Install new copy of MailFilter",MB_ICONQUESTION|MB_YESNO) == IDYES)
			{
				app->mf_IsUpgrade = TRUE;
				oldMFPath.MakeUpper();

				CString oldSharedBasePath = "\\\\" + app->mf_ServerName + "\\SYS\\ETC\\MAILFLT";
				oldSharedBasePath.MakeUpper();
				if (oldMFPath == oldSharedBasePath)
				{
					app->mf_SharedInstallation = TRUE;
				} else {
					// dig out volume and path
					if (oldMFPath.Left(2).Compare("\\\\") == 0)
					{
						oldMFPath.Delete(0,2);
						oldMFPath.Delete(0,oldMFPath.Find('\\')+1);
						oldMFPath.SetAt(oldMFPath.Find('\\'),':');
					}
					app->mf_AppDir = oldMFPath;
					AfxMessageBox("Note: using the following path to upgrade MailFilter:\n  " + app->mf_AppDir + "\nThis has to be a valid NetWare Server Path!",MB_ICONINFORMATION);
				}
			}
		}
	}
	
	// gwia.cfg detection
	if (!app->mf_IsUpgrade)
	{
		CString newGwiaPath = "\\\\";
		newGwiaPath += app->mf_ServerName;
		newGwiaPath += "\\SYS\\SYSTEM\\gwia.cfg";
		CFile file;
		if (file.Open(newGwiaPath,file.modeRead) == TRUE)
		{
			app->mf_GwiaCfgPath = newGwiaPath;
		}
	}

	EndWaitCursor();

	if (!app->mf_IsUpgrade)
		return CPropertyPage::OnWizardNext();
	else
		return IDD_INSTALL;
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
