// FoldersDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "FoldersDlg.h"
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include ".\foldersdlg.h"

// FoldersDlg-Dialogfeld

IMPLEMENT_DYNAMIC(FoldersDlg, CPropertyPage)
FoldersDlg::FoldersDlg()
	: CPropertyPage(FoldersDlg::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->dwFlags &= ~PSP_HASHELP;
	m_pPSP->pszHeaderTitle = "Choose Install Location";
	m_pPSP->pszHeaderSubTitle = "Choose the folder in which to install MailFilter Server.";
}

FoldersDlg::~FoldersDlg()
{
}

void FoldersDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	CInstApp* app = ((CInstApp*)AfxGetApp());
	DDX_Text(pDX, IDC_APPDIR, app->mf_AppDir);
	DDX_Text(pDX, IDC_GWIACFGPATH, app->mf_GwiaCfgPath);
	DDX_Radio(pDX, IDC_SHAREDINSTALL, app->mf_SharedInstallation);
}


BEGIN_MESSAGE_MAP(FoldersDlg, CPropertyPage)
	ON_BN_CLICKED(IDC_SELECTGWIACFG, OnBnSelectGwiaCfg)
END_MESSAGE_MAP()


// FoldersDlg-Meldungshandler


LRESULT FoldersDlg::OnWizardNext(void)
{
	this->UpdateData(TRUE);
	CInstApp* app = ((CInstApp*)AfxGetApp());

	CFileStatus status;
	if (CFile::GetStatus(app->mf_GwiaCfgPath, status) == TRUE)
	{
		app->mf_GroupwiseVersion6 = FALSE;
		app->mf_GwiaResetSmtpHome = TRUE;
		app->mf_GwiaSmtpHome = "";
		app->mf_GwiaDHome = "";

		static const std::string::size_type npos = -1;
		size_t i;

		std::string line;
		std::string value;
		std::ifstream cfgfile(app->mf_GwiaCfgPath);
		while (cfgfile >> line)
		{
			if (line[0] == '/')
			{
				value = "";
				i = line.find("=");
				if (i == npos) 
					i = line.find("-");
				if (i != npos)
					value = line.substr(++i);

				if (value[0] == '"')
					value = value.substr(1);
				if (value[value.length()-1] == '"')
					value = value.substr(0,value.length()-1);

				// /PARAMETER
				if (stricmp(line.substr(0,3).c_str(),"/hn") == 0)
				{
					// /hn
					// contains the hostname GWIA reports to the outside world
					if (value != "")
					{
						app->mf_HostName = value.c_str();
						i = value.find(".");
						if (i != npos)
							app->mf_DomainName = value.substr(++i).c_str();
					}
				}
				if (stricmp(line.substr(0,7).c_str(),"/dsnage") == 0)
				{
					// /dsnage
					// I believe this is only supported by GWIA 6.0+
					app->mf_GroupwiseVersion6 = TRUE;
				}
				if (stricmp(line.substr(0,6).c_str(),"/dhome") == 0)
				{
					// /dhome
					// Path to the GWIA Working Directory
					if (value != "")
						app->mf_GwiaDHome = value.c_str();
				}
				if (stricmp(line.substr(0,9).c_str(),"/smtphome") == 0)
				{
					// /smtphome
					// Path to the GWIA "SMTP Home" Directory
					// Normally, this is empty - else MF probably was already there, 
					// or some other gwia addon ...
					if (value != "")
						app->mf_GwiaSmtpHome = value.c_str();
				}
			}
		}
		cfgfile.close();

		if (app->mf_GwiaDHome == "")
		{
			MessageBox("The specified GWIA.CFG could not be read successfully - essential information (GWIA Switch /DHOME) is missing. Please select a valid GWIA.CFG before continuing.",0,MB_ICONEXCLAMATION);
			((CEdit*)this->GetDlgItem(IDC_GWIACFGPATH))->SetFocus();
			return -1;
		}

	} else {
		MessageBox("Could not find the specified GWIA.CFG. Usually it is located on SYS:\\SYSTEM\\GWIA.CFG.",0,MB_ICONEXCLAMATION);
		((CEdit*)this->GetDlgItem(IDC_GWIACFGPATH))->SetFocus();
		return -1;
	}
	
	return CPropertyPage::OnWizardNext();
}

BOOL FoldersDlg::OnSetActive(void)
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
	return CPropertyPage::OnSetActive();
}

void FoldersDlg::OnBnSelectGwiaCfg()
{
	this->UpdateData(TRUE);
	CInstApp* app = ((CInstApp*)AfxGetApp());

	CFileDialog fileDlg(TRUE, NULL, app->mf_GwiaCfgPath, OFN_HIDEREADONLY, "Configuration Files (*.CFG)|*.CFG|");
	if (fileDlg.DoModal() == IDOK)
	{
		app->mf_GwiaCfgPath = fileDlg.GetPathName();
		this->UpdateData(FALSE);
	}	
}
