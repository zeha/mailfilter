// EditValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "EditValueDlg.h"


// CEditValueDlg dialog

IMPLEMENT_DYNAMIC(CEditValueDlg, CDialog)
CEditValueDlg::CEditValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditValueDlg::IDD, pParent)
	, m_pStorage(NULL)
{
}

CEditValueDlg::~CEditValueDlg()
{
}

void CEditValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditValueDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()


// CEditValueDlg message handlers

void CEditValueDlg::OnOK()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL CEditValueDlg::InitDialog()
{
	CDialog::OnInitDialog();
	CString tmp;

	if (this->m_pStorage != NULL)
	{
		this->SetDlgItemText(IDC_CONFIGNAME,this->m_pStorage->name);
		switch(this->m_pStorage->type)
		{
		case None:
			break;
		case Integer:
			tmp.Format("%d",this->m_pStorage->data);
			this->SetDlgItemText(IDC_CONFIGVALUE,tmp);
			break;
		case String:
			this->SetDlgItemText(IDC_CONFIGVALUE,(char*)this->m_pStorage->data);
			break;
		case Bool:
			this->SetDlgItemText(IDC_CONFIGVALUE,(bool)this->m_pStorage->data == TRUE ? "Yes" : "No");
			break;
		default:
			break;
		}
	}
	return TRUE;
}
