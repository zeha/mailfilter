// TemplateEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "TemplateEditorDlg.h"


// CTemplateEditorDlg dialog

IMPLEMENT_DYNAMIC(CTemplateEditorDlg, CDialog)
CTemplateEditorDlg::CTemplateEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateEditorDlg::IDD, pParent)
{
	this->szCurrentFilename = "";
}

CTemplateEditorDlg::~CTemplateEditorDlg()
{
}

void CTemplateEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CTemplateEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CFont* pFont = new CFont();
	CEdit* textBox = (CEdit*)this->GetDlgItem(IDC_TEXTEDITOR);
	VERIFY(pFont->CreatePointFont(90 /*9pt*/,"Courier New"));

	textBox->SetFont(pFont);
	textBox->SetReadOnly(TRUE);
	
	CListBox* pListBox = (CListBox*)this->GetDlgItem(IDC_TEMPLATETYPE);
	pListBox->ResetContent();
	pListBox->AddString("REPORT.TPL Admin Notifications");
	pListBox->AddString("RINSIDE.TPL Internal User Notifications");
	pListBox->AddString("ROUTRCPT.TPL Inbound E-Mail, Internal User Notification");
	pListBox->AddString("ROUTSNDR.TPL Inbound E-Mail, Outbound User Notification");

	return FALSE;
}

BEGIN_MESSAGE_MAP(CTemplateEditorDlg, CDialog)
	ON_EN_CHANGE(IDC_TEXTEDITOR, OnEnChangeTexteditor)
	ON_LBN_SELCHANGE(IDC_TEMPLATETYPE, OnLbnSelchangeTemplatetype)
	ON_BN_CLICKED(IDC_INSERTDYNAMICTEXT, OnBnClickedInsertdynamictext)
	ON_COMMAND(ID_DYNAMICTEXTS_ORIGINALSENDER, OnDynamictextsOriginalSender)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_COMMAND(ID_DYNAMICTEXTS_ORIGINALRECIPIENT, OnDynamictextsOriginalRecipient)
	ON_COMMAND(ID_DYNAMICTEXTS_ORIGINALSUBJECT, OnDynamictextsOriginalSubject)
	ON_COMMAND(ID_DYNAMICTEXTS_DROPREASON, OnDynamictextsDropReason)
	ON_COMMAND(ID_DYNAMICTEXTS_MAILFILENAME, OnDynamictextsMailFilename)
	ON_COMMAND(ID_DYNAMICTEXTS_POSTMASTEREMAIL, OnDynamictextsPostmasterEmail)
	ON_COMMAND(ID_DYNAMICTEXTS_MAILFILTEREMAIL, OnDynamictextsMailfilterEmail)
	ON_COMMAND(ID_DYNAMICTEXTS_SERVERNAME, OnDynamictextsServerName)
	ON_COMMAND(ID_DYNAMICTEXTS_MAILFILTERVERSION, OnDynamictextsMailfilterVersion)
END_MESSAGE_MAP()


// CTemplateEditorDlg message handlers

void CTemplateEditorDlg::OnEnChangeTexteditor()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
}

void CTemplateEditorDlg::OnLbnSelchangeTemplatetype()
{
	CString szCompleteFilename = "";
	CString szBuffer = "";
	CEdit* textBox = (CEdit*)this->GetDlgItem(IDC_TEXTEDITOR);
	CFile textFile;
	char szBufferCharArray[20000];
	int selection = 0;

	if (this->szCurrentFilename.GetLength() > 0)
	{
		szBuffer = "";
		szCompleteFilename = ((CMFConfigApp*)AfxGetApp())->MF_ConfigPath + "\\" + this->szCurrentFilename;
		
		this->GetDlgItemText(IDC_TEXTEDITOR,szBuffer);
		textFile.Open(szCompleteFilename,CFile::modeWrite);

		textFile.Write(szBuffer,szBuffer.GetLength());
		textFile.Close();
	}

	CListBox* pListBox = (CListBox*)this->GetDlgItem(IDC_TEMPLATETYPE);
	selection = pListBox->GetCurSel();
	pListBox->GetText(selection,this->szCurrentFilename);

	this->szCurrentFilename = this->szCurrentFilename.SpanExcluding(" ,-;");

	szCompleteFilename = ((CMFConfigApp*)AfxGetApp())->MF_ConfigPath + "\\" + this->szCurrentFilename;

	memset(szBufferCharArray,0,19999);

	CFileFind ffind;
	if (!ffind.FindFile(szCompleteFilename))
	{
		textFile.Open(szCompleteFilename,CFile::modeCreate);
	} else {
		textFile.Open(szCompleteFilename,CFile::modeRead);
		textFile.Read(szBufferCharArray,19999);
	}
	textFile.Close();

	textBox->SetReadOnly(FALSE);
	this->SetDlgItemText(IDC_TEXTEDITOR,szBufferCharArray);

}

void CTemplateEditorDlg::OnBnClickedInsertdynamictext()
{
	RECT buttonrect;
	CMenu menu;
	CMenu* popupMenu;
	BOOL rc;

	this->GetDlgItem(IDC_INSERTDYNAMICTEXT)->GetWindowRect(&buttonrect);

	rc = menu.LoadMenu(IDR_DYNAMICTEXTS);
	popupMenu = menu.GetSubMenu(0);
	rc = popupMenu->TrackPopupMenu(TPM_LEFTALIGN, buttonrect.right, buttonrect.top, this);

}

void CTemplateEditorDlg::PasteText(CString str)
{
	CEdit* textBox = (CEdit*)this->GetDlgItem(IDC_TEXTEDITOR);
	if ( (textBox->GetStyle() & ES_READONLY) != ES_READONLY )
	{
		textBox->ReplaceSel(str);
		textBox->SetFocus();
	}
}

void CTemplateEditorDlg::OnDynamictextsOriginalSender()
{
	this->PasteText("%f");
}

void CTemplateEditorDlg::OnBnClickedOk()
{
	CString szCompleteFilename = "";
	CString szBuffer = "";
	CFile textFile;

	if (this->szCurrentFilename.GetLength() > 0)
	{
		szBuffer = "";
		szCompleteFilename = ((CMFConfigApp*)AfxGetApp())->MF_ConfigPath + "\\" + this->szCurrentFilename;
		
		this->GetDlgItemText(IDC_TEXTEDITOR,szBuffer);
		textFile.Open(szCompleteFilename,CFile::modeWrite);
		textFile.Write(szBuffer,szBuffer.GetLength());
		textFile.Close();
	}

	OnOK();
}

void CTemplateEditorDlg::OnDynamictextsOriginalRecipient()
{	this->PasteText("%t");	}

void CTemplateEditorDlg::OnDynamictextsOriginalSubject()
{	this->PasteText("%s");	}

void CTemplateEditorDlg::OnDynamictextsDropReason()
{	this->PasteText("%R");	}

void CTemplateEditorDlg::OnDynamictextsMailFilename()
{	this->PasteText("%F");	}

void CTemplateEditorDlg::OnDynamictextsPostmasterEmail()
{	this->PasteText("%P");	}

void CTemplateEditorDlg::OnDynamictextsMailfilterEmail()
{	this->PasteText("%M");	}

void CTemplateEditorDlg::OnDynamictextsServerName()
{	this->PasteText("%S");	}

void CTemplateEditorDlg::OnDynamictextsMailfilterVersion()
{	this->PasteText("%V");	}
