// FilterPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "FilterPropertyDlg.h"
#include "../MailFilter/Config/MFConfig.h"

extern MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];

// CFilterPropertyDlg dialog

IMPLEMENT_DYNAMIC(CFilterPropertyDlg, CPropertyPage)
CFilterPropertyDlg::CFilterPropertyDlg()
	: CPropertyPage(CFilterPropertyDlg::IDD)
	, m_NewEntry(false)
{
}

CFilterPropertyDlg::~CFilterPropertyDlg()
{
}

void CFilterPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFilterPropertyDlg, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_MATCHFIELD, OnCbnSelchangeMatchfield)
END_MESSAGE_MAP()


// CFilterPropertyDlg message handlers
BOOL CFilterPropertyDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CComboBox* box;
	box = (CComboBox*)this->GetDlgItem(IDC_MATCHACTION);
	box->AddString("Pass");
	box->AddString("Drop Mail");
	box->AddString("Schedule: On");
	box->AddString("Schedule: Off");
	box->AddString("Copy");

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHFIELD);
	box->AddString("Attachment Name");
	box->AddString("E-Mail Address: both");
	box->AddString("E-Mail Address: From");
	box->AddString("E-Mail Address: To");
	box->AddString("Relay blacklisted");
	box->AddString("Relay unresolvable");
	box->AddString("Size");
	box->AddString("Subject Line");

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHTYPE);
	box->AddString("Match");
	box->AddString("No Match");

	if (m_NewEntry)
	{
		CheckDlgButton(IDC_NOTIFY_IN_SENDER		, MFC_Notification_EnableInternalSndr );
		CheckDlgButton(IDC_NOTIFY_OUT_SENDER	, MFC_Notification_EnableExternalSndr );
		CheckDlgButton(IDC_NOTIFY_IN_RECIPIENT	, MFC_Notification_EnableInternalRcpt );
		CheckDlgButton(IDC_NOTIFY_OUT_RECIPIENT	, MFC_Notification_EnableExternalRcpt );

		return TRUE;
	}

	CListCtrl* cv = (CListCtrl*)this->GetParent()->GetParent()->GetDlgItem(IDC_CONFIGLIST);

	int nItem = -1;
	nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
	ASSERT(nItem != -1);
	int nID = (int)cv->GetItemData(nItem);
	SetDlgItemText(IDC_FILTERNAME,				MFC_Filters[nID].name);
	SetDlgItemText(IDC_FILTEREXPRESSION,		MFC_Filters[nID].expression);
	
	box = (CComboBox*)this->GetDlgItem(IDC_MATCHACTION);
	switch (MFC_Filters[nID].action)
	{
	case MAILFILTER_MATCHACTION_PASS:
        box->SetCurSel(0);		break;
	case MAILFILTER_MATCHACTION_DROP_MAIL:
        box->SetCurSel(1);		break;
	case MAILFILTER_MATCHACTION_SCHEDULE:
        box->SetCurSel(2);		break;
	case MAILFILTER_MATCHACTION_NOSCHEDULE:
        box->SetCurSel(3);		break;
	case MAILFILTER_MATCHACTION_COPY:
        box->SetCurSel(4);		break;
	}

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHFIELD);
	switch (MFC_Filters[nID].matchfield)
	{
	case MAILFILTER_MATCHFIELD_ATTACHMENT:
        box->SetCurSel(0);		break;
	case MAILFILTER_MATCHFIELD_EMAIL:
        box->SetCurSel(1);		break;
	case MAILFILTER_MATCHFIELD_EMAIL_FROM:
        box->SetCurSel(2);		break;
	case MAILFILTER_MATCHFIELD_EMAIL_TO:
        box->SetCurSel(3);		break;
	case MAILFILTER_MATCHFIELD_BLACKLIST:
        box->SetCurSel(4);		break;
	case MAILFILTER_MATCHFIELD_IPUNRESOLVABLE:
        box->SetCurSel(5);		break;
	case MAILFILTER_MATCHFIELD_SIZE:
        box->SetCurSel(6);		break;
	case MAILFILTER_MATCHFIELD_SUBJECT:
        box->SetCurSel(7);		break;
	}

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHTYPE);
	switch (MFC_Filters[nID].type)
	{
	case MAILFILTER_MATCHTYPE_MATCH:
		box->SetCurSel(0);		break;
	case MAILFILTER_MATCHTYPE_NOMATCH:
		box->SetCurSel(1);		break;
	}

	CheckDlgButton(IDC_ENABLED_IN,MFC_Filters[nID].enabledIncoming);
	CheckDlgButton(IDC_ENABLED_OUT,MFC_Filters[nID].enabledOutgoing);

	CheckDlgButton(IDC_NOTIFY_IN_ADMIN		, chkFlag( MFC_Filters[nID].notify , MAILFILTER_NOTIFICATION_ADMIN_INCOMING ) );
	CheckDlgButton(IDC_NOTIFY_OUT_ADMIN		, chkFlag( MFC_Filters[nID].notify , MAILFILTER_NOTIFICATION_ADMIN_OUTGOING ) );
	CheckDlgButton(IDC_NOTIFY_IN_SENDER		, chkFlag( MFC_Filters[nID].notify , MAILFILTER_NOTIFICATION_SENDER_INCOMING ) );
	CheckDlgButton(IDC_NOTIFY_OUT_SENDER	, chkFlag( MFC_Filters[nID].notify , MAILFILTER_NOTIFICATION_SENDER_OUTGOING ) );
	CheckDlgButton(IDC_NOTIFY_IN_RECIPIENT	, chkFlag( MFC_Filters[nID].notify , MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING ) );
	CheckDlgButton(IDC_NOTIFY_OUT_RECIPIENT	, chkFlag( MFC_Filters[nID].notify , MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING ) );

	return FALSE;
}

