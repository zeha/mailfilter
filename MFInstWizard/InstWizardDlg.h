// InstWizardDlg.h : Headerdatei
//

#pragma once


// InstWizardDlg Dialogfeld
class InstWizardDlg : public CPropertyPage
{
// Konstruktion
public:
	InstWizardDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_MFINSTWIZARD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
