// WelcomeDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "WelcomeDlg.h"
#include "../MailFilter/Main/MFVersion.h"

// WelcomeDlg-Dialogfeld

IMPLEMENT_DYNAMIC(WelcomeDlg, CPropertyPage)
WelcomeDlg::WelcomeDlg()
	: CPropertyPage(WelcomeDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_HIDEHEADER;
	m_pPSP->dwFlags &= ~PSP_HASHELP;
}

WelcomeDlg::~WelcomeDlg()
{
}

void WelcomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(WelcomeDlg, CPropertyPage)
END_MESSAGE_MAP()


// WelcomeDlg-Meldungshandler
BOOL WelcomeDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_NEXT);

	CFont *headerFont = new CFont();
	VERIFY(headerFont->CreateFont(-14,0,0,0,FW_BOLD,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,"Verdana"));

	CStatic* headerCtrl = ((CStatic*)this->GetDlgItem(IDC_TITLE));
	headerCtrl->SetFont(headerFont);

	CStatic* versionCtrl = ((CStatic*)this->GetDlgItem(IDC_VERSION));
	versionCtrl->SetWindowText("Version "MAILFILTERVERNUM);

	return CPropertyPage::OnSetActive();
}