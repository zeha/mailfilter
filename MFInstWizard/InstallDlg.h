#pragma once


// InstallDlg-Dialogfeld

class InstallDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(InstallDlg)

public:
	InstallDlg();
	virtual ~InstallDlg();

// Dialogfelddaten
	enum { IDD = IDD_INSTALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()

public:
	BOOL OnSetActive(void);
};
