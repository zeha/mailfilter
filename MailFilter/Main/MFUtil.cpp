/*+
 +		MFUtil.cpp
 +		
 +		Utility Functions
 +		
 +		Copyright 2001 Christian Hofstädtler.
 +		
 +		
 +		- Aug/2001 ; ch   ; Initial Code
 +		
 +		
+*/

#define _MFD_MODULE			"MFUTIL.CPP"
#include "MailFilter.h"
#include "MFVersion.h"
#include <string>


#ifdef HAVE_WINSOCK
bool WinSockStartup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	 
	wVersionRequested = MAKEWORD( 2, 2 );
	 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
	    /* Tell the user that we could not find a usable */
	    /* WinSock DLL.                                  */
	    return false;
	}
	 
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	 
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
	        HIBYTE( wsaData.wVersion ) != 2 ) {
	    /* Tell the user that we could not find a usable */
	    /* WinSock DLL.                                  */
	    WSACleanup( );
	    return false; 
	}
	return true;
}

bool WinSockShutdown()
{
	WSACleanup( );
	return true;
}
#endif



#ifdef IXPLAT_WIN32
char* GetRegValue( HKEY hRoot , char* lpszSubKey , char* lpszValue , char* lpszDefault )
{
	long rc;
	HKEY hKey;
	char szBuf[2*MAX_PATH];
    unsigned long dwBufLen = 2*MAX_PATH;
	szBuf[0]='\0';

	rc = RegCreateKey(hRoot,
					lpszSubKey,
                    &hKey);
	

	rc = RegQueryValueEx(hKey,
					lpszValue,
                    NULL,
                    NULL,
                    (LPBYTE)szBuf,
                    &dwBufLen);

	rc = RegCloseKey( hKey );
	

	if (strlen(szBuf) == 0) { strcpy(szBuf,lpszDefault); }

	return szBuf;

}	

#endif


int MF_CountFilters(int action)
{
	unsigned int curItem;
	int num = 0;
	
	for (curItem = 0; curItem < MF_GlobalConfiguration.filterList.size(); curItem++)
	{
		if (MF_GlobalConfiguration.filterList[curItem].expression == "")
			break;

		if (
			(MF_GlobalConfiguration.filterList[curItem].action == action)
			&& 
			(MF_GlobalConfiguration.filterList[curItem].enabled == true)
			&&
			( (MF_GlobalConfiguration.filterList[curItem].enabledIncoming == true) || (MF_GlobalConfiguration.filterList[curItem].enabledOutgoing == true) )
		) {

			num++;
		
		}
	}
	return num;
}

int MF_CountAllFilters()
{
	unsigned int curItem;
	int num = 0;
	
	for (curItem = 0; curItem < MF_GlobalConfiguration.filterList.size(); curItem++)
	{
		if (MF_GlobalConfiguration.filterList[curItem].expression == "")
			break;

		num++;
	}
	return num;
}

int MFUtil_CheckCurrentVersion()
{
	// Version Up2Date check
	struct hostent *he;

	he = gethostbyname("version-1.mailfilter.cc");
	if (he == NULL)
	{
		MF_StatusLog("Error while checking for MailFilter updates.");
		MFD_Out(MFD_SOURCE_CONFIG,"Could not get current MF version.\n");
		return 0;
	} else {
		struct in_addr host_ipaddress;
		memcpy(&host_ipaddress.S_un.S_addr,he->h_addr_list[0],4);
		
		int majorVersion = host_ipaddress.S_un.S_un_b.s_b1;
		int buildNumber = (host_ipaddress.S_un.S_un_b.s_b2 * 100) + host_ipaddress.S_un.S_un_b.s_b3;
		int otherNews = host_ipaddress.S_un.S_un_b.s_b4;
		MFD_Out(MFD_SOURCE_CONFIG,"Current Build for R%d: %d (have other news: %d)\n",majorVersion,buildNumber,otherNews);
		MFD_Out(MFD_SOURCE_CONFIG,"and this Build is: R%d %d\n",MAILFILTERVER_MAJOR,MAILFILTERVER_BUILD);
		
		if (
			(majorVersion > MAILFILTERVER_MAJOR) ||
			(buildNumber > MAILFILTERVER_BUILD)
			)
		{
			char* szEmail = (char*)malloc(5000);
			sprintf(szEmail,"Dear PostMaster!\r\n\r\n"
						"A newer version of MailFilter is available from the MailFilter website,\r\n"
						"http://www.mailfilter.cc/ .\r\n"
						"\r\n"
						"Yours,\r\nMailFilter Version %s on %s.\r\n\r\n",
						MAILFILTERVERNUM,
						MF_GlobalConfiguration.ServerName.c_str()
						);

			MF_EMailPostmasterGeneric(
						"New Version Available",szEmail,
						"","");
			free(szEmail);
		}
			
		if ( otherNews != 0)
		{
			char* szEmail = (char*)malloc(5000);
			sprintf(szEmail,"Dear PostMaster!\r\n\r\n"
						"Important News about MailFilter are available on the MailFilter website,\r\n"
						"http://www.mailfilter.cc/ .\r\n"
						"\r\n"
						"Yours,\r\nMailFilter Version %s on %s.\r\n\r\n",
						MAILFILTERVERNUM,
						MF_GlobalConfiguration.ServerName.c_str()
						);

			MF_EMailPostmasterGeneric(
						"Important News Available",szEmail,
						"","");
			free(szEmail);
		}

		return 1;
	}
}

