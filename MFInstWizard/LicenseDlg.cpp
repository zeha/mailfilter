// LicenseDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "LicenseDlg.h"
#include ".\licensedlg.h"


// LicenseDlg-Dialogfeld

IMPLEMENT_DYNAMIC(LicenseDlg, CPropertyPage)
LicenseDlg::LicenseDlg()
	: CPropertyPage(LicenseDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->dwFlags &= ~PSP_HASHELP;
	m_pPSP->pszHeaderTitle = "License Details";
	m_pPSP->pszHeaderSubTitle = "Please enter your MailFilter Server License Key.";
}

LicenseDlg::~LicenseDlg()
{
}

void LicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	CInstApp* app = ((CInstApp*)AfxGetApp());
	DDX_Text(pDX, IDC_LICENSEKEY, app->mf_LicenseKey);
	DDV_MaxChars(pDX, app->mf_LicenseKey, 128);
}


BEGIN_MESSAGE_MAP(LicenseDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_READLICFILE, OnBnReadLicFile)
END_MESSAGE_MAP()


// LicenseDlg-Meldungshandler

void LicenseDlg::OnBnReadLicFile()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, "Text Files (*.txt)|*.txt|");
	if (fileDlg.DoModal() == IDOK)
	{
		CString szFilename;
		szFilename = fileDlg.GetPathName();
		CInstApp* app = ((CInstApp*)AfxGetApp());
		
		{
			FILE* fImp = fopen(szFilename,"rt");
			if (!fImp)
			{
				AfxMessageBox("Error opening specified file. Please make sure you have all required rights to perform this operation.",MB_ICONERROR);
				return;
			}
			
			char szTemp[MAX_PATH];
			char szTemp2[MAX_PATH];
			char* cResult;

			int formatPosition = 0;
			while (!feof(fImp))
			{
				szTemp[0]=0;
				fgets(szTemp, MAX_PATH, fImp);
				szTemp[MAX_PATH-2]=0;
				
				if (formatPosition == 2) {
					cResult = strstr(szTemp,"KEY: ");	
					if (cResult != NULL)
						{ 
						  app->mf_LicenseKey = cResult+5;
						  app->mf_LicenseKey.Remove('\n');
						  app->mf_LicenseKey.Remove('\r');
						  app->mf_LicenseKey.Remove('\0');
						  if (formatPosition==2) formatPosition = 3;
						} else formatPosition=0;		
				}

				if (formatPosition == 1) {
					sprintf(szTemp2,"SERVER: ",app->mf_ServerName);
					cResult = strstr(szTemp,szTemp2);	
					if (cResult != NULL)
						formatPosition = 2;
						else formatPosition=0;		
				}

				if (formatPosition == 0) {
					cResult = strstr(szTemp,"-----------------------MAILFILTER  CLEARINGHOUSE-----------------------");
					if (cResult != NULL)
						formatPosition = 1;
				}
				
				if (formatPosition == 3)
					break;
			}
			
			fclose(fImp);

			if (formatPosition != 3)
			{
				AfxMessageBox("The specified file does not contain a MailFilter License Key for Server " + app->mf_ServerName + ".",MB_ICONEXCLAMATION);
				return;
			}
			else
			{
				this->UpdateData(FALSE);
			}
		}
	}
}
