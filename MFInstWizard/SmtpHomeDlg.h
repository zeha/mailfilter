#pragma once


// SmtpHomeDlg-Dialogfeld

class SmtpHomeDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(SmtpHomeDlg)

public:
	SmtpHomeDlg();
	virtual ~SmtpHomeDlg();

// Dialogfelddaten
	enum { IDD = IDD_SMTPHOME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnWizardFinish(void);
	BOOL OnSetActive(void);
};
