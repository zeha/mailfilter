/*
 *	MFBug.c++
 *  © Copyright 2004 Christian Hofstädtler
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <nks/thread.h>
#include <screen.h> 
#include <stdlib.h>
#include <stdarg.h> 
#include "MFZip.h"
#include "MFUnZip.h"
#include "mfbug2.h"

#define PATH_CONFIGTXT "SYS:\\SYSTEM\\MFBUG.TXT"
#define PATH_CONFIGTXT2 "SYS:\\SYSTEM\\MFBUG.CFG"
#define PATH_BUGZIP "SYS:\\SYSTEM\\MFBUG.MFZ"

#define PATH_MFBUG2NLM "SYS:\\SYSTEM\\MFBUG2.NLM"

int main (int argc, char* argv[])
{
#pragma unused(argc)

	printf("\n");
	printf(" MailFilter/ax 1.5 bug reporting tool\n\n");
	printf(" This process will collect information for bug reports.\n");
	pressenter();

	unlink(PATH_CONFIGTXT);
	unlink(PATH_CONFIGTXT2);
	unlink(PATH_BUGZIP);
	unlink(PATH_MFBUG2NLM);
	
	{
		if (mfbug2nlm_zip_data[0] == 0)
		{
			printf("** Embedded data is corrupt.\n**You may want to redownload mfbug.nlm.\n");
			return 3;
		}
	}
	
	{
		// unpack mfbug2.nlm from internal file
		MFUnZip *unzip;

		unzip = new MFUnZip(argv[0]);
		
		if (unzip->ExtractFile("mfbug2.nlm",PATH_MFBUG2NLM) != MFUnZip_OK)
		{
			printf("** Could not unpack required files.\n**You may want to redownload mfbug.nlm.\n");
			delete(unzip);
			return 2;
		}
		
		delete(unzip);
	}
	
	{
		// config.txt stuff
		
		time_t starttime = time(NULL);
		
	
		printf(" Loading MFBUG2.NLM ...\n");
		printf("         Please wait for this to complete.\n");
		
		LoadModule(getscreenhandle(),PATH_MFBUG2NLM" /all /o=MFBUG.TXT /d /c /f /a9",LO_LOAD_SILENT);
		// let the user see the logger, output from config.nlm
		ActivateScreen(getnetwarelogger());
		//system("LOAD MFBUG2.NLM /all /o=MFBUG.TXT /d /c /f /a9");
		
		while (rename(PATH_CONFIGTXT,PATH_CONFIGTXT2))
		{
			NXThreadYield();
			if (difftime(time(NULL),starttime) > (5*60))
			{
				// we are waiting more than 5 minutes now...
				
				int key;
				
				// user interaction on our screen...
				ActivateScreen(getscreenhandle());
				
				printf("** mfbug2.nlm did not complete.\n** Do you want to wait another 5 minutes? (y/n)\n");
				key = getchar();
				
				if ( (key == 121) || (key == 89) )
					starttime = time(NULL);
				else
				{
					printf(" Process aborted!\n");
					pressanykey();
					return 1;
				}
				ActivateScreen(getnetwarelogger());
			}
		}
		// switch back to our screen
		ActivateScreen(getscreenhandle());
	}	

	{	
		// Zip stuff
		MFZip *zip;
		
		printf(" One more moment please...\n");

		zip = new MFZip(PATH_BUGZIP,9,1);

		zip->AddFile("CONFIG.TXT",PATH_CONFIGTXT2);
		

		zip->AddFile("ETC\\HOSTS","SYS:\\ETC\\HOSTS");
		zip->AddFile("ETC\\HOSTNAME","SYS:\\ETC\\HOSTNAME");
		zip->AddFile("ETC\\PATHTAB","SYS:\\ETC\\PATHTAB");
		zip->AddFile("ETC\\RESOLV.CFG","SYS:\\ETC\\RESOLV.CFG");

		zip->AddFile("SYSTEM\\MAILFLT.NLM","SYS:\\System\\mailflt.nlm");
		zip->AddFile("SYSTEM\\MAILFLT.BAK","SYS:\\System\\mailflt.bak");

		zip->AddFile("SYSTEM\\MFCONFIG.NLM","SYS:\\System\\MFCONFIG.NLM");
		zip->AddFile("SYSTEM\\MFNRM.NLM","SYS:\\System\\MFNRM.NLM");
		zip->AddFile("SYSTEM\\MFREST.NLM","SYS:\\System\\MFREST.NLM");
		zip->AddFile("SYSTEM\\MFUPGR.NLM","SYS:\\System\\MFUPGR.NLM");

		zip->AddFile("SYSTEM\\MFSTART.NCF","SYS:\\System\\MFSTART.NCF");
		zip->AddFile("SYSTEM\\MFSTOP.NCF","SYS:\\System\\MFSTOP.NCF");

		zip->AddFile("SYSTEM\\GWIA.CFG","SYS:\\SYSTEM\\GWIA.CFG");
		zip->AddFile("SYSTEM\\GWIA.NCF","SYS:\\SYSTEM\\GWIA.NCF");

		zip->AddFile("MAILFLT\\EXEPATH.CFG","SYS:\\ETC\\MAILFLT\\EXEPATH.CFG");
		zip->AddFile("MAILFLT\\CONFIG.BIN","SYS:\\ETC\\MAILFLT\\CONFIG.BIN");
		zip->AddFile("MAILFLT\\CONFIG.BAK","SYS:\\ETC\\MAILFLT\\CONFIG.BAK");

		zip->AddFile("MAILFLT\\MAILCOPY.TPL","SYS:\\ETC\\MAILFLT\\MAILCOPY.TPL");
		zip->AddFile("MAILFLT\\REPORT.TPL","SYS:\\ETC\\MAILFLT\\REPORT.TPL");
		zip->AddFile("MAILFLT\\RINSIDE.TPL","SYS:\\ETC\\MAILFLT\\RINSIDE.TPL");
		zip->AddFile("MAILFLT\\ROUTRCPT.TPL","SYS:\\ETC\\MAILFLT\\ROUTRCPT.TPL");
		zip->AddFile("MAILFLT\\ROUTSNDR.TPL","SYS:\\ETC\\MAILFLT\\ROUTSNDR.TPL");
		
		delete(zip);		// close it
		
	}
	
	unlink(PATH_CONFIGTXT);
	unlink(PATH_CONFIGTXT2);
	unlink(PATH_MFBUG2NLM);
	
	printf(" MFBUG Complete!\n\n");
	printf(" Output written to: "PATH_BUGZIP".\n");
	printf(" Please e-mail this file with your problem description to:\n   support@mailfilter.cc \n");
	
	printf("\n\n");
	
	pressanykey();
	
	return 0;
}
