/*
 *	MFpack.c++
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
#include <library.h> 
#include "MFZip.h"

#define PATH_PACKZIP "SYS:\\SYSTEM\\MFPACK.ZIP"

int main (int argc, char* argv[])
{
#pragma unused(argc,argv)

	unlink(PATH_PACKZIP);
	
	{	
		// Zip stuff
		MFZip *zip;
		
		zip = new MFZip(PATH_PACKZIP,9,1);

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
	
	printf(" MFPACK Complete! Output written to: "PATH_PACKZIP".\n");
	
	
	return 0;
}
