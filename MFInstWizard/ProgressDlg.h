#pragma once
#include "afxcmn.h"


// ProgressDlg-Dialogfeld

class ProgressDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(ProgressDlg)

public:
	ProgressDlg();
	virtual ~ProgressDlg();

// Dialogfelddaten
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnSetActive(void);
	CProgressCtrl m_ProgressCtrl;
};
