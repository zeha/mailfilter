#pragma once


// FoldersDlg-Dialogfeld

class FoldersDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(FoldersDlg)

public:
	FoldersDlg();
	virtual ~FoldersDlg();

// Dialogfelddaten
	enum { IDD = IDD_FOLDERSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	LRESULT OnWizardNext(void);
	BOOL OnSetActive(void);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnSelectGwiaCfg();
};
