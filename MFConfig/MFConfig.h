// MFConfig.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "mf_c.h"


// CMFConfigApp:
// See MFConfig.cpp for the implementation of this class
//

class CMFConfigApp : public CWinApp
{
public:
	CMFConfigApp();
	CMF_C m_CMFC;
	CString MF_ConfigPath;
	CString szVersion;

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
	virtual BOOL CMFConfigApp::DoInitConfig(CString ConfigPath);

	DECLARE_MESSAGE_MAP()
};

extern CMFConfigApp theApp;