#pragma once


// ServerDlg-Dialogfeld

class ServerDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(ServerDlg)

public:
	ServerDlg();
	virtual ~ServerDlg();

// Dialogfelddaten
	enum { IDD = IDD_SERVERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnWizardNext(void);
	BOOL OnSetActive(void);
};
