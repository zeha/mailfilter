#pragma once


// LicenseDlg-Dialogfeld

class LicenseDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(LicenseDlg)

public:
	LicenseDlg();
	virtual ~LicenseDlg();

// Dialogfelddaten
	enum { IDD = IDD_LICENSEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnReadLicFile();
};
