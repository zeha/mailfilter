#pragma once


// CTemplateEditorDlg dialog

class CTemplateEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CTemplateEditorDlg)

public:
	CTemplateEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTemplateEditorDlg();

// Dialog Data
	enum { IDD = IDD_TEXTEDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PasteText(CString str);
	DECLARE_MESSAGE_MAP()
	CString szCurrentFilename;

public:
	afx_msg void OnEnChangeTexteditor();
	afx_msg void OnLbnSelchangeTemplatetype();
	afx_msg void OnBnClickedInsertdynamictext();
	afx_msg void OnDynamictextsOriginalSender();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDynamictextsOriginalRecipient();
	afx_msg void OnDynamictextsOriginalSubject();
	afx_msg void OnDynamictextsDropReason();
	afx_msg void OnDynamictextsMailFilename();
	afx_msg void OnDynamictextsPostmasterEmail();
	afx_msg void OnDynamictextsMailfilterEmail();
	afx_msg void OnDynamictextsServerName();
	afx_msg void OnDynamictextsMailfilterVersion();
};
