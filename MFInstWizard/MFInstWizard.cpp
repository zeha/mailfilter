// MFInstWizard.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "ServerDlg.h"
#include "FoldersDlg.h"
#include "DomainDetailsDlg.h"
#include "SmtpHomeDlg.h"
#include "WelcomeDlg.h"
#include "ProgressDlg.h"
#include "LicenseDlg.h"
#include "netwareapi.h"

#include <iostream>
#include <fstream>

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
	this->mf_AppDir = "SYS:MAILFLT";
	this->mf_InstallLegacyVersion = FALSE;
	this->mf_GroupwiseVersion6 = TRUE;

	NetwareApi api;
	api.GetPrimaryServerName(&this->mf_ServerName);	
}


// Das einzige CInstApp-Objekt

CInstApp theApp;

static void WriteLog(CString szText)
{
	CTime time = CTime::GetCurrentTime();

	std::ofstream logFile("mfinstall.log",std::ios_base::app);
	logFile << time.Format("%c") << ": " << szText << std::endl;
	logFile.close();
}

static unsigned int MF_CreateDirectory(CString szDirectoryName)
{
	CFileStatus status;
	WriteLog(" CreateDirectory: " + szDirectoryName);
	if (CFile::GetStatus(szDirectoryName,status))
	{
		if (!(status.m_attribute | 0x10))
		{
			WriteLog("   ==> ERROR rc 1: file already exists, but !directory");
			return 1;
		}
		// ok directory already exists, no need to create it
		WriteLog("   ==> rc 0: directory already exists");
		return 0;
	} else {
		// no status --> nothing there, go create.
		if (CreateDirectory(szDirectoryName,NULL) == FALSE)
		{
			WriteLog("   ==> ERROR rc 2: error when creating directory");
			return 2;
		}
		WriteLog("   ==> rc 0: directory created");
		return 0;
	}
}

static unsigned int MF_CopyFile(CProgressCtrl *progressCtrl, CString sourcePath, CString destPath, CString fileName)
{
	unsigned int rc = 0;
	char szTemp[40];

	progressCtrl->SetPos(progressCtrl->GetPos()+1);
	WriteLog(" Copy file " + fileName + " from " + sourcePath + " to " + destPath);
	rc = CopyFile(sourcePath + "\\" + fileName, destPath + "\\" + fileName, FALSE);
	
	CString buf = "  => rc: ";
	itoa(rc,szTemp,10); 	WriteLog(buf + szTemp);
	return rc;
}