int MF_ParseCommandLine( int argc, char **argv )
{

	int c;
	bool bTArgOkay;
	ix_getopt_init();

	MF_GlobalConfiguration.ApplicationMode = MailFilter_Configuration::SERVER;

	while (1)
	{

		c = ix_getopt(argc, argv, "dh?avxt");
		if (c == -1)
			break;
			
		switch(c)
		{
#ifndef _MF_CLEANBUILD
			case 'x':
				if (argc>ix_optind)
				{
					MFT_Debug = atoi(argv[ix_optind]);
					ix_optind++;
				}
				break;
				
			case 'd':
				MFT_Debug = 0xFFFF;
				break;
			case 'a':
				MFT_AVAVerbose = true;
				break;
#endif
			case 'v':
				MF_DisplayCriticalError("MAILFILTER: Option -v is now obsolete.\n");
				break;
			case '?':
			case 'h':
				MF_DisplayCriticalError("MAILFILTER: Usage:\n"
						"  %s [-d | -x debugflags] [-t app] [ConfigurationPath]\n"
						"  -d debug (with flags = 0xFFFF)\n"
						"  -t args are: \"server\" \"config\" \"restore\" \"nrm\"\n",argv[0]);
				return false;
				break;
			case 't':
				bTArgOkay = false;
				if (argc>ix_optind)
				{
#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)				
					if (strcmp(argv[ix_optind],"server") == 0)
#else
					if (strcasecmp(argv[ix_optind],"server") == 0)
#endif
					{
						MF_GlobalConfiguration.ApplicationMode = MailFilter_Configuration::SERVER;
						bTArgOkay = true;
					}
#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)				
					if (strcmp(argv[ix_optind],"config") == 0)
#else
					if (strcasecmp(argv[ix_optind],"config") == 0)
#endif
					{
						MF_GlobalConfiguration.ApplicationMode = MailFilter_Configuration::CONFIG;
						bTArgOkay = true;
					}
#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)				
					if (strcmp(argv[ix_optind],"restore") == 0)
#else
					if (strcasecmp(argv[ix_optind],"restore") == 0)
#endif
					{
						MF_GlobalConfiguration.ApplicationMode = MailFilter_Configuration::RESTORE;
						bTArgOkay = true;
					}
#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)				
					if (strcmp(argv[ix_optind],"install") == 0)
#else
					if (strcasecmp(argv[ix_optind],"install") == 0)
#endif
					{
						MF_GlobalConfiguration.ApplicationMode = MailFilter_Configuration::INSTALL;
						bTArgOkay = true;
					}
#if defined(N_PLAT_NLM) && !defined(__NOVELL_LIBC__)				
					if (strcmp(argv[ix_optind],"upgrade") == 0)
#else
					if (strcasecmp(argv[ix_optind],"upgrade") == 0)
#endif
					{
						MF_GlobalConfiguration.ApplicationMode = MailFilter_Configuration::UPGRADE;
						bTArgOkay = true;
					}
#if defined(N_PLAT_NLM)
#if !defined(__NOVELL_LIBC__)
					if (strcmp(argv[ix_optind],"nrm") == 0)
#else
					if (strcasecmp(argv[ix_optind],"nrm") == 0)
#endif
					{
						MF_GlobalConfiguration.ApplicationMode = MailFilter_Configuration::NRM;
						bTArgOkay = true;
					}
#endif
					ix_optind++;
				}
				
				if (!bTArgOkay)
				{
					MF_DisplayCriticalError("MAILFILTER: Invalid argument passed to -t.\n");
					return false;
				}
				break;
			default:
				break;
			}
		}

	if (ix_optind < argc)
	{
		MF_DisplayCriticalError("MAILFILTER: Using %s as Configuration Directory.\n",argv[ix_optind]);
		MF_GlobalConfiguration.config_directory = argv[ix_optind];
		
#ifdef IXPLAT_WIN32
	} else {
		char* xdir = GetRegValue(HKEY_LOCAL_MACHINE,"SOFTWARE\\Hofstaedtler IE GmbH\\MailFilter","InstallDir","C:\\Progra~1\\MailFilter");
		MF_DisplayCriticalError("MAILFILTER: Using %s as Configuration Directory.\n",xdir);
		MF_GlobalConfiguration.config_directory = xdir;
	}
#endif
#ifdef IXPLAT_NETWARE
	} else {
		MF_GlobalConfiguration.config_directory = "sys:\\etc\\mailflt";
	}
