#pragma once


// CWizardImport dialog

class CWizardImport : public CPropertyPage
{
	DECLARE_DYNAMIC(CWizardImport)

public:
	CWizardImport();
	virtual ~CWizardImport();

// Dialog Data
	enum { IDD = IDD_WIZ_IMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);
	virtual BOOL OnSetActive(void);
	virtual BOOL OnWizardFinish(void);

	DECLARE_MESSAGE_MAP()
};
