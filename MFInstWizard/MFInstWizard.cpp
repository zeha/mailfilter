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

	NetwareApi api;
	api.GetPrimaryServerName(&this->mf_ServerName);	
}


// Das einzige CInstApp-Objekt

CInstApp theApp;


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

UIWizard:
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
			progressCtrl->SetPos(1);
			CStatic* progressTextCtrl = (CStatic*)((ProgressDlg*)progress.GetPage(0))->GetDlgItem(IDC_PROGRESSTEXT);


			// do what we want
			CString szAppConfigDest = "";
			CString szAppBinaryDest = "";

			if (this->mf_SharedInstallation == TRUE)
			{
				szAppConfigDest = "\\\\" + this->mf_ServerName;
				szAppConfigDest += "\\" + this->mf_AppDir.Replace(":","\\");
				if (szAppConfigDest.Right(1) != "\\")
					szAppConfigDest += "\\";
				szAppBinaryDest = szAppConfigDest;
			}
			else
			{
				szAppConfigDest = "\\\\" + this->mf_ServerName + "\\SYS\\";
				szAppBinaryDest = szAppConfigDest + "SYSTEM\\";
				szAppConfigDest += "ETC\\MAILFLT";
			}


			// 1: Create Directory
			{
				{
					CFileStatus status;
					if (CFile::GetStatus(szAppConfigDest,status))
					{
						if (!(status.m_attribute | 0x10))
						{
							szError = "The target directory could not be created: a file already exists at this place. Please delete it before continuing.";
							bErrors = true;
						}

					} else {
						// no status --> nothing there, go create.
						if (CreateDirectory(szAppConfigDest,NULL) == FALSE)
						{
							szError = "The target directory could not be created: an unknown error occoured.";
							bErrors = true;
						}
					}
				}
			}

			if (!bErrors)
			{
				// 2: Copy Files
				progressCtrl->SetPos(2);
				progressTextCtrl->SetWindowText("Copying files");
				{
					CString sourceBase = ".\\NLM\\";
					CopyFile(sourceBase + "MAILFLT.NLM", szAppBinaryDest + "MAILFLT.NLM", FALSE);
					CopyFile(sourceBase + "MFLT50.NLM", szAppBinaryDest + "MFLT50.NLM", FALSE);
					CopyFile(sourceBase + "MFCONFIG.NLM", szAppBinaryDest + "MFCONFIG.NLM", FALSE);
					CopyFile(sourceBase + "MFREST.NLM", szAppBinaryDest + "MFREST.NLM", FALSE);
					CopyFile(sourceBase + "MFNRM.NLM", szAppBinaryDest + "MFNRM.NLM", FALSE);
					CopyFile(sourceBase + "MFSTOP.NCF", szAppBinaryDest + "MFSTOP.NCF", FALSE);
				}
			}

			if (!bErrors)
			{
				// 3: Create mfstart.ncf
			}

			if (!bErrors)
			{
				// 5: Create Configuration
			}

			if (!bErrors)
			{
				// 6: patch autoexec.ncf
			}

			// Done.

			if (!bErrors)
			{
				// present finish dialog

				progress.UpdateWindow();
				progress.SetWizardButtons(PSWIZB_FINISH);
				progress.RunModalLoop(MLF_SHOWONIDLE);

			} else {
				AfxMessageBox("An Error occoured while installing MailFilter on the target server:\n " + szError + "\n\nPlease correct the problem and follow the Installation Wizard steps again.", MB_ICONERROR);
				progress.DestroyWindow();
			}
		}
	}
	if (bErrors)
		goto UIWizard;

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}
