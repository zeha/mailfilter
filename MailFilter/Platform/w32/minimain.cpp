/*
 +		minimain.cpp
 +		
 +		Win32/Main
 +		
 */

#ifdef WIN32

#define _MFD_MODULE			"minimain.CPP"

#include "..\..\Main\MailFilter.h"
#include "MFMail.h++"

extern int MF_HandleMailFile(MailFilter_MailData* m);

int main( int argc, char **argv )
{

	if (argc<2)
	{
		printf("this minimf needs a mail file parameter.\n");
		return 1;
	}

//	if (!MF_GlobalConfiguration.ReadFromFile(""))
//		goto MF_WIN32_MAIN_TERMINATE;

	MF_GlobalConfiguration.setDefaults();
	MF_GlobalConfiguration.EnableAttachmentDecoder = true;

	// Init Status
	if (!MF_StatusInit())
		return 1;

	MFC_MAILSCAN_Enabled = 1;
	MFT_Debug = 0xFFFF;


	int rc = -1;
	
	MailFilter_MailData* m = MailFilter_MailInit((char*)argv[1],0);

	strcpy(m->szScanDirectory,"E:\\MF\\SCAN\\");
	sprintf(m->szFileWork, "%s%s", m->szScanDirectory, argv[1] );

	rc = MF_HandleMailFile ( m );

	printf("rc: %d\n",rc);

	return 0;
}


#endif // WIN32

/*
 *
 *
 *  --- eof ---
 *
 *
 */

