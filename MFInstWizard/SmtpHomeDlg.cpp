// SmtpHomeDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "SmtpHomeDlg.h"


// SmtpHomeDlg-Dialogfeld

IMPLEMENT_DYNAMIC(SmtpHomeDlg, CPropertyPage)
SmtpHomeDlg::SmtpHomeDlg()
	: CPropertyPage(SmtpHomeDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->dwFlags &= ~PSP_HASHELP;
	m_pPSP->pszHeaderTitle = "GroupWise Internet Agent Details";
	m_pPSP->pszHeaderSubTitle = "SmtpHome Conflict Resolution";
}

SmtpHomeDlg::~SmtpHomeDlg()
{
}

void SmtpHomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	CInstApp* app = ((CInstApp*)AfxGetApp());
	DDX_Text(pDX, IDC_DHOME, app->mf_GwiaDHome);
	DDX_Text(pDX, IDC_SMTPHOME, app->mf_GwiaSmtpHome);
	DDX_Check(pDX, IDC_RESET_SMTPHOME, app->mf_GwiaResetSmtpHome);
}


BEGIN_MESSAGE_MAP(SmtpHomeDlg, CPropertyPage)
END_MESSAGE_MAP()


// SmtpHomeDlg-Meldungshandler
BOOL SmtpHomeDlg::OnWizardFinish(void)
{
	this->UpdateData(TRUE);
	return CPropertyPage::OnWizardFinish();
}

BOOL SmtpHomeDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	psheet->SetFinishText("Install");

	return CPropertyPage::OnSetActive();
}