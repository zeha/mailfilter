// ProgressDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "ProgressDlg.h"


// ProgressDlg-Dialogfeld

IMPLEMENT_DYNAMIC(ProgressDlg, CPropertyPage)
ProgressDlg::ProgressDlg()
	: CPropertyPage(ProgressDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->pszHeaderTitle = "MailFilter Server Installation";
	m_pPSP->pszHeaderSubTitle = "Progress";
}

ProgressDlg::~ProgressDlg()
{
}

void ProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(ProgressDlg, CPropertyPage)
END_MESSAGE_MAP()


// ProgressDlg-Meldungshandler
BOOL ProgressDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_DISABLEDFINISH);

	return CPropertyPage::OnSetActive();
}