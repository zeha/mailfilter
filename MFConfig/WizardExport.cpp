// WizardExport.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "WizardExport.h"
#include "../MailFilter/Config/MFConfig.h"

extern MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];

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

   return CPropertyPage::OnSetActive();
}

BOOL CWizardExport::OnWizardFinish(void)
{
	FILE* fLst;
	CString szFile;
	int curItem = -1;
	int iExportMode = 0;
	
	if (this->IsDlgButtonChecked(IDC_TYPE_CSV))
		iExportMode = 1;	//CSV

	this->GetDlgItemText(IDC_EXPORTPATH, szFile);

	/*if (access(szFile,F_OK) == 0)
	{
		//NWSDisplayErrorText ( ERROR_FILE_EXISTS , SEVERITY_FATAL , NLM_nutHandle , szTemp );
		AfxMessageBox("The file specified already exists. MFConfig will not overwrite existing files. Please select another file.",MB_OK,0);
		return FALSE;
		
	} else {
	*/
	
	if (iExportMode == 0)
		fLst = fopen(szFile,"wb");
	if (iExportMode == 1)
		fLst = fopen(szFile,"wt");

	if (!fLst)
	{
		AfxMessageBox("The file specified could not be written to. Please select another file.",MB_OK,0);
		return FALSE;
	}

	if (iExportMode == 0)
	{
		fprintf(fLst,"MAILFILTER_FILTER_EXCHANGEFILE");
		fputc(0,fLst);
		fprintf(fLst,MAILFILTER_CONFIGURATION_SIGNATURE);
		fputc(0,fLst);
		fputc(0,fLst);
		fputc(0,fLst);
		fputc(0,fLst);
	}
	if (iExportMode == 1)
		fprintf(fLst,"Matchfield,Notify,Type,Action,Enabled,Incoming,Outgoing,Description,Expression\n");


	for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
	{
		if (iExportMode == 0)
		{
			fputc(MFC_Filters[curItem].matchfield,fLst);
			fputc(MFC_Filters[curItem].notify,fLst);
			fputc(MFC_Filters[curItem].type,fLst);
			fputc(MFC_Filters[curItem].action,fLst);
			fputc(MFC_Filters[curItem].enabled,fLst);
			fputc(MFC_Filters[curItem].enabledIncoming,fLst);
			fputc(MFC_Filters[curItem].enabledOutgoing,fLst);
		
			fprintf(fLst,MFC_Filters[curItem].expression);
			fputc(0,fLst);
			fprintf(fLst,MFC_Filters[curItem].name);
			fputc(0,fLst);
		}
		if (iExportMode == 1)
		{
			fprintf(fLst,"0x%02x,0x%02x,0x%02x,0x%02x,%d,%d,%d,\"%s\",\"%s\"\n",
				MFC_Filters[curItem].matchfield,
				MFC_Filters[curItem].notify,
				MFC_Filters[curItem].type,
				MFC_Filters[curItem].action,
				MFC_Filters[curItem].enabled,
				MFC_Filters[curItem].enabledIncoming,
				MFC_Filters[curItem].enabledOutgoing,
				MFC_Filters[curItem].name,
				MFC_Filters[curItem].expression
				);
		}

		if (
			(MFC_Filters[curItem].expression[0]==0)
			&&
			(MFC_Filters[curItem].name[0]==0)
			)
			break;

	}

	fclose(fLst);

	this->MessageBox("Export complete.",NULL,MB_ICONINFORMATION);
	
	return TRUE;
	//}
}

BOOL CWizardExport::OnInitDialog(void)
{
	this->CheckDlgButton(IDC_TYPE_MFREX,1);
	return 0;
}