static unsigned int MF_CreateNCFFile(CString szNCFFilename, CString szExecLine)
{
	DeleteFile(szNCFFilename);

	WriteLog(" CreateNCFFile: " + szNCFFilename + " - '" + szExecLine + "'");

	FILE* fp = fopen(szNCFFilename,"w");
	if (fp == NULL)
	{
		WriteLog("  => ERROR fopen failed");
		return 1;
	}

	fprintf(fp,"#\n# %s\n# NCF File created by MailFilter Installation Wizard\n",szNCFFilename);
	fprintf(fp,"%s\n",szExecLine);
	fprintf(fp,"#\n#\n\n");

	fclose(fp);

	WriteLog("  => done");

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
	CBitmap bmpWizSpecial;
	bmpWizSpecial.LoadBitmap(IDB_WIZBMP);

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
		CPropertySheet wizard("MailFilter Installation Wizard",NULL, 0, bmpWizSpecial, NULL, bmpWizHeader);
		wizard.SetWizardMode();
		// 0x198128|
		wizard.m_psh.dwFlags |= PSH_WIZARD97|PSH_HEADER|PSH_USEHICON;
		wizard.m_psh.hInstance = AfxGetInstanceHandle();
		wizard.m_psh.hIcon = hIcon;
		

		WelcomeDlg welcomeDlg;
		ServerDlg serverDlg;
		LicenseDlg licenseDlg;
		FoldersDlg foldersDlg;
		DomainDetailsDlg domainDlg;
		SmtpHomeDlg smtpHomeDlg;

		wizard.AddPage(&welcomeDlg);
		wizard.AddPage(&serverDlg);
		wizard.AddPage(&licenseDlg);
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
			progress.SetWizardButtons(PSWIZB_DISABLEDFINISH);

			CProgressCtrl *progressCtrl = &((ProgressDlg*)progress.GetPage(0))->m_ProgressCtrl;
			progressCtrl->SetRange(0,100);
			progressCtrl->SetPos(30);
			CStatic* progressTextCtrl = (CStatic*)((ProgressDlg*)progress.GetPage(0))->GetDlgItem(IDC_PROGRESSTEXT);
			progress.UpdateWindow();

			// do what we want


			NetwareApi api;
			api.SelectServerByName(this->mf_ServerName);
		
			WriteLog("***************************************************************");
			WriteLog("New Installation to server " + this->mf_ServerName + ", AppDir: " + this->mf_AppDir);
			WriteLog("Gwia.Cfg: " + this->mf_GwiaCfgPath);

			unsigned int server_majorVersion;
			unsigned int server_minorVersion;
			unsigned int server_revision;
			if (!api.GetServerVersion(server_majorVersion,server_minorVersion,server_revision))
			{
				WriteLog("Unable to get server version!");
				szError = "The NetWare Server Version could not be determined. Please check if you are still connected to the target server.";
				bErrors = true;
			} else {
				CString verbuf;
				verbuf.Format(" Raw Server Version Data: %d.%d.%d",server_majorVersion,server_minorVersion,server_revision);
				WriteLog(verbuf);

				this->mf_InstallLegacyVersion = false;
				if (server_majorVersion < 5)
				{
					WriteLog(" Server reported Version < 5... too old for us.");
					szError = "The NetWare Server reported Product Version 4.x or earlier. MailFilter requires at least NetWare 5.0.";
					bErrors = true;
				}
				else
				{
					if ((server_minorVersion == 0) && (server_revision < 10))
					{	WriteLog(" Server 5.0");
						this->mf_InstallLegacyVersion = true;
					}
					if ((server_minorVersion == 0) && (server_revision == 10))
					{	WriteLog(" Server 5.1");
						this->mf_InstallLegacyVersion = true;
					}
					if ((server_minorVersion == 60) && (server_revision < 4))
					{	WriteLog(" Server 6.0 SP3 or earlier");
						this->mf_InstallLegacyVersion = true;
					}
					if ((server_minorVersion == 70) && (server_revision < 1))
					{	WriteLog(" Server 6.5 SP0");
						this->mf_InstallLegacyVersion = true;
					}
					if ((server_minorVersion == 70) && (server_revision > 0))
					{	WriteLog(" Server 6.5 SP1 or newer");
						this->mf_InstallLegacyVersion = false;
					}
					if (server_minorVersion >= 70)
					{	WriteLog(" Server Version newer then 6.5");
						this->mf_InstallLegacyVersion = false;
					}
				}
			}

			if (!bErrors)
			{
				if (this->mf_InstallLegacyVersion)
					WriteLog("Installing LEGACY Version");
				else
					WriteLog("Installing LIBC Version");
				WriteLog("License Key: " + this->mf_LicenseKey);
			}

			CString szAppConfigDest = "";
			CString szAppBinaryDest = "";
			CString szAppBaseDest = "";

			CString szServerAppBaseDest = "";
			CString szServerAppConfigDest = "";
			CString szServerAppBinaryDest = "";

			if (!bErrors)
			{
				if (this->mf_SharedInstallation == TRUE)
				{
					szAppBaseDest = "\\\\" + this->mf_ServerName;
					szAppBaseDest += "\\" + this->mf_AppDir;
					szAppBaseDest.Replace(":","\\");
					if (szAppBaseDest.Right(1) == "\\")
						szAppBaseDest.Delete(szAppBaseDest.GetLength());

					szAppConfigDest = szAppBaseDest + "\\ETC";
					szAppBinaryDest = szAppBaseDest + "\\BIN";

					szServerAppBaseDest = this->mf_AppDir;
					if (szServerAppBaseDest.Right(1) == "\\")
						szServerAppBaseDest.Delete(szServerAppBaseDest.GetLength());
					szServerAppConfigDest = szServerAppBaseDest + "\\ETC";
					szServerAppBinaryDest = szServerAppBaseDest + "\\BIN";

				}
				else
				{
					szAppBaseDest = "\\\\" + this->mf_ServerName + "\\SYS\\";
					szAppBinaryDest = szAppBaseDest + "SYSTEM";
					szAppConfigDest = szAppBaseDest + "ETC\\MAILFLT";
					szAppBaseDest += "SYSTEM";

					szServerAppBaseDest = "SYS:";
					szServerAppBinaryDest = szServerAppBaseDest + "\\SYSTEM";
					szServerAppConfigDest = szServerAppBaseDest + "\\ETC\\MAILFLT";
					szServerAppBaseDest += "SYSTEM";
				}

				WriteLog("Local Directories: " + szAppBaseDest + ", " + szAppConfigDest + ", " + szAppBinaryDest);
				WriteLog("Server Directories: " + szServerAppBaseDest + ", " + szServerAppConfigDest + ", " + szServerAppBinaryDest);

			}


			if (!bErrors)
			{
				// 1: Create Directories
				WriteLog("1 - Create Directories.");
				int rc = 0;
				rc = MF_CreateDirectory(szAppBaseDest);
				if (!rc) 
					MF_CreateDirectory(szAppBinaryDest);
				if (!rc) 
					MF_CreateDirectory(szAppConfigDest);

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
				WriteLog("2 - Copy files.");
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
					if (MF_CopyFile(progressCtrl, sourceEtc, szAppConfigDest, "FILTERS.BIN") == FALSE)		bErrors = true;
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
				// 3: Create ncf files
				WriteLog("3 - Create NCF files.");
				progressCtrl->SetPos(progressCtrl->GetPos()+1);
				progressTextCtrl->SetWindowText("Creating NCF files");
				{
					// mfstart.ncf
					CString line = "LOAD ";
					if (!((server_majorVersion == 5) && (server_minorVersion == 0)))
						line += "PROTECTED ";
					line += szServerAppBinaryDest;
					line += "\\";

					if (!this->mf_InstallLegacyVersion)
						line += "MAILFLT";
					else
						line += "MFLT50";

					line += ".NLM -t server ";
					line += szServerAppConfigDest;

					MF_CreateNCFFile(szAppBaseDest + "\\MFSTART.NCF", line);

					// mfinst.ncf
					line = "LOAD ";
					if (!((server_majorVersion == 5) && (server_minorVersion == 0)))
						line += "PROTECTED ";
					line += szServerAppBinaryDest;
					line += "\\";

					if (!this->mf_InstallLegacyVersion)
						line += "MAILFLT";
					else
						line += "MFLT50";

					line += ".NLM -t install ";
					line += szServerAppConfigDest;

					MF_CreateNCFFile(szAppBaseDest + "\\MFINST.NCF", line);
				}
			}

			if (!bErrors)
			{
				// 4: Create Configuration
				WriteLog("4 - Create Install Configuration.");
				progressCtrl->SetPos(progressCtrl->GetPos()+1);
				progressTextCtrl->SetWindowText("Creating configuration files");
				{
					WriteLog("Writing install.cfg as " + szAppConfigDest + "\\INSTALL.CFG");
					DeleteFile(szAppConfigDest + "\\INSTALL.CFG");
					std::ofstream installCfg(szAppConfigDest + "\\INSTALL.CFG");
					if (installCfg.is_open())
					{
						installCfg << "# created by MFInstallWizard" << std::endl;
						installCfg << "/domain=" << this->mf_DomainName << std::endl;
							WriteLog("  /domain="+this->mf_DomainName+"\n");
						installCfg << "/hostname=" << this->mf_HostName << std::endl;
							WriteLog("  /hostname="+this->mf_HostName+"\n");
						installCfg << "/config-directory=" << szAppConfigDest << std::endl;
							WriteLog("  /config-directory="+szAppConfigDest+"\n");
						installCfg << "/gwia-version=" << (this->mf_GroupwiseVersion6 ? 600 : 550) << std::endl;
							WriteLog(this->mf_GroupwiseVersion6 ? "  /gwia-version=600\n" : "  /gwia-version=550\n");
						installCfg << "/home-gwia=" << this->mf_GwiaDHome << std::endl;
							WriteLog("  /home-gwia="+this->mf_GwiaDHome+"\n");
						installCfg << "/home-mailfilter=" << this->mf_GwiaSmtpHome << std::endl;
							WriteLog("  /home-mailfilter="+this->mf_GwiaSmtpHome+"\n");
						installCfg << "/licensekey=" << this->mf_LicenseKey << std::endl;
							WriteLog("  /licensekey="+this->mf_LicenseKey+"\n");
						installCfg << "/config-importfilterfile=" << szAppConfigDest + "\\FILTERS.BIN" << std::endl;
							WriteLog("  /config-importfilterfile="+ szAppConfigDest + "\\FILTERS.BIN" +"\n");
						installCfg << "/config-deleteinstallfile=yes" << std::endl;
							WriteLog("  /config-deleteinstallfile=yes\n");
						installCfg.close();

						progressCtrl->SetPos(progressCtrl->GetPos()+1);
						progressTextCtrl->SetWindowText("Contacting Server to create Configuration file...");
						WriteLog("Executing NCF: " + szServerAppBaseDest + "\\MFINST.NCF");
						if (!api.ExecuteNCF(szServerAppBaseDest + "\\MFINST.NCF"))
						{
							bErrors = true;
							szError = "MFINST.NCF could not be executed on the Server. Please check the Server Console for errors!";
						}

					} else {
						WriteLog("  => ofstream() failed");
						bErrors = true;
						szError = "Could not write initial configuration file.";
					}
				}
			}

			if (!bErrors)
			{
				// 5: patch autoexec.ncf
				WriteLog("5 - patch autoexec.ncf.");
			}

			if (!bErrors)
			{
				// 6: patch gwia.cfg
				WriteLog("6 - patch gwia.ncf.");
			}

			// Done.

			if (!bErrors)
			{
				WriteLog("----- -------------------------------------------------");
				WriteLog("----- Installation Finished successfully.");
				WriteLog("----- -------------------------------------------------");

				// present finish dialog
				int nLower; int nUpper;	progressCtrl->GetRange(nLower,nUpper);
				progressCtrl->SetPos(nUpper);
				progressTextCtrl->SetWindowText("Finished.");
				progress.UpdateWindow();
				progress.SetWizardButtons(PSWIZB_FINISH);
				progress.RunModalLoop(MLF_SHOWONIDLE);

			} else {
				WriteLog("----- -------------------------------------------------");
				WriteLog("----- Installation aborted:");
				WriteLog("----- " + szError);
				WriteLog("----- -------------------------------------------------");

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

