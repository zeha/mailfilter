// FilterPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "FilterPropertyDlg.h"
#include "filterice.hxx"
#include "../MailFilter/Main/mftools.h"

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
	box->AddString("Always");
	box->AddString("Attachment Name");
	box->AddString("E-Mail Address: From, To, CC");
	box->AddString("E-Mail Address: From, To");
	box->AddString("E-Mail Address: From");
	box->AddString("E-Mail Address: To, CC");
	box->AddString("E-Mail Address: To");
	box->AddString("Relay Host blacklisted");
	box->AddString("Relay Host unresolvable");
	box->AddString("Size");
	box->AddString("Subject Line");
	box->AddString("Archive Content: Name");
	box->AddString("Archive Content: File Count");
	box->AddString("Virus Scan");

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHTYPE);
	box->AddString("Match");
	box->AddString("No Match");

	if (m_NewEntry)
	{
		CheckDlgButton(IDC_NOTIFY_IN_SENDER		, 0 );
		CheckDlgButton(IDC_NOTIFY_OUT_SENDER	, 0 );
		CheckDlgButton(IDC_NOTIFY_IN_RECIPIENT	, 0 );
		CheckDlgButton(IDC_NOTIFY_OUT_RECIPIENT	, 0 );

		return TRUE;
	}

	CListCtrl* cv = (CListCtrl*)this->GetParent()->GetParent()->GetDlgItem(IDC_CONFIGLIST);

	int nItem = -1;
	nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
	ASSERT(nItem != -1);
	int nID = (int)cv->GetItemData(nItem);
	SetDlgItemText(IDC_FILTERNAME,				MFC_FilterList[nID].name.c_str());
	SetDlgItemText(IDC_FILTEREXPRESSION,		MFC_FilterList[nID].expression.c_str());
	
	/*
enum FilterField { always = 0, attachment = 1, email = 2, subject = 3, size = 4, emailFrom = 5, emailTo = 6, blacklist = 7, ipUnresolvable = 8, archiveContentName = 9, archiveContentCount = 10, emailToAndCC = 11, emailBothAndCC = 12, virus = 13 }; 
enum FilterAction { noAction = 0, dropMail = 1, moveMail = 2, rescan = 4, pass = 8, schedule = 0x10, noschedule = 0x20, copy = 0x40 };
enum Notification { noNotification = 0, adminIncoming = 1, adminOutgoing = 2, senderIncoming = 4, senderOutgoing = 8, recipientIncoming = 0x10, recipientOutgoing = 0x20 };
enum FilterType { match = 0, noMatch = 1 };
	*/

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHACTION);
	switch (MFC_FilterList[nID].action)
	{
	case MailFilter_Configuration::pass:
        box->SetCurSel(0);		break;
	case MailFilter_Configuration::dropMail:
        box->SetCurSel(1);		break;
	case MailFilter_Configuration::schedule:
        box->SetCurSel(2);		break;
	case MailFilter_Configuration::noschedule:
        box->SetCurSel(3);		break;
	case MailFilter_Configuration::copy:
        box->SetCurSel(4);		break;
	}

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHFIELD);
	switch (MFC_FilterList[nID].matchfield)
	{
	case MailFilter_Configuration::always:
        box->SetCurSel(0);		break;
	case MailFilter_Configuration::attachment:
        box->SetCurSel(1);		break;
	case MailFilter_Configuration::emailBothAndCC:
        box->SetCurSel(2);		break;
	case MailFilter_Configuration::email:
        box->SetCurSel(3);		break;
	case MailFilter_Configuration::emailFrom:
        box->SetCurSel(4);		break;
	case MailFilter_Configuration::emailToAndCC:
        box->SetCurSel(5);		break;
	case MailFilter_Configuration::emailTo:
        box->SetCurSel(6);		break;
	case MailFilter_Configuration::blacklist:
        box->SetCurSel(7);		break;
	case MailFilter_Configuration::ipUnresolvable:
        box->SetCurSel(8);		break;
	case MailFilter_Configuration::size:
        box->SetCurSel(9);		break;
	case MailFilter_Configuration::subject:
        box->SetCurSel(10);		break;
	case MailFilter_Configuration::archiveContentName:
        box->SetCurSel(11);		break;
	case MailFilter_Configuration::archiveContentCount:
        box->SetCurSel(12);		break;
	case MailFilter_Configuration::virus:
        box->SetCurSel(13);		break;
	}

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHTYPE);
	switch (MFC_FilterList[nID].type)
	{
	case MailFilter_Configuration::match:
		box->SetCurSel(0);		break;
	case MailFilter_Configuration::noMatch:
		box->SetCurSel(1);		break;
	}

	CheckDlgButton(IDC_ENABLED_IN,MFC_FilterList[nID].enabledIncoming);
	CheckDlgButton(IDC_ENABLED_OUT,MFC_FilterList[nID].enabledOutgoing);

	CheckDlgButton(IDC_NOTIFY_IN_ADMIN		, chkFlag( MFC_FilterList[nID].notify , MAILFILTER_NOTIFICATION_ADMIN_INCOMING ) );
	CheckDlgButton(IDC_NOTIFY_OUT_ADMIN		, chkFlag( MFC_FilterList[nID].notify , MAILFILTER_NOTIFICATION_ADMIN_OUTGOING ) );
	CheckDlgButton(IDC_NOTIFY_IN_SENDER		, chkFlag( MFC_FilterList[nID].notify , MAILFILTER_NOTIFICATION_SENDER_INCOMING ) );
	CheckDlgButton(IDC_NOTIFY_OUT_SENDER	, chkFlag( MFC_FilterList[nID].notify , MAILFILTER_NOTIFICATION_SENDER_OUTGOING ) );
	CheckDlgButton(IDC_NOTIFY_IN_RECIPIENT	, chkFlag( MFC_FilterList[nID].notify , MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING ) );
	CheckDlgButton(IDC_NOTIFY_OUT_RECIPIENT	, chkFlag( MFC_FilterList[nID].notify , MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING ) );

	return FALSE;
}

