#pragma once

#include "resource.h"
#include "mf_c.h"

// CEditValueDlg dialog

class CEditValueDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditValueDlg)

public:
	CEditValueDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditValueDlg();

// Dialog Data
	enum { IDD = IDD_EDITVALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP();
public:
	MF_C_Storage* m_pStorage;
	afx_msg void OnOK();
	BOOL InitDialog(void);
};
