#pragma once


// CWizardExport dialog

class CWizardExport : public CPropertyPage
{
	DECLARE_DYNAMIC(CWizardExport)

public:
	CWizardExport();
	virtual ~CWizardExport();

// Dialog Data
	enum { IDD = IDD_WIZ_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);
	virtual BOOL OnSetActive(void);
	virtual BOOL OnWizardFinish(void);

	DECLARE_MESSAGE_MAP()
};
