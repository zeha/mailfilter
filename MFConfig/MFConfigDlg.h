// MFConfigDlg.h : header file
//

#pragma once
#include "mf_c.h"

// CMFConfigDlg dialog
class CMFConfigDlg : public CDialog
{
// Construction
public:
	CMFConfigDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	void DoConfigFill();
	void SaveParameter();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemActivateConfiglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRevert();
private:
	MF_C_Storage* m_LastSelectedID;
public:
	afx_msg void OnBnClickedFilters();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedTemplates();
};
