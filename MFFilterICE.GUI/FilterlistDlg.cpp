// FilterlistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "FilterlistDlg.h"
#include "FilterPropertyDlg.h"

#include "filterice.hxx"
#include "filterlistdlg.h"


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedBitmap();
	afx_msg void OnStnDblclickBitmap();
protected:
	virtual BOOL OnInitDialog();
	virtual void OnPaint();

};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::OnPaint()
{
	CDC* dc = this->GetDC();
	dc->SetBkColor(WHITE_BRUSH);
	dc->FloodFill(0,0,WHITE_BRUSH);
	dc->ReleaseOutputDC();
}

BOOL CAboutDlg::OnInitDialog()
{

	this->SetDlgItemText(IDC_VERSION,((CMFConfigApp*)AfxGetApp())->szVersion);

	return FALSE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFilterlistDlg dialog

IMPLEMENT_DYNAMIC(CFilterlistDlg, CDialog)
CFilterlistDlg::CFilterlistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterlistDlg::IDD, pParent)
{
}

CFilterlistDlg::~CFilterlistDlg()
{
}

void CFilterlistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFilterlistDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_CONFIGLIST, OnNMDblclkConfiglist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CONFIGLIST, OnLvnKeydownConfiglist)
	ON_COMMAND(ID_ENTRY_ADD, OnEntryAdd)
	ON_COMMAND(ID_ENTRY_DELETE, OnEntryDelete)
	ON_COMMAND(ID_LIST_EXPORT, OnListExport)
	ON_COMMAND(ID_LIST_IMPORT, OnListImport)
	ON_COMMAND(ID_LIST_CLOSE, OnListClose)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONFIGLIST, OnLvnItemchangedConfiglist)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_HELP_TEXTFILEDOCUMENTATION, OnHelpTextfiledocumentation)
END_MESSAGE_MAP()

// CFilterlistDlg message handlers

void CFilterlistDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	CListCtrl* list = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);
	list->MoveWindow(0, 0, cx, cy);

	CListCtrl* cv = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);

	int xx; xx = cx - (2*40) - 120 -4;
	xx = xx/2;
	xx = xx - 8;

	cv->SetColumnWidth(0,xx);
	cv->SetColumnWidth(1,xx);
	cv->SetColumnWidth(2,120);

}

void CFilterlistDlg::FillIn()
{
	CListCtrl* cv = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);
	
	cv->DeleteAllItems();

	int ci = 0;

	std::vector<MailFilter_Configuration::Filter>::const_iterator first = MFC_FilterList.begin();
	std::vector<MailFilter_Configuration::Filter>::const_iterator last = MFC_FilterList.end();


	for (; first != last; ++first)
	{
		char szTemp[60];
		cv->InsertItem( LVIF_TEXT, ci, first->name.c_str(), 0, LVIS_SELECTED, 0, 0);
		cv->SetItemData(ci, first-MFC_FilterList.begin());
		cv->SetItemText(ci, 0, first->name.c_str());
		cv->SetItemText(ci, 1, first->expression.c_str());
		cv->SetItemText(ci, 3, first->enabledIncoming ? "Yes" : "No" );
		cv->SetItemText(ci, 4, first->enabledOutgoing ? "Yes" : "No" );

		szTemp[0] = ' '; szTemp[1] = ' ';
		switch (first->matchfield)
		{
		case MAILFILTER_MATCHFIELD_ALWAYS:
			strcpy(szTemp+1,"Always");	break;
		case MAILFILTER_MATCHFIELD_ATTACHMENT:
			strcpy(szTemp+1,"Attachment");	break;
		case MAILFILTER_MATCHFIELD_EMAIL:
			strcpy(szTemp+1,"From:, To:");	break;
		case MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC:
			strcpy(szTemp+1,"From:, To:, CC:");	break;
		case MAILFILTER_MATCHFIELD_SUBJECT:
			strcpy(szTemp+1,"Subject:");	break;
		case MAILFILTER_MATCHFIELD_SIZE:
			strcpy(szTemp+1,"Size");	break;
		case MAILFILTER_MATCHFIELD_EMAIL_FROM:
			strcpy(szTemp+1,"From:");	break;
		case MAILFILTER_MATCHFIELD_EMAIL_TO:
			strcpy(szTemp+1,"To:");	break;
		case MAILFILTER_MATCHFIELD_EMAIL_TOANDCC:
			strcpy(szTemp+1,"To: and CC:");	break;
		case MAILFILTER_MATCHFIELD_BLACKLIST:
			strcpy(szTemp+1,"Realtime IP Blacklist");	break;
		case MAILFILTER_MATCHFIELD_IPUNRESOLVABLE:
			strcpy(szTemp+1,"IP Unresolvable");	break;
		default:
			strcpy(szTemp+1,"Unknown");	break;
		}
		
		if (first->type == MAILFILTER_MATCHTYPE_NOMATCH)
		{
			szTemp[0] = '!';
		}
		cv->SetItemText(ci, 2, szTemp);
				

		ci++;
	}

}

BOOL CFilterlistDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);			// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	CListCtrl* cv = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);
	
	cv->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE);
	cv->EnableToolTips(TRUE);
	cv->EnableTrackingToolTips(TRUE);

	cv->DeleteAllItems();
	cv->InsertColumn(0,"Name");
	cv->InsertColumn(1,"Expression");
	cv->InsertColumn(2,"Type");
	cv->InsertColumn(3,"In");
	cv->InsertColumn(4,"Out");
	
	cv->SetColumnWidth(0,180);
	cv->SetColumnWidth(1,180);
	cv->SetColumnWidth(2,120);
	cv->SetColumnWidth(3,35);
	cv->SetColumnWidth(4,35);

	this->FillIn();

	return FALSE;
}

