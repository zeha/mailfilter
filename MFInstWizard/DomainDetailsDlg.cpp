// DomainDetailsDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "DomainDetailsDlg.h"


// DomainDetailsDlg-Dialogfeld

IMPLEMENT_DYNAMIC(DomainDetailsDlg, CPropertyPage)
DomainDetailsDlg::DomainDetailsDlg()
	: CPropertyPage(DomainDetailsDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->dwFlags &= ~PSP_HASHELP;
	m_pPSP->pszHeaderTitle = "Mail System Details";
	m_pPSP->pszHeaderSubTitle = "Details about your GroupWise installation and mail domain";
}

DomainDetailsDlg::~DomainDetailsDlg()
{
}

void DomainDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	CInstApp* app = ((CInstApp*)AfxGetApp());
	DDX_Text(pDX, IDC_DOMAINNAME, app->mf_DomainName);
	DDV_MaxChars(pDX, app->mf_DomainName, 128);
	DDX_Text(pDX, IDC_HOSTNAME, app->mf_HostName);
	DDV_MaxChars(pDX, app->mf_HostName, 128);
	
	INT foo = 0;
	if (app->mf_GroupwiseVersion6)
		foo = 0;
	else
		foo = 1;
	DDX_Radio(pDX, IDC_GW_VERSION6, foo);
	app->mf_GroupwiseVersion6 = (foo == 0);
}


BEGIN_MESSAGE_MAP(DomainDetailsDlg, CPropertyPage)
END_MESSAGE_MAP()


// DomainDetailsDlg-Meldungshandler

LRESULT DomainDetailsDlg::OnWizardNext(void)
{
	this->UpdateData(TRUE);

	CInstApp* app = ((CInstApp*)AfxGetApp());
	if (app->mf_GwiaSmtpHome != "")
	{
		CString tmp = app->mf_GwiaDHome;
		tmp += "\\MF";

		if (tmp.CompareNoCase(app->mf_GwiaSmtpHome) != 0)
		{
			// bleh, workaround for old mf installations
			// where servername\ was prepended to dhome path
			tmp = app->mf_ServerName + "\\" + tmp;
			if (tmp.CompareNoCase(app->mf_GwiaSmtpHome) != 0)
				return CPropertyPage::OnWizardNext();
		}
	}

	return IDD_INSTALL;
}

BOOL DomainDetailsDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

	return CPropertyPage::OnSetActive();
}