// FilterlistDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "FilterlistDlg.h"
#include "FilterPropertyDlg.h"
#include "WizardExport.h"
#include "WizardImport.h"
#include "../MailFilter/Config/MFConfig.h"

extern MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];

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
END_MESSAGE_MAP()

// CFilterlistDlg message handlers

void CFilterlistDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	CListCtrl* list = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);
	list->MoveWindow(0, 0, cx, cy);

	CListCtrl* cv = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);

	int xx; xx = cx - (2*40) -4;
	xx = xx/2;
	xx = xx - 8;

	cv->SetColumnWidth(0,xx);
	cv->SetColumnWidth(1,xx);

}

void CFilterlistDlg::FillIn()
{
	CListCtrl* cv = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);
	
	cv->DeleteAllItems();

	int ci = 0;

	for (int i=0; i < MailFilter_MaxFilters; i++)
	{
		if (MFC_Filters[i].expression[0] == 0)
			break;

		cv->InsertItem( LVIF_TEXT, ci, MFC_Filters[i].name, 0, LVIS_SELECTED, 0, 0);
		cv->SetItemData(ci, i);
		cv->SetItemText(ci, 0, (char*)MFC_Filters[i].name);
		cv->SetItemText(ci, 1, (char*)MFC_Filters[i].expression);
		cv->SetItemText(ci, 2, MFC_Filters[i].enabledIncoming ? "Yes" : "No" );
		cv->SetItemText(ci, 3, MFC_Filters[i].enabledOutgoing ? "Yes" : "No" );

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
	cv->InsertColumn(2,"In");
	cv->InsertColumn(3,"Out");
	
	cv->SetColumnWidth(0,180);
	cv->SetColumnWidth(1,180);
	cv->SetColumnWidth(2,35);
	cv->SetColumnWidth(3,35);

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

void CFilterlistDlg::OnLvnKeydownConfiglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	CListCtrl* cv = (CListCtrl*)GetDlgItem(IDC_CONFIGLIST);

	WORD key = pLVKeyDown->wVKey;
	if ( key == VK_DELETE )
	{
/*		int nItem = -1;
		nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
		if (nItem != -1)
		{
			int data = cv->GetItemData(nItem);
			for (int i = data ; i < MailFilter_MaxFilters ; i++)
			{
				MFC_Filters[i] = MFC_Filters[i+1];
				if (MFC_Filters[i].expression[0] == 0)
					break;
			}
			this->FillIn();
		}*/
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
		int data = (int)cv->GetItemData(nItem);
		for (int i = data ; i < MailFilter_MaxFilters ; i++)
		{
			MFC_Filters[i] = MFC_Filters[i+1];
			if (MFC_Filters[i].expression[0] == 0)
				break;
		}
		this->FillIn();
	}
}

void CFilterlistDlg::OnListExport()
{
	CPropertySheet wizard("Export Wizard",this);
	wizard.SetWizardMode();
	wizard.AddPage(new CWizardExport);
	wizard.m_psh.dwFlags |= PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
	wizard.m_psh.hInstance = AfxGetInstanceHandle();
	wizard.DoModal();

	this->FillIn();
}

void CFilterlistDlg::OnListImport()
{
	CPropertySheet wizard("Import Wizard",this);
	wizard.SetWizardMode();
	wizard.AddPage(new CWizardImport);
	wizard.m_psh.dwFlags |= PSH_WIZARD97|PSH_WATERMARK|PSH_HEADER;
	wizard.m_psh.hInstance = AfxGetInstanceHandle();
	wizard.DoModal();

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
