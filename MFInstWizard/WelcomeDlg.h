#pragma once


// WelcomeDlg-Dialogfeld

class WelcomeDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(WelcomeDlg)

public:
	WelcomeDlg();
	virtual ~WelcomeDlg();

// Dialogfelddaten
	enum { IDD = IDD_WELCOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()

public:
	BOOL OnSetActive(void);

};
