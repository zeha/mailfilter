#pragma once


// CFilterlistDlg dialog

class CFilterlistDlg : public CDialog
{
	DECLARE_DYNAMIC(CFilterlistDlg)

public:
	CFilterlistDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFilterlistDlg();

// Dialog Data
	enum { IDD = IDD_FILTERLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void FillIn();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkConfiglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownConfiglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEntryAdd();
	afx_msg void OnEntryDelete();
	afx_msg void OnListExport();
	afx_msg void OnListImport();
	afx_msg void OnListClose();
	afx_msg void OnLvnItemchangedConfiglist(NMHDR *pNMHDR, LRESULT *pResult);
};