void CFilterPropertyDlg::OnOK() 
{
	CComboBox* box;
	int nID = 0;
	MailFilter_Configuration::Filter *flt;

	if (!m_NewEntry)
	{
		CListCtrl* cv = (CListCtrl*)this->GetParent()->GetParent()->GetDlgItem(IDC_CONFIGLIST);

		int nItem = -1;
		nItem = cv->GetNextItem(nItem, LVNI_SELECTED);
		ASSERT(nItem != -1);
		nID = (int)cv->GetItemData(nItem);
		flt = &MFC_FilterList[nID];
	} else {
		flt = new MailFilter_Configuration::Filter();
//		for (nID = 0; nID < MailFilter_MaxFilters; nID++)
//			if (MFC_Filters[nID].expression[0] == 0)
//				break;
	}

	CString szTmp;
	this->GetDlgItemText(IDC_FILTEREXPRESSION,szTmp);
	if (szTmp == "")
	{
		AfxMessageBox("The expression must be filled in!",MB_OK,0);
		return;
	}

	char szTemp[255];

	this->GetDlgItemText(IDC_FILTERNAME,			szTemp,			60	);
	flt->name = szTemp;

	this->GetDlgItemText(IDC_FILTEREXPRESSION,		szTemp,	250	);
	flt->expression = szTemp;
	
	box = (CComboBox*)this->GetDlgItem(IDC_MATCHACTION);
	switch (box->GetCurSel())
	{
	case 0:
		flt->action = (MailFilter_Configuration::FilterAction)MAILFILTER_MATCHACTION_PASS;			break;
	case 1:
		flt->action = (MailFilter_Configuration::FilterAction)MAILFILTER_MATCHACTION_DROP_MAIL;		break;
	case 2:
		flt->action = (MailFilter_Configuration::FilterAction)MAILFILTER_MATCHACTION_SCHEDULE;		break;
	case 3:
		flt->action = (MailFilter_Configuration::FilterAction)MAILFILTER_MATCHACTION_NOSCHEDULE;	break;
	case 4:
		flt->action = (MailFilter_Configuration::FilterAction)MAILFILTER_MATCHACTION_COPY;			break;
	}

	box = (CComboBox*)this->GetDlgItem(IDC_MATCHFIELD);
	switch (box->GetCurSel())
	{
	case 0:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_ALWAYS;			break;
	case 1:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_ATTACHMENT;		break;
	case 2:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC;	break;
	case 3:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL;			break;
	case 4:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_FROM;		break;
	case 5:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_TOANDCC;	break;
	case 6:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_TO;		break;
	case 7:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_BLACKLIST;		break;
	case 8:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_IPUNRESOLVABLE;	break;
	case 9:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_SIZE;			break;
	case 10:
		flt->matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_SUBJECT;		break;
	}


	box = (CComboBox*)this->GetDlgItem(IDC_MATCHTYPE);
	switch (box->GetCurSel())
	{
	case 0:
		flt->type = (MailFilter_Configuration::FilterType)MAILFILTER_MATCHTYPE_MATCH;					break;
	case 1:
		flt->type = (MailFilter_Configuration::FilterType)MAILFILTER_MATCHTYPE_NOMATCH;				break;
	}

	flt->enabledIncoming = (bool)this->IsDlgButtonChecked(IDC_ENABLED_IN);
	flt->enabledOutgoing = (bool)this->IsDlgButtonChecked(IDC_ENABLED_OUT);

	flt->notify = MailFilter_Configuration::noNotification;

	if (this->IsDlgButtonChecked(IDC_NOTIFY_IN_ADMIN))
		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MailFilter_Configuration::adminIncoming);
	if (this->IsDlgButtonChecked(IDC_NOTIFY_OUT_ADMIN))
		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING);
	if (this->IsDlgButtonChecked(IDC_NOTIFY_IN_SENDER))
		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_SENDER_INCOMING);
	if (this->IsDlgButtonChecked(IDC_NOTIFY_OUT_SENDER))
		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_SENDER_OUTGOING);
	if (this->IsDlgButtonChecked(IDC_NOTIFY_IN_RECIPIENT))
		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING);
	if (this->IsDlgButtonChecked(IDC_NOTIFY_OUT_RECIPIENT))
		flt->notify = (MailFilter_Configuration::Notification)(flt->notify | MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING);

	if (m_NewEntry)
		MFC_FilterList.push_back(*flt);


	CPropertyPage::OnOK();
}


void CFilterPropertyDlg::OnCbnSelchangeMatchfield()
{
}
