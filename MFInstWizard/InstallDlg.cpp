// InstallDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "InstallDlg.h"


// InstallDlg-Dialogfeld

IMPLEMENT_DYNAMIC(InstallDlg, CPropertyPage)
InstallDlg::InstallDlg()
	: CPropertyPage(InstallDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_HIDEHEADER;
	m_pPSP->dwFlags &= ~PSP_HASHELP;
}

InstallDlg::~InstallDlg()
{
}

void InstallDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	CInstApp* app = (CInstApp*)AfxGetApp();
	DDX_Text(pDX, IDC_SERVER, app->mf_ServerName);
	DDX_Check(pDX, IDC_CHECK1, app->mf_LoadMailFilter);
}


BEGIN_MESSAGE_MAP(InstallDlg, CPropertyPage)
END_MESSAGE_MAP()


// InstallDlg-Meldungshandler
BOOL InstallDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetFinishText("Install");
	psheet->SetWizardButtons(PSWIZB_FINISH|PSWIZB_BACK);

	CFont *headerFont = new CFont();
	VERIFY(headerFont->CreateFont(-14,0,0,0,FW_BOLD,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,"Verdana"));

	CStatic* headerCtrl = ((CStatic*)this->GetDlgItem(IDC_TITLE));
	headerCtrl->SetFont(headerFont);

	CInstApp* app = (CInstApp*)AfxGetApp();
	if (app->mf_IsUpgrade)
		headerCtrl->SetWindowText("Upgrade MailFilter");
	else
		headerCtrl->SetWindowText("Finish Installation");

	return CPropertyPage::OnSetActive();
}
