// MFInstWizard.h : Hauptheaderdatei für die MFInstWizard-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CInstApp:
// Siehe MFInstWizard.cpp für die Implementierung dieser Klasse
//

class CInstApp : public CWinApp
{
public:
	CInstApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()

public:
	CString mf_ServerName;
	BOOL mf_SharedInstallation;
	BOOL mf_GroupwiseVersion6;
	BOOL mf_GwiaResetSmtpHome;
	BOOL mf_InstallLegacyVersion;
	BOOL mf_LoadMailFilter;
	CString mf_AppDir;
	CString mf_GwiaCfgPath;
	CString mf_LicenseKey;
	CString mf_DomainName;
	CString mf_HostName;
	CString mf_GwiaDHome;
	CString mf_GwiaSmtpHome;
};

extern CInstApp theApp;
