#include "mfnrm.h"
#include <nwcntask.h>

#define _MAILFILTER_MFCONFIG_STORAGE
#define _MAILFILTER_MFCONFIG_DONTWANTFILTERCACHE
#include "../../../Config/MFConfig.h"
#include <ix.h>

extern bool MF_NRM_WantRestore;
extern int NLM_threadCnt;
extern int NLM_mainThreadGroupID;
extern int MF_NRM_InitComplete;
extern char MF_ProductName[];


void MF_ConfigFree(void)
{
	if (MFC_GWIA_ROOT != NULL)					{	free(MFC_GWIA_ROOT);				MFC_GWIA_ROOT				= NULL;	}
	if (MFC_MFLT_ROOT != NULL)					{	free(MFC_MFLT_ROOT);				MFC_MFLT_ROOT				= NULL;	}
}

int MF_NRM_ParseCommandLine( int argc, char **argv )
{

	int c;
	
	MF_NRM_WantRestore = TRUE;

	ix_getopt_init();

	while (1)
	{

		c = ix_getopt(argc, argv, "hr");
		if (c == -1)
			break;
			
		switch(c)
		{
			case 'r':
				MF_NRM_WantRestore = FALSE;
				break;
			case 'h':
				ConsolePrintf ("MFNRM: Usage:\n	[-r] %s ConfigurationDir\n -r: do not allow restore operations\n",argv[0]);
				return FALSE;
				break;
			default:
				break;
			}
		}

	if (ix_optind < argc)
	{
		ConsolePrintf ("MFNRM: Using %s as Configuration Directory.\n",argv[ix_optind]);
		sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"%s\\CONFIG.BIN",argv[ix_optind]);
		sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"%s\\CONFIG.BAK",argv[ix_optind]);
		sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"%s\\CONFIG.ERR",argv[ix_optind]);								
#ifdef IXPLAT_WIN32
	} else {
		char* xdir = GetRegValue(HKEY_LOCAL_MACHINE,"SOFTWARE\\Hofstaedtler IE GmbH\\MailFilter","InstallDir","C:\\Progra~1\\MailFilter");
		ConsolePrintf ("MFNRM: Using %s as Configuration Directory.\n",xdir);
		sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"%s\\CONFIG.BIN",xdir);
		sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"%s\\CONFIG.BAK",xdir);
		sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"%s\\CONFIG.ERR",xdir);								
	}
#endif
#ifdef IXPLAT_NETWARE
	} else {
		sprintf(MAILFILTER_CONFIGURATION_MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.BIN");
		sprintf(MAILFILTER_CONFIGBACKUP__MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.BAK");
		sprintf(MAILFILTER_CONFIGERROR___MAILFILTER,"SYS:\\ETC\\MAILFLT\\CONFIG.ERR");								
	}
#endif

	return TRUE;
}

int MF_ConfigReadString(const char ConfigFile[MAX_PATH], int Entry, char* Value)
{
	FILE* cfgFile;
	size_t dRead;

	Value[0]=0;

	cfgFile = fopen(ConfigFile,"rb");

	if (cfgFile == NULL)
		return -2;

	fseek(cfgFile,Entry,SEEK_SET);
	
	dRead = fread(Value,sizeof(char),MAILFILTER_CONFIGURATION_LENGTH-2,cfgFile);
	
	fclose(cfgFile);
	Value[dRead+1]=0;

	Value[MAX_PATH-2]=0;

	return (int)(strlen(Value));
}

// Reads the configuration ...
int MF_ConfigRead()
{
	int rc;
	struct stat statInfo;
	rc = 0;
	
	MFC_GWIA_ROOT = NULL;
	MFC_MFLT_ROOT = NULL;
	
	if (stat(MAILFILTER_CONFIGURATION_MAILFILTER,&statInfo))
		rc = 99;
	if (rc != 0)	goto MF_ConfigRead_ERR;

	// Init Vars ...
	MFC_GWIA_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	MFC_MFLT_ROOT = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);

	MFC_GWIA_ROOT							[0] = 0;
	MFC_MFLT_ROOT							[0] = 0;

	//
	// Read In Configuration Values
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 1*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_GWIA_ROOT);
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 2*MAILFILTER_CONFIGURATIONFILE_STRING, MFC_MFLT_ROOT);
	
// Error Handling goes here ...
MF_ConfigRead_ERR:
	{
		if ( (rc) && (rc != 500) )
		{
			MF_ConfigFree();

			printf("MFNRM: Error Code %d\n",rc);

			return FALSE;
		}
	}

	// No Errors! Register Cleanup ...
	atexit(MF_ConfigFree);
	return TRUE;

}

