#pragma once


// CFilterPropertyDlg dialog

class CFilterPropertyDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(CFilterPropertyDlg)

public:
	CFilterPropertyDlg();
	virtual ~CFilterPropertyDlg();

// Dialog Data
	enum { IDD = IDD_FILTEREDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	// determines if this is a new entry to be written
	bool m_NewEntry;
	afx_msg void OnCbnSelchangeMatchfield();
};