void CFilterPropertyDlg::OnOK() 
{
	CComboBox* box;
	int nID = 0;

	if (!m_NewEntry)
	{
		CListCtrl* cv = (CListCtrl*)this->GetParent()->GetParent()->GetDlgItem(IDC_CONFIGLIST);

		int nItem = -1;
		nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);
		nID = (int)cv->GetItemData(nItem);
	} else {
		for (nID = 0; nID < MailFilter_MaxFilters; nID++)
			if (MFC_Filters[nID].expression[0] == 0)
				break;
	}

	CString szTmp;
	this->GetDlgItemText(IDC_FILTEREXPRESSION,szTmp);
	if (szTmp == "")
	{
		AfxMessageBox("The expression must be filled in!",MB_OK,0);
		return;
	}

	this->GetDlgItemText(IDC_FILTERNAME,			MFC_Filters[nID].name,			60	);
	this->GetDlgItemText(IDC_FILTEREXPRESSION,		MFC_Filters[nID].expression,	250	);
	
	box = (CComboBox*)this->GetDlgItem(IDC_MATCHACTION);
	switch (box->GetCurSel())
	{
	case 0:
		MFC_Filters[nID].action = MAILFILTER_MATCHACTION_PASS;			break;
	case 1:
		MFC_Filters[nID].action = MAILFILTER_MATCHACTION_DROP_MAIL;		break;
	case 2:
		MFC_Filters[nID].action = MAILFILTER_MATCHACTION_SCHEDULE;		break;
	case 3:
		MFC_Filters[nID].action = MAILFILTER_MATCHACTION_NOSCHEDULE;	break;
	case 4:
		MFC_Filters[nID].action = MAILFILTER_MATCHACTION_COPY;			break;
	}

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHFIELD);
	switch (box->GetCurSel())
	{
	case 0:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_ATTACHMENT;		break;
	case 1:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_EMAIL;			break;
	case 2:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_EMAIL_FROM;		break;
	case 3:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_EMAIL_TO;		break;
	case 4:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_BLACKLIST;		break;
	case 5:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_IPUNRESOLVABLE;	break;
	case 6:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_SIZE;			break;
	case 7:
		MFC_Filters[nID].matchfield = MAILFILTER_MATCHFIELD_SUBJECT;		break;
	}


	box = (CComboBox*)this->GetDlgItem(IDC_MATCHTYPE);
	switch (box->GetCurSel())
	{
	case 0:
		MFC_Filters[nID].type = MAILFILTER_MATCHTYPE_MATCH;					break;
	case 1:
		MFC_Filters[nID].type = MAILFILTER_MATCHTYPE_NOMATCH;				break;
	}

	MFC_Filters[nID].enabledIncoming = this->IsDlgButtonChecked(IDC_ENABLED_IN);
	MFC_Filters[nID].enabledOutgoing = this->IsDlgButtonChecked(IDC_ENABLED_OUT);

	MFC_Filters[nID].notify = MAILFILTER_NOTIFICATION_NONE;

	if (this->IsDlgButtonChecked(IDC_NOTIFY_IN_ADMIN))
		MFC_Filters[nID].notify = MFC_Filters[nID].notify | MAILFILTER_NOTIFICATION_ADMIN_INCOMING ;
	if (this->IsDlgButtonChecked(IDC_NOTIFY_OUT_ADMIN))
		MFC_Filters[nID].notify = MFC_Filters[nID].notify | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
	if (this->IsDlgButtonChecked(IDC_NOTIFY_IN_SENDER))
		MFC_Filters[nID].notify = MFC_Filters[nID].notify | MAILFILTER_NOTIFICATION_SENDER_INCOMING;
	if (this->IsDlgButtonChecked(IDC_NOTIFY_OUT_SENDER))
		MFC_Filters[nID].notify = MFC_Filters[nID].notify | MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
	if (this->IsDlgButtonChecked(IDC_NOTIFY_IN_RECIPIENT))
		MFC_Filters[nID].notify = MFC_Filters[nID].notify | MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
	if (this->IsDlgButtonChecked(IDC_NOTIFY_OUT_RECIPIENT))
		MFC_Filters[nID].notify = MFC_Filters[nID].notify | MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;

	CPropertyPage::OnOK();
}


void CFilterPropertyDlg::OnCbnSelchangeMatchfield()
{
}
