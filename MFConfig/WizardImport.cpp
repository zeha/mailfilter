// WizardImport.cpp : implementation file
//

#include "stdafx.h"
#include "MFConfig.h"
#include "WizardImport.h"
#include "../MailFilter/Config/MFConfig.h"

extern MailFilter_Filter MFC_Filters[MailFilter_MaxFilters];

// CWizardImport dialog

IMPLEMENT_DYNAMIC(CWizardImport, CPropertyPage)
CWizardImport::CWizardImport()
	: CPropertyPage(CWizardImport::IDD)
{
	m_pPSP->dwFlags |= PSP_DEFAULT|PSP_USEHEADERTITLE|PSP_USEHEADERSUBTITLE;
	m_pPSP->pszHeaderTitle = "Import Rule Set";
	m_pPSP->pszHeaderSubTitle = "This wizard allows you to import a complete rule set.";
}

CWizardImport::~CWizardImport()
{
}

void CWizardImport::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWizardImport, CPropertyPage)
END_MESSAGE_MAP()


// CWizardImport message handlers
BOOL CWizardImport::OnSetActive(void)
{
   CPropertySheet* psheet = (CPropertySheet*) GetParent();   
   psheet->SetWizardButtons(PSWIZB_FINISH);

   return CPropertyPage::OnSetActive();
}

int extractString(char* inStr, char* szOut)
{
	int cpos = 0;
	int strStart = -1;
	int tmpCharsRead = 0;
	int len = (int)strlen(inStr);

	while ( (inStr[cpos++] != '\"') && (inStr[cpos] != 0) ) {}; // bis zum ersten " lesen
	if (inStr[cpos] == 0)
		return 0;

	strStart = cpos;
	for (cpos = strStart ; cpos < len ; cpos++)
	{
		szOut[cpos-strStart] = inStr[cpos];
		if (inStr[cpos] == '\0')
		{	szOut[0] = 0;	/* String ungültig..., kein 2tes " gefunden */
			return 0;
		}
		
		if (inStr[cpos] == '\"')
		{
			if (
					( (inStr[cpos+1] == '\r') )
					||
					( (inStr[cpos+1] == '\n') )
					||
					( (inStr[cpos+1] == 0) )
					||
					( (inStr[cpos+1] == ',') ) 
					|| 
					( (inStr[cpos+1] == ' ') && (inStr[cpos+2] == ',') )
				)
			{
				/* TODO ergänzen string weiterlesen/test */
				szOut[cpos-strStart] = 0;
				return cpos+1;
				break;
			}
//			if (inStr[cpos+1] == 
		}

	}
	return 0;
}

