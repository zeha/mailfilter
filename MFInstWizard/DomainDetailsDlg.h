#pragma once


// DomainDetailsDlg-Dialogfeld

class DomainDetailsDlg : public CPropertyPage
{
	DECLARE_DYNAMIC(DomainDetailsDlg)

public:
	DomainDetailsDlg();
	virtual ~DomainDetailsDlg();

// Dialogfelddaten
	enum { IDD = IDD_DOMAINDETAILS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnWizardNext(void);
	BOOL OnSetActive(void);
};