#endif


	MF_GlobalConfiguration.setDefaults();

	return true;
}


/***** PLATFORM SUPPORT FUNCTIONS *****/

void MF_OutOfMemoryHandler()
{
	MF_DisplayCriticalError("MAILFILTER:	OUT OF MEMORY!\n	MailFilter is out of memory. Operation cancelled.\n");
}

//
//  Makes a Path a valid OS-dependend path
//
void MF_MakeValidPath(char* thePath)
{
	if (thePath == NULL) return;

#if defined(N_PLAT_NLM) && (!defined(__NOVELL_LIBC__))
	char *server;
	char *volume;
	char *dir;

	server = (char*)malloc(_MAX_SERVER);
	volume = (char*)malloc(_MAX_VOLUME);
	dir = (char*)malloc(_MAX_DIR);
	if (server == NULL)	{ MF_OutOfMemoryHandler();	return;	}
	if (volume == NULL)	{ free(volume); MF_OutOfMemoryHandler();	return;	}
	if (dir	 == NULL)	{ free(volume); free(server); MF_OutOfMemoryHandler();	return;	}

	ParsePath ( thePath , server, volume, dir );
	sprintf(thePath,"%s\\%s:%s",server,volume,dir);

	free(dir);
	free(volume);
	free(server);
#else	//  N_PLAT_NLM
#ifdef __NOVELL_LIBC__
    char server[48+1], volume[256+1], directory[512+1],
    filename[256+1], extension[256+1];//, path[1024+1];
    int elements, flags;
      
    deconstruct(thePath, server, volume, directory, filename, extension, &elements, &flags);
    construct(thePath, server, volume, directory, filename, extension, PATH_NETWARE);
#endif	// __NOVELL_LIBC__
#endif	// !N_PLAT_NLM

#ifdef WIN32
	char *fullpath;

	fullpath = (char*)malloc(_MAX_PATH);
	if (fullpath == NULL) { MF_OutOfMemoryHandler();	return; }

	if (_fullpath( fullpath, thePath, _MAX_PATH ) != NULL )
		strncpy(thePath,fullpath,_MAX_PATH);

	free(fullpath);
#endif	// WIN32
}

/***** REAL UTILITY FUNCTIONS *****/

//
//  Generates a unique fileName
//  * fileName: Output Buffer
//  * addVal:   Adds this int to filenumber
//
bool MF_Util_MakeUniqueFileName(char* fileName,int addVal)
{
	unsigned long uniNum;
	char uniNumC[40];

	fileName[0]=0;
	fileName[8]=0;
	fileName[9]=0;

	uniNum = clock()+addVal;

	sprintf(uniNumC,"%08u",uniNum);

	strncpy( fileName , uniNumC+strlen(uniNumC)-8 , 8 );
	strcat( fileName , ".077");

	return true;
}

