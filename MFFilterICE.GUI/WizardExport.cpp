// WizardExport.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "WizardExport.h"
#include "filterice.hxx"


// CWizardExport dialog

IMPLEMENT_DYNAMIC(CWizardExport, CPropertyPage)
CWizardExport::CWizardExport()
	: CPropertyPage(CWizardExport::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->pszHeaderTitle = "Export Rule Set";
	m_pPSP->pszHeaderSubTitle = "This wizard allows you to export the complete rule set.";
}

CWizardExport::~CWizardExport()
{
}

void CWizardExport::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWizardExport, CPropertyPage)
END_MESSAGE_MAP()


// CWizardExport message handlers

BOOL CWizardExport::OnSetActive(void)
{
   CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_FINISH);
   this->CheckDlgButton(IDC_TYPE_BIN,TRUE);

   return CPropertyPage::OnSetActive();
}

BOOL CWizardExport::OnWizardFinish(void)
{
	CString szFile;
	int curItem = -1;

	int iExportMode = 0;	//BIN
	
	if (this->IsDlgButtonChecked(IDC_TYPE_TAB))
		iExportMode = 1;	//TAB
	if (this->IsDlgButtonChecked(IDC_TYPE_CSV))
		iExportMode = 2;	//CSV

	this->GetDlgItemText(IDC_EXPORTPATH, szFile);

	if (strlen(szFile) == 0)
	{
		AfxMessageBox("Please enter a filename.",MB_OK,0);
		return FALSE;
	}
	
	int rc = ExecuteWrite(szFile,iExportMode);

	if (rc == 0)
	{
		return TRUE;
	}
	if (rc == -1)
	{
		AfxMessageBox("The specified file could not be opened. Please select another file.",MB_OK,0);
		return FALSE;
	}
	if (rc == -2)
	{
		AfxMessageBox("The file data could not be exported. File in use?",MB_OK,0);
		return FALSE;
	}
	AfxMessageBox("Generic Error - file not accessible?",MB_OK,0);
	return FALSE;
}

BOOL CWizardExport::OnInitDialog(void)
{
	this->CheckDlgButton(IDC_TYPE_MFREX,1);
	return 0;
}