void CFilterlistDlg::OnNMDblclkConfiglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	CListCtrl* cv = (CListCtrl*)GetDlgItem(IDC_CONFIGLIST);

	int nItem = -1;
	nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
	if (nItem != -1)
	{
		CFilterPropertyDlg filterprp;
		CPropertySheet prp;
		prp.AddPage(&filterprp);
		prp.SetTitle("Filter",PSH_PROPTITLE);
		if (prp.DoModal() == IDOK)
			this->FillIn();
	}

	cv->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);

	*pResult = 0;
}

static void DeleteEntry(int nItem)
{
	std::vector<MailFilter_Configuration::Filter>::iterator first = MFC_FilterList.begin();
	first += nItem;
	MFC_FilterList.erase(first);

}

void CFilterlistDlg::OnLvnKeydownConfiglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	CListCtrl* cv = (CListCtrl*)GetDlgItem(IDC_CONFIGLIST);

	WORD key = pLVKeyDown->wVKey;
	if ( key == VK_DELETE )
	{
		int nItem = -1;
		nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
		if (nItem != -1)
		{
			DeleteEntry(nItem);
			this->FillIn();
		}
		this->OnEntryDelete();
	}

	if ( key == VK_INSERT )
	{
		this->OnEntryAdd();
	}

	*pResult = 0;
}

void CFilterlistDlg::OnEntryAdd()
{
	CFilterPropertyDlg filterprp;
	filterprp.m_NewEntry = true;
	
	CPropertySheet prp;
	prp.AddPage(&filterprp);
	prp.SetTitle("Add Filter",0);

	if (prp.DoModal() == IDOK)
		this->FillIn();
}

void CFilterlistDlg::OnEntryDelete()
{
	CListCtrl* cv = (CListCtrl*)GetDlgItem(IDC_CONFIGLIST);

	int nItem = -1;
	nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
	if (nItem != -1)
	{
		//int data = (int)cv->GetItemData(nItem);
		DeleteEntry(nItem);
		this->FillIn();
	}
}

void CFilterlistDlg::OnListExport()
{
/*	CPropertySheet wizard("Export Wizard",this);
	wizard.SetWizardMode();
	wizard.AddPage(new CWizardExport);
	wizard.m_psh.dwFlags |= PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
	wizard.m_psh.hInstance = AfxGetInstanceHandle();
	wizard.DoModal();
*/
	CFileDialog fileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "MailFilter Binary Filter Files (*.bin)|*.bin|TAB Seperated Text Files (*.tab)|*.tab|");
	if (fileDlg.DoModal() != IDOK)
		return;

	CString szFile;
	szFile = fileDlg.GetPathName();

	int iMode = 0;	//BIN

	int rc = ExecuteWrite(szFile,iMode);

	if (rc == -1)
	{
		AfxMessageBox("The specified file could not be opened. Please select another file.",MB_OK,0);
		return;
	}
	if (rc == -2)
	{
		AfxMessageBox("The file data could not be exported. File in use?",MB_OK,0);
		return;
	}
	if (rc != 0)
		AfxMessageBox("Generic Error - file not accessible?",MB_OK,0);

	this->FillIn();
}

void CFilterlistDlg::OnListImport()
{
/*	CPropertySheet wizard("Import Wizard",this);
	wizard.SetWizardMode();
	wizard.AddPage(new CWizardImport);
	wizard.m_psh.dwFlags |= PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
	wizard.m_psh.hInstance = AfxGetInstanceHandle();
	wizard.DoModal();
*/

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, "MailFilter Binary Filter Files (*.bin)|*.bin|TAB Seperated Text Files (*.tab)|*.tab|");
	if (fileDlg.DoModal() != IDOK)
		return;

	CString szFile;
	szFile = fileDlg.GetPathName();

	int iMode = 0;	//BIN
/*
	if (this->IsDlgButtonChecked(IDC_TYPE_TAB))
		iImportMode = 1;	//TAB
	if (this->IsDlgButtonChecked(IDC_TYPE_CSV))
		iImportMode = 2;	//CSV

*/	
	if (szFile.Right(3).MakeLower() == "bin")
		iMode = 0;	//BIN
	if (szFile.Right(3).MakeLower() == "tab")
		iMode = 1;	//TAB
	if (szFile.Right(3).MakeLower() == "csv")
		iMode = 2;	//CSV

	int rc = ExecuteRead(szFile,iMode);

	if (rc == -1)
	{
		AfxMessageBox("The specified file could not be opened. Please select another file.",MB_OK,0);
		return;
	}
	if (rc == -2)
	{
		AfxMessageBox("The file data could not be imported. File corrupt?",MB_OK,0);
		return;
	}
	if (rc == -3)
	{
		AfxMessageBox("The file is not in the specified format. File corrupt?",MB_OK,0);
		return;
	}
	if (rc == -4)
	{
		AfxMessageBox("The file contains data in a previous format. Please export your filter data from a recent MailFilter NLM version.",MB_OK,0);
		return;
	}

	this->FillIn();
}

void CFilterlistDlg::OnListClose()
{
	this->OnOK();
}

void CFilterlistDlg::OnLvnItemchangedConfiglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CFilterlistDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CFilterlistDlg::OnHelpTextfiledocumentation()
{
	ShellExecute(this->m_hWnd, "open", "docs\\notes\\MFFilterICE.en.html", "", "", SW_SHOW);
}