#ifdef N_PLAT_NLM
extern "C" unsigned long ReturnFileServerName(char *nameBuffer);
#endif

//
//  Get's a servername from a path
//
//	* Pass NULL to sourcePath to get the local servername
//
void MF_GetServerName(char* serverName, unsigned long bufSize)	//, char* sourcePath)
{
#ifdef N_PLAT_NLM
/*
#ifndef __NOVELL_LIBC__
	char *volume;
	char *dir;
	if (serverName == NULL) return;

	volume = (char*)malloc(_MAX_VOLUME);
	dir = (char*)malloc(_MAX_DIR);
	
		ParsePath ( "SYS:" , serverName, volume, dir );		// returns local name
	free(dir);
	free(volume);
#else
	struct utsname u;
	uname(&u);
	strncpy(serverName,u.servername,bufSize);
#endif
*/
	serverName[0] = '\0';
	
	// ReturnFileServerName gets called by both uname and ParsePath
	// so we just call it oureselves and that's fine.
	//
	// from http://developer.novell.com/ndk/doc/samplecode/smscomp_sample/NLM_tsatest/TSATest.h.html
	unsigned int retLen = ReturnFileServerName(serverName);
	serverName[retLen] = '\0';
	strcpy(serverName, serverName + 1);
	
	if (serverName[0] == '\0')
	{
		// damn
		// ReturnFileServerName doesn't always return useful stuff (in autoexec.ncf!)
		// 
		// now try with gethostname
		
		gethostname (serverName,(int)bufSize);
	}	

	if (serverName[0] == '\0')
	{
		MF_DisplayCriticalError("MAILFILTER: ERROR: Could not detect servername.\n");
	}

#endif
#ifdef WIN32
//	unsigned long bufSize = _MAX_SERVER;
	GetComputerName(serverName, &bufSize); 
#endif
}

//
//  Checks if two Path's are on the same server/volume pair.
// 
//  * 0 = no
//  * 1 = yes
//  * -xx = error
//
int MF_CheckPathSameVolume(char* thePath1 , char* thePath2)
{
#pragma unused (thePath1, thePath2)

	int rc = 0;

#if defined(N_PLAT_NLM) && (!defined(__NOVELL_LIBC__))
	char *dir;

	char *server1;
	char *server2;
	char *volume1;
	char *volume2;
	
	if (thePath1 == NULL) return -255;
	if (thePath2 == NULL) return -255;

	dir = (char*)malloc(_MAX_DIR);

	server1 = (char*)malloc(_MAX_SERVER);
	server2 = (char*)malloc(_MAX_SERVER);
	volume1 = (char*)malloc(_MAX_VOLUME);
	volume2 = (char*)malloc(_MAX_VOLUME);

	ParsePath ( thePath1 , server1, volume1, dir );
	ParsePath ( thePath2 , server2, volume2, dir );

	if ((stricmp(server1,server2) == 0) && (stricmp(volume1,volume2) == 0))
		rc = 1;
			
	free(volume2);
	free(volume1);
	free(server2);
	free(server1);

	free(dir);
#else

	rc = 1;

#endif  // WIN32

	return rc;
}

//
//  Checks if two Path's are on the same server
// 
//  * 0 = no
//  * 1 = yes
//  * -xx = error
//
int MF_CheckPathSameServer(char* thePath1 , char* thePath2)
{
#pragma unused (thePath1, thePath2)
	int rc = 0;

#if defined(N_PLAT_NLM) && (!defined(__NOVELL_LIBC__))
	char *dir;

	char *server1;
	char *server2;
	char *volume1;
	char *volume2;
	
	if (thePath1 == NULL) return -255;
	if (thePath2 == NULL) return -255;

	dir = (char*)malloc(_MAX_DIR);

	server1 = (char*)malloc(_MAX_SERVER);
	server2 = (char*)malloc(_MAX_SERVER);
	volume1 = (char*)malloc(_MAX_VOLUME);
	volume2 = (char*)malloc(_MAX_VOLUME);

	ParsePath ( thePath1 , server1, volume1, dir );
	ParsePath ( thePath2 , server2, volume2, dir );

	if (stricmp(server1,server2) == 0)
		rc = 1;
			
	free(volume2);
	free(volume1);
	free(server2);
	free(server1);

	free(dir);
#else

	rc = 1;

#endif

	return rc;
}