BOOL CWizardImport::OnWizardFinish(void)
{
	FILE* fLst;
	CString szFile;
	int rc;
	int curItem = -1;
	char defaultNotify;
	bool overrideNotify = false;
	bool deleteRules = false;
	char szTemp[1001];

	int iImportMode = 0;
	if (this->IsDlgButtonChecked(IDC_TYPE_CSV))
		iImportMode = 1;	//CSV

	this->GetDlgItemText(IDC_IMPORTPATH, szFile);
	
	defaultNotify = MAILFILTER_NOTIFICATION_ADMIN_INCOMING | MAILFILTER_NOTIFICATION_ADMIN_OUTGOING;
	if (MFC_Notification_EnableInternalRcpt)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_INCOMING;
	if (MFC_Notification_EnableInternalSndr)	defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_OUTGOING;
	if (MFC_Notification_EnableExternalRcpt)	defaultNotify |= MAILFILTER_NOTIFICATION_RECIPIENT_OUTGOING;
	if (MFC_Notification_EnableExternalSndr)	defaultNotify |= MAILFILTER_NOTIFICATION_SENDER_INCOMING;
	
	szTemp[0] = 0;
	
/*		if (access(szTemp,F_OK) != 0)
		{
			NWSDisplayErrorText ( ERROR_FILE_NOT_FOUND , SEVERITY_FATAL , NLM_nutHandle , szTemp );
			
		} else {
*/		
	if (this->IsDlgButtonChecked(IDC_IGNORENOTIFY))
		overrideNotify = true;
	else
		overrideNotify = false;

	if (this->IsDlgButtonChecked(IDC_DELETEALL))
		deleteRules = true;
	else
		deleteRules = false;

	if (iImportMode == 0)
		fLst = fopen(szFile,"rb");
	if (iImportMode == 1)
		fLst = fopen(szFile,"rt");

	if (!fLst)
	{
		AfxMessageBox("The file specified could not be written to. Please select another file.",MB_OK,0);
		return FALSE;
	}

	for (curItem = 0; curItem<(MailFilter_MaxFilters+1); curItem++)
	{
		if (deleteRules)
		{
			//memset((void)(&MFC_Filters[curItem]),1,sizeof(MailFilter_Filter));
			MFC_Filters[curItem].action = 0;
			MFC_Filters[curItem].enabled = 0;
			MFC_Filters[curItem].enabledIncoming = 0;
			MFC_Filters[curItem].enabledOutgoing = 0;
			MFC_Filters[curItem].expression[0] = 0;
			MFC_Filters[curItem].matchfield = 0;
			MFC_Filters[curItem].name[0] = 0;
			MFC_Filters[curItem].notify = 0;
			MFC_Filters[curItem].type = 0;
		}
		else
			if (MFC_Filters[curItem].expression[0] == 0)
				break;
	}

	/*if (access(szFile,F_OK) == 0)
	{
		//NWSDisplayErrorText ( ERROR_FILE_EXISTS , SEVERITY_FATAL , NLM_nutHandle , szTemp );
		AfxMessageBox("The file specified already exists. MFConfig will not overwrite existing files. Please select another file.",MB_OK,0);
		return FALSE;
		
	} else {
	*/
	
	curItem = 0;

	if (iImportMode == 0)
	{
		fseek(fLst, 54, SEEK_SET);

		while (!feof(fLst))
		{
			if (curItem > MailFilter_MaxFilters)
				break;

			MFC_Filters[curItem].matchfield = (char)fgetc(fLst);
			MFC_Filters[curItem].notify = (char)fgetc(fLst);
			MFC_Filters[curItem].type = (char)fgetc(fLst);
			MFC_Filters[curItem].action = (char)fgetc(fLst);
			MFC_Filters[curItem].enabled = (char)fgetc(fLst);
			MFC_Filters[curItem].enabledIncoming = (char)fgetc(fLst);
			MFC_Filters[curItem].enabledOutgoing = (char)fgetc(fLst);
			
			if (overrideNotify == true)
				MFC_Filters[curItem].notify = defaultNotify;
			
			rc = (long)fread(szTemp,sizeof(char),500,fLst);
			fseek(fLst,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
			
			if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
				else MFC_Filters[curItem].expression[0]=0;		

			rc = (long)fread(szTemp,sizeof(char),60,fLst);
			fseek(fLst,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

			if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
				else MFC_Filters[curItem].name[0]=0;
			
			if (MFC_Filters[curItem].expression[0]==0)
				break;

			curItem++;

		}
	}
	if (iImportMode == 1)
	{
	
#define mfi MFC_Filters[curItem]
		//int read = 500;
		//fread(szTemp,500,read,fLst);
		fgets(szTemp,500,fLst);

		while (!feof(fLst))
		{
			if (curItem > MailFilter_MaxFilters)
				break;
			
			rc = fscanf(fLst," 0x%hhx %*[,] 0x%hhx %*[,] 0x%hhx %*[,] 0x%hhx %*[,] %hhd %*[,] %hhd %*[,] %hhd %*[,]",// %*[\"]%[abcdefghijklmnopqrstuvwxyzABCDEFQRSTUVWXYZ -_.,;:@!"§$%&/()=?0123456789]%*[\"] %*[,] %*[\"]%s%*[\"] \n",
				&MFC_Filters[curItem].matchfield,
				&MFC_Filters[curItem].notify,
				&MFC_Filters[curItem].type,
				&MFC_Filters[curItem].action,
				&MFC_Filters[curItem].enabled,
				&MFC_Filters[curItem].enabledIncoming,
				&MFC_Filters[curItem].enabledOutgoing);

			if (overrideNotify == true)
				MFC_Filters[curItem].notify = defaultNotify;

			szTemp[0]=0;
			fgets(szTemp,1000,fLst);
			
			rc = extractString(szTemp,MFC_Filters[curItem].name);
//			printf("%s - ",mfi.name);
			rc = extractString(szTemp+rc,MFC_Filters[curItem].expression);
//			printf("%s\n",mfi.expression);
			
//			if (! ( (MFC_Filters[curItem].enabled == 0) || (MFC_Filters[curItem].enabled == 1) ) )
//			{	printf("Ungueltiger Eintrag (enabled) in Zeile %d\n",curItem+1);	fclose(fFile); return;	}
//			if (! ( (MFC_Filters[curItem].enabledIncoming == 0) || (MFC_Filters[curItem].enabledIncoming == 1) ) )
//			{	printf("Ungueltiger Eintrag (enabledIncoming) in Zeile %d\n",curItem+1);	fclose(fFile); return;	}
//			if (! ( (MFC_Filters[curItem].enabledOutgoing == 0) || (MFC_Filters[curItem].enabledOutgoing == 1) ) )
//			{	printf("Ungueltiger Eintrag (enabledOutgoing) in Zeile %d\n",curItem+1);	fclose(fFile); return;	}
			
			sprintf(szTemp,"%4d: 0x%02x 0x%02x 0x%02x 0x%02x %d %d %d ",curItem+1,mfi.matchfield,mfi.notify,mfi.type,mfi.action,mfi.enabled,mfi.enabledIncoming,mfi.enabledOutgoing);
			if (curItem < 4)
				this->MessageBox(szTemp);

			if (MFC_Filters[curItem].expression[0]==0)
				break;

			curItem++;

		}
	}
	fclose(fLst);

	sprintf(szTemp,"Imported %d items.",curItem);
	this->MessageBox(szTemp,NULL,MB_ICONINFORMATION);

	return TRUE;
}

BOOL CWizardImport::OnInitDialog(void)
{
	this->CheckDlgButton(IDC_TYPE_MFREX,1);
	return 0;
}
