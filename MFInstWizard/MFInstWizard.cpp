// MFInstWizard.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "ServerDlg.h"
#include "FoldersDlg.h"
#include "DomainDetailsDlg.h"
#include "SmtpHomeDlg.h"
#include "ProgressDlg.h"
#include "netwareapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInstApp

BEGIN_MESSAGE_MAP(CInstApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CInstApp-Erstellung

CInstApp::CInstApp()
{
	this->mf_SharedInstallation = FALSE;
	this->mf_AppDir = "SYS:MAILFILTER";
	this->mf_InstallLegacyVersion = FALSE;

	NetwareApi api;
	api.GetPrimaryServerName(&this->mf_ServerName);	
}


// Das einzige CInstApp-Objekt

CInstApp theApp;


static unsigned int MF_CreateDirectory(const char* szDirectoryName)
{
	CFileStatus status;
	if (CFile::GetStatus(szDirectoryName,status))
	{
		if (!(status.m_attribute | 0x10))
			return 1;
		// ok directory already exists, no need to create it
		return 0;
	} else {
		// no status --> nothing there, go create.
		if (CreateDirectory(szDirectoryName,NULL) == FALSE)
			return 2;
		return 0;
	}
}

static unsigned int MF_CopyFile(CProgressCtrl *progressCtrl, CString sourcePath, CString destPath, CString fileName)
{
	progressCtrl->SetPos(progressCtrl->GetPos()+1);
	return CopyFile(sourcePath + "\\" + fileName, destPath + "\\" + fileName, FALSE);
}

static unsigned int MF_CreateNCFFile(const char* szNCFFilename, const char* szExecLine)
{
	DeleteFile(szNCFFilename);
	FILE* fp = fopen(szNCFFilename,"w");
	if (fp == NULL)
		return 1;

	fprintf(fp,"#\n# %s\n# NCF File created by MailFilter Installation Wizard\n",szNCFFilename);
	fprintf(fp,"%s\n",szExecLine);
	fprintf(fp,"#\n#\n\n");

	fclose(fp);

	return 0;
}

// CInstApp Initialisierung

BOOL CInstApp::InitInstance()
{
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
	InitCommonControls();

	CWinApp::InitInstance();

	// Standardinitialisierung
	// Wenn Sie diese Features nicht verwenden und die Größe
	// der ausführbaren Datei verringern möchten, entfernen Sie
	// die nicht erforderlichen Initialisierungsroutinen.
	// Ändern Sie den Registrierungsschlüssel unter dem Ihre Einstellungen gespeichert sind.
	// TODO: Ändern Sie diese Zeichenfolge entsprechend,
	// z.B. zum Namen Ihrer Firma oder Organisation.
	SetRegistryKey(_T("Hofstaedtler IE GmbH\\MailFilter"));

	CBitmap bmpWizHeader;
	bmpWizHeader.LoadBitmap(IDB_WIZHEADER);

	HICON hIcon;
	hIcon = CWinApp::LoadIcon(IDR_MAINFRAME);

	bool bErrors = false;
	CString szError = "";

	do
	{
	bErrors = false;
	szError = "";
	// "ui" wizard = ask user bleh and blah
	{
		CPropertySheet wizard("MailFilter Installation Wizard",NULL, 0, NULL, NULL, bmpWizHeader);
		wizard.SetWizardMode();
		wizard.m_psh.dwFlags |= PSH_WIZARD97|PSH_HEADER|PSH_USEHICON;
		wizard.m_psh.hInstance = AfxGetInstanceHandle();
		wizard.m_psh.hIcon = hIcon;

		ServerDlg serverDlg;
		FoldersDlg foldersDlg;
		DomainDetailsDlg domainDlg;
		SmtpHomeDlg smtpHomeDlg;

		wizard.AddPage(&serverDlg);
		wizard.AddPage(&foldersDlg);
		wizard.AddPage(&domainDlg);
		wizard.AddPage(&smtpHomeDlg);

		if (wizard.DoModal() == ID_WIZFINISH)
		{
			// act "wizard" = progress dialog

			CPropertySheet progress("MailFilter Installation Wizard",NULL, 0, NULL, NULL, bmpWizHeader);
			progress.SetWizardMode();
			progress.m_psh.dwFlags |= PSH_WIZARD97|PSH_HEADER|PSH_USEHICON;
			progress.m_psh.hInstance = AfxGetInstanceHandle();
			progress.m_psh.hIcon = hIcon;

			ProgressDlg progressDlg;
			progress.AddPage(&progressDlg);

			progress.Create();
			progress.ShowWindow(SW_SHOW);

			CProgressCtrl *progressCtrl = &((ProgressDlg*)progress.GetPage(0))->m_ProgressCtrl;
			progressCtrl->SetRange(0,100);
			progressCtrl->SetPos(30);
			CStatic* progressTextCtrl = (CStatic*)((ProgressDlg*)progress.GetPage(0))->GetDlgItem(IDC_PROGRESSTEXT);
			progress.UpdateWindow();


			// do what we want
			CString szAppConfigDest = "";
			CString szAppBinaryDest = "";
			CString szAppBaseDest = "";

			if (this->mf_SharedInstallation == TRUE)
			{
				szAppBaseDest = "\\\\" + this->mf_ServerName;
				szAppBaseDest += "\\" + this->mf_AppDir;
				szAppBaseDest.Replace(":","\\");
				if (szAppBaseDest.Right(1) == "\\")
					szAppBaseDest.Delete(szAppBaseDest.GetLength());
				szAppConfigDest = szAppBaseDest + "\\ETC";
				szAppBinaryDest = szAppBaseDest + "\\BIN";
			}
			else
			{
				szAppBaseDest = "\\\\" + this->mf_ServerName + "\\SYS\\";
				szAppBinaryDest = szAppBaseDest + "SYSTEM";
				szAppConfigDest = szAppBaseDest + "ETC\\MAILFLT";
				szAppBaseDest += "SYSTEM";
			}


			// 1: Create Directory
			{
				int rc = 0;
				rc = MF_CreateDirectory(szAppBaseDest);
				if (!rc) MF_CreateDirectory(szAppBinaryDest);
				if (!rc) MF_CreateDirectory(szAppConfigDest);
				switch (rc) 
				{
				case 0:
					// kay
					break;
				case 1:
					szError = "The target directory could not be created: a file already exists at this place. Please delete it before continuing.";
					bErrors = true;
					break;
				default:
					szError = "The target directory could not be created: an unknown error occoured.";
					bErrors = true;
					break;
				}
			}

			if (!bErrors)
			{
				// 2: Copy Files
				progressCtrl->SetPos(progressCtrl->GetPos()+1);
				progressTextCtrl->SetWindowText("Copying files");
				{
					CString sourceBase = ".\\SERVER\\";
					CString sourceBin = sourceBase + "BIN\\";
					CString sourceEtc = sourceBase + "ETC\\";

					// base
					szError = "Could not copy NCF file.";
					if (MF_CopyFile(progressCtrl, sourceBase, szAppBaseDest, "MFSTOP.NCF") == FALSE)			bErrors = true;

					// binaries
					if (!bErrors)
						szError = "Could not copy NLM file.";
					if (MF_CopyFile(progressCtrl, sourceBin, szAppBinaryDest, "MAILFLT.NLM") == FALSE)		bErrors = true;
					if (MF_CopyFile(progressCtrl, sourceBin, szAppBinaryDest, "MFLT50.NLM") == FALSE)			bErrors = true;
					DeleteFile(szAppBinaryDest + "\\MFCONFIG.NLM");
					DeleteFile(szAppBinaryDest + "\\MFUPGR.NLM");
					//if (MF_CopyFile(progressCtrl, sourceBin, szAppBinaryDest, "MFCONFIG.NLM") == FALSE)		bErrors = true;
					if (MF_CopyFile(progressCtrl, sourceBin, szAppBinaryDest, "MFREST.NLM") == FALSE)			bErrors = true;
					if (MF_CopyFile(progressCtrl, sourceBin, szAppBinaryDest, "MFNRM.NLM") == FALSE)			bErrors = true;

					// config
					if (!bErrors)
						szError = "Could not copy configuration file.";
					if (MF_CopyFile(progressCtrl, sourceEtc, szAppConfigDest, "MAILCOPY.TPL") == FALSE)		bErrors = true;
					if (MF_CopyFile(progressCtrl, sourceEtc, szAppConfigDest, "REPORT.TPL") == FALSE)			bErrors = true;
					if (MF_CopyFile(progressCtrl, sourceEtc, szAppConfigDest, "RINSIDE.TPL") == FALSE)		bErrors = true;
					if (MF_CopyFile(progressCtrl, sourceEtc, szAppConfigDest, "ROUTRCPT.TPL") == FALSE)		bErrors = true;
					if (MF_CopyFile(progressCtrl, sourceEtc, szAppConfigDest, "ROUTSNDR.TPL") == FALSE)		bErrors = true;

					if (!bErrors)
						szError = "";
				}
			}

			if (!bErrors)
			{
				// 3: Create mfstart.ncf
				progressCtrl->SetPos(progressCtrl->GetPos()+1);
				progressTextCtrl->SetWindowText("Creating NCF files");
				{
					// mfstart.ncf
					CString line = "LOAD PROTECTED ";
					line += szAppBinaryDest;
					line += "\\";

					if (!this->mf_InstallLegacyVersion)
						line += "MAILFLT";
					else
						line += "MFLT50";

					line += ".NLM -t server ";
					line += szAppConfigDest;

					MF_CreateNCFFile(szAppBaseDest + "\\MFSTART.NCF", line);

					// mfinst.ncf
					line = "LOAD PROTECTED ";
					line += szAppBinaryDest;
					line += "\\";

					if (!this->mf_InstallLegacyVersion)
						line += "MAILFLT";
					else
						line += "MFLT50";

					line += ".NLM -t install ";
					line += szAppConfigDest;

					MF_CreateNCFFile(szAppBaseDest + "\\MFINST.NCF", line);
				}
			}

			if (!bErrors)
			{
				// 5: Create Configuration
			}

			if (!bErrors)
			{
				// 6: patch autoexec.ncf
			}

			if (!bErrors)
			{
				// 7: patch gwia.cfg
			}

			// Done.

			if (!bErrors)
			{
				// present finish dialog
				int nLower; int nUpper;	progressCtrl->GetRange(nLower,nUpper);
				progressCtrl->SetPos(nUpper);
				progressTextCtrl->SetWindowText("Finished.");
				progress.UpdateWindow();
				progress.SetWizardButtons(PSWIZB_FINISH);
				progress.RunModalLoop(MLF_SHOWONIDLE);

			} else {
				AfxMessageBox("An Error occoured while installing MailFilter on the target server:\n " + szError + "\n\nPlease correct the problem and follow the Installation Wizard steps again.", MB_ICONERROR);
				progress.DestroyWindow();
			}
		}
	}
	} while (bErrors);

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}
