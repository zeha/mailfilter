// InstWizardDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "MFInstWizard.h"
#include "InstWizardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg-Dialogfeld f�r Anwendungsbefehl 'Info'

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// InstWizardDlg Dialogfeld



InstWizardDlg::InstWizardDlg(CWnd* pParent /*=NULL*/)
	: CPropertyPage(InstWizardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void InstWizardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(InstWizardDlg, CPropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// InstWizardDlg Meldungshandler

BOOL InstWizardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol f�r dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Gro�es Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	return TRUE;  // Geben Sie TRUE zur�ck, au�er ein Steuerelement soll den Fokus erhalten
}