bool MFBW_CheckCurrentScheduleState()
{
	time_t now_t;
	struct tm *now;
	const char* p;
	
	int starthour; int startmin;
	int endhour; int endmin;
	int rc=1;
	int thenow;
	int starttime;
	int endtime;
	bool bState = true;

	now_t = time(NULL);
	now = localtime(&now_t);

	thenow = (now->tm_hour)*60 + now->tm_min;

	if (MF_GlobalConfiguration.BWLScheduleTime != "")
	{
		p = MF_GlobalConfiguration.BWLScheduleTime.c_str();
		
		while (rc>0)
		{	
			starthour = 0;		startmin = 0;
			endhour = 0;		endmin = 0;
			rc = sscanf(p,"%02d%*[:]%02d%*[-]%02d%*[:]%02d",&starthour,&startmin,&endhour,&endmin);
			starttime = starthour*60 + startmin;
			endtime = endhour*60 + endmin;
			
			if ( (rc>0) && (thenow > starttime) && (thenow < endtime) )
			{
				bState = false;
				break;
			}
			p=p+12;
		}
	}
	
	return bState;
}

//#include "snip_str.h"
/*
** strrepl: Replace OldStr by NewStr in string Str contained in buffer
** of size BufSiz.
**
** Str should have enough allocated space for the replacement - if not,
** NULL is returned. Str and OldStr/NewStr should not overlap.
**
** The empty string ("") is found at the beginning of every string.
**
** Returns: pointer to first location behind where NewStr was inserted.
** Str if OldStr was not found.
** NULL if replacement would overflow Str's buffer
**
** this is useful for multiple replacements, see example in main() below
** (be careful not to replace the empty string this way !)
**
** NOTE: The name of this funtion violates ANSI/ISO 9899:1990 sec. 7.1.3,
**but this violation seems preferable to either violating sec. 7.13.8
**or coming up with some hideous mixed-case or underscore infested
**naming. Also, many SNIPPETS str---() functions duplicate existing
**functions which are supported by various vendors, so the naming
**violation may be required for portability.
*/
/*#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif*/

    char *strrepl(char *Str, size_t BufSiz, char *OldStr, char *NewStr)


        {
        unsigned int OldLen, NewLen;
        char *p, *q;
        if(NULL == (p = strstr(Str, OldStr)))
        return Str;
        OldLen = strlen(OldStr);
        NewLen = strlen(NewStr);
        if ((strlen(Str) + NewLen - OldLen + 1) > BufSiz)
        return NULL;
        memmove(q = p+NewLen, p+OldLen, strlen(p+OldLen)+1);
        memcpy(p, NewStr, NewLen);
        return q;
    }

/*#if defined(__cplusplus) && __cplusplus
}
#endif*/

#ifdef TEST
int main(int argc, char *argv[])
{
    char buf[200];
    char *Start, *Str;
    size_t BufSiz, BufLeft;
    if(argc < 3)


        {
        puts("Usage: STRREPL old_string new_string [buffer_size]");
        return EXIT_FAILURE;
    }

    if (argc > 3)


        {
        BufSiz = atoi(argv[3]);
        if (200 < BufSiz)
        BufSiz = 200;
    }

    else BufSiz = 20;/* Pretend we have a short buffer*/
    /* Repeat until all occurrences replaced */
    while(NULL != (Start = fgets(buf, 200, stdin)))


        {
        if ('\n' == LAST_CHAR(Start))
        LAST_CHAR(Start) = NUL;/* Strip trailing \n*/
        for (Str = Start, BufLeft = BufSiz; Start != NULL; )


            {
            Start = strrepl(Start, BufLeft, argv[1], argv[2]);
            if (Start == Str)
            break;
            BufLeft -= Start - Str;
            Str = Start;
        }

        
        if (NULL == Start)
        puts("\a*** Modified string will not fit in buffer ***");
        else printf("%s\n", buf);
    }

    return EXIT_SUCCESS;
}

#endif /* TEST */

/*
 *
 *
 *  --- eof ---
 *
 *
 */
