// MFConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "listctrlex.h"
#include "MFConfig.h"
#include "MFConfigDlg.h"
#include "FilterlistDlg.h"
#include "TemplateEditorDlg.h"

#include "../MailFilter/Config/MFConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	ON_STN_CLICKED(IDC_BITMAP, OnStnClickedBitmap)
	ON_STN_DBLCLK(IDC_BITMAP, OnStnDblclickBitmap)
END_MESSAGE_MAP()


void CAboutDlg::OnStnClickedBitmap()
{
	this->DestroyWindow();
//	OnOK();
}

void CAboutDlg::OnStnDblclickBitmap()
{
	this->DestroyWindow();
//	OnOK();
}

///////////////////////////////////////////////////////////////
// ** MFConfigDlg
//


// CMFConfigDlg dialog



CMFConfigDlg::CMFConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFConfigDlg::IDD, pParent)
	, m_LastSelectedID(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFConfigDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_CONFIGLIST, OnLvnItemActivateConfiglist)
	ON_BN_CLICKED(IDC_REVERT, OnBnClickedRevert)
	ON_BN_CLICKED(IDC_FILTERS, OnBnClickedFilters)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_TEMPLATES, OnBnClickedTemplates)
END_MESSAGE_MAP()


// CMFConfigDlg message handlers

BOOL CMFConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Change Title...
	CString szTitle;
	this->GetWindowText(szTitle);
	this->SetWindowText(szTitle + ": " + theApp.MF_ConfigPath);

	// Output Configuration Items
	CListCtrlEx* pList = (CListCtrlEx*)this->GetDlgItem(IDC_CONFIGLIST);
	pList->FillIn();
	pList->EnableToolTips(TRUE);
	pList->EnableTrackingToolTips(TRUE);

	CListCtrl* pListOrig = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);
	pListOrig->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFConfigDlg::DoConfigFill()
{
	CMF_C cmfc = theApp.m_CMFC;
	MF_C_Storage* mfcs = cmfc.GetFirst();
	CListCtrl* list = (CListCtrl*)this->GetDlgItem(IDC_CONFIGLIST);
	POSITION pos = list->GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int id = ((int)pos) - 1;
		while (mfcs->next != NULL)
		{
			if (mfcs->displayid == id)
			{
				this->m_LastSelectedID = mfcs;
				CString tmp;

				switch(mfcs->type)
				{
				case None:
					break;
				case Integer:
					tmp.Format("%d",(int*)mfcs->data);
					this->SetDlgItemText(IDC_EDITVALUE,tmp);
					break;
				case String:
					this->SetDlgItemText(IDC_EDITVALUE,(char*)mfcs->data);
					break;
				case Bool:
					this->SetDlgItemText(IDC_EDITVALUE,(bool)(mfcs->data) == TRUE ? "Yes" : "No");
					break;
				default:
					break;
				}

				break;
			} else {
				mfcs = mfcs->next;
			}
		}
	}
}

void CMFConfigDlg::SaveParameter()
{
	CListCtrlEx* pList = (CListCtrlEx*)this->GetDlgItem(IDC_CONFIGLIST);
	
	if (this->m_LastSelectedID != NULL)
	{
		CString tmp;
		this->GetDlgItemText(IDC_EDITVALUE,tmp);

		switch(this->m_LastSelectedID->type)
		{
		case None:
			break;
		case Integer:
			this->m_LastSelectedID->data = (void*)atoi(tmp);
			break;
		case String:
			if (tmp.GetBuffer(0) == NULL)
				strcpy((char*)this->m_LastSelectedID->data,"");
			else
				strcpy((char*)this->m_LastSelectedID->data,tmp.GetBuffer(0));
			break;
		case Bool:
			tmp.MakeLower();
			if ((tmp[0] == 'n') || (tmp[0] == 'd') || (tmp[0] == '0'))
				this->m_LastSelectedID->data = FALSE;
			else
				this->m_LastSelectedID->data = (void*)TRUE;
			break;
		default:
			break;
		}

		pList->UpdateValue(this->m_LastSelectedID);
	}

	DoConfigFill();
}

void CMFConfigDlg::OnLvnItemActivateConfiglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	this->SaveParameter();
}

void CMFConfigDlg::OnBnClickedRevert()
{
	DoConfigFill();
}

void CMFConfigDlg::OnBnClickedFilters()
{
	CFilterlistDlg flt;
	flt.DoModal();
}

void CMFConfigDlg::OnBnClickedOk()
{
	this->SaveParameter();
	OnOK();
}

void CMFConfigDlg::OnBnClickedCancel()
{
	if (AfxMessageBox(IDS_EXITWITHOUTSAVE, MB_YESNOCANCEL |MB_ICONASTERISK, 0) == IDYES)
		OnCancel();
}

void CMFConfigDlg::OnBnClickedTemplates()
{
	CTemplateEditorDlg dlg;
	dlg.DoModal();
}
