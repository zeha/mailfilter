// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "ListCtrlEx.h"
#include "mf_c.h"
#undef _MAILFILTER_MFCONFIG_STORAGE
#include "../MailFilter/Config/MFConfig.h"


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)
CListCtrlEx::CListCtrlEx()
{
}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
END_MESSAGE_MAP()

// Fills in values from MFConfig
int CListCtrlEx::UpdateValue(MF_C_Storage* mfcs)
{
	if (mfcs == NULL)
		return FALSE;

	CString tmp;

	switch(mfcs->type)
	{
	case None:
		break;
	case Integer:
		tmp.Format("%d",mfcs->data);
		this->SetItemText(mfcs->displayid, 1, tmp);
		break;
	case String:
		this->SetItemText(mfcs->displayid, 1, (char*)mfcs->data);
		break;
	case Bool:
		this->SetItemText(mfcs->displayid, 1, (bool)(mfcs->data) == TRUE ? "Yes" : "No");
		break;
	default:
		break;
	}
	
	this->RedrawItems(mfcs->displayid-2,mfcs->displayid+2);
	return TRUE;
}


// Fills in values from MFConfig
int CListCtrlEx::FillIn(void)
{
	this->InsertColumn(0,"Name");
	this->InsertColumn(1,"Value");
	this->SetColumnWidth(0,200);
	this->SetColumnWidth(1,320);
	
	CMF_C mfc = theApp.m_CMFC;
	MF_C_Storage* mfcs = mfc.GetFirst();

	int ci = 0;
	CString tmp;

	while (mfcs->next != NULL)
	{
		this->InsertItem( LVIF_TEXT, ci, mfcs->name, 0, LVIS_SELECTED, 0, 0);
		this->SetItemData(ci,mfcs->displayid);
		switch(mfcs->type)
		{
		case None:
			break;
		case Integer:
			tmp.Format("%d",mfcs->data);
			this->SetItemText(ci, 1, tmp);
			break;
		case String:
			this->SetItemText(ci, 1, (char*)mfcs->data);
			break;
		case Bool:
			this->SetItemText(ci, 1, (int)mfcs->data == TRUE ? "Yes" : "No");
			break;
		default:
			break;
		}

		ci++;
		mfcs = mfcs->next;
	}
	
	return TRUE;
}

