// WizardImport.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "WizardImport.h"
#include "filterice.hxx"

// CWizardImport dialog

IMPLEMENT_DYNAMIC(CWizardImport, CPropertyPage)
CWizardImport::CWizardImport()
	: CPropertyPage(CWizardImport::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->pszHeaderTitle = "Import Rule Set";
	m_pPSP->pszHeaderSubTitle = "This wizard allows you to import a complete rule set.";
}

CWizardImport::~CWizardImport()
{
}

void CWizardImport::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWizardImport, CPropertyPage)
END_MESSAGE_MAP()


// CWizardImport message handlers
BOOL CWizardImport::OnSetActive(void)
{
   CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_FINISH);
   this->CheckDlgButton(IDC_TYPE_BIN,TRUE);

   return CPropertyPage::OnSetActive();
}

int extractString(char* inStr, char* szOut)
{
	int cpos = 0;
	int strStart = -1;
	int tmpCharsRead = 0;
	int len = (int)strlen(inStr);

	while ( (inStr[cpos++] != '\"') && (inStr[cpos] != 0) ) {}; // bis zum ersten " lesen
	if (inStr[cpos] == 0)
		return 0;

	strStart = cpos;
	for (cpos = strStart ; cpos < len ; cpos++)
	{
		szOut[cpos-strStart] = inStr[cpos];
		if (inStr[cpos] == '\0')
		{	szOut[0] = 0;	/* String ungültig..., kein 2tes " gefunden */
			return 0;
		}
		
		if (inStr[cpos] == '\"')
		{
			if (
					( (inStr[cpos+1] == '\r') )
					||
					( (inStr[cpos+1] == '\n') )
					||
					( (inStr[cpos+1] == 0) )
					||
					( (inStr[cpos+1] == ',') ) 
					|| 
					( (inStr[cpos+1] == ' ') && (inStr[cpos+2] == ',') )
				)
			{
				/* TODO ergänzen string weiterlesen/test */
				szOut[cpos-strStart] = 0;
				return cpos+1;
				break;
			}
//			if (inStr[cpos+1] == 
		}

	}
	return 0;
}

BOOL CWizardImport::OnWizardFinish(void)
{
	CString szFile;

	int iImportMode = 0;	//BIN

	if (this->IsDlgButtonChecked(IDC_TYPE_TAB))
		iImportMode = 1;	//TAB
	if (this->IsDlgButtonChecked(IDC_TYPE_CSV))
		iImportMode = 2;	//CSV

	this->GetDlgItemText(IDC_IMPORTPATH, szFile);
	
	if (strlen(szFile) == 0)
	{
		AfxMessageBox("Please enter a filename.",MB_OK,0);
		return FALSE;
	}

	int rc = ExecuteRead(szFile,iImportMode);

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
		AfxMessageBox("The file data could not be imported. File corrupt?",MB_OK,0);
		return FALSE;
	}
	if (rc == -3)
	{
		AfxMessageBox("The file is not in the specified format. File corrupt?",MB_OK,0);
		return FALSE;
	}
	if (rc == -4)
	{
		AfxMessageBox("The file contains data in a previous format. Please export your filter data from a recent MailFilter NLM version.",MB_OK,0);
		return FALSE;
	}
	AfxMessageBox("Generic Error - file corrupt?",MB_OK,0);
	return FALSE;
}

BOOL CWizardImport::OnInitDialog(void)
{
	this->CheckDlgButton(IDC_TYPE_MFREX,1);
	return 0;
}
