/*+
 +		MFConfig.c++
 +		
 +		MailFilter Configuration C++ Interface Implementation
 +		
 +		Copyright 2001-2004 Christian Hofstädtler
 +		
 +		
 +		
 +		
+*/

#include <string>
#include <iostream>
#include <fstream>
#include <ostream>

#define _MFD_MODULE			"MFCONFIG.C++"
#include "MailFilter.h"
#include "MFConfig.h++"
#include "MFConfig-Filter.h++"
#include "MFConfig-defines.h"

#define MAILFILTER_CONFIGURATION_PATHFILE "sys:\\etc\\mfpath.cfg"

char MFC_ConfigFile[MAX_PATH] = "";
char* MFT_Local_ServerName = NULL;

namespace MailFilter_Configuration {

std::string MF_ConfigReadString2(std::string ConfigFile, const int startByte)
{
	FILE* cfgFile;
	size_t dRead;
	char Value[MAILFILTER_CONFIGURATION_LENGTH+1];

	Value[0]=0;

	cfgFile = fopen(ConfigFile.c_str(),"rb");

	if (cfgFile == NULL)
		return "";

	fseek(cfgFile,startByte,SEEK_SET);
	
	dRead = fread(Value,sizeof(char),MAILFILTER_CONFIGURATION_LENGTH-2,cfgFile);
	
	fclose(cfgFile);
	Value[dRead+1]=0;

	Value[MAX_PATH-2]=0;
	
	return Value;
}
std::string MF_ConfigReadString(std::string ConfigFile, const int Entry)
{
	return MF_ConfigReadString2(ConfigFile,Entry*(MAILFILTER_CONFIGURATION_LENGTH+1));
}

int MF_ConfigReadInt(std::string ConfigFile, const int Entry)
{
	int retVal = 0;

	char readBuf[16];

	FILE* cfgFile;

	readBuf[0]='\0';

	cfgFile = fopen(ConfigFile.c_str(),"rb");

	if (cfgFile == NULL)
		return -2;

	fseek(cfgFile,Entry,SEEK_SET);
	fread(readBuf,sizeof(char),15,cfgFile);
	fclose(cfgFile);

	readBuf[14]=0;

	retVal = atoi(readBuf);

	return retVal;
}

std::string MF_MakeValidPath(std::string thePath)
{
	if (thePath == "") return "";

#if defined(N_PLAT_NLM) && (!defined(__NOVELL_LIBC__))
	std::string ret;
	char *server;
	char *volume;
	char *dir;

	server = (char*)_mfd_malloc(_MAX_SERVER,"ValPath:Server");
	volume = (char*)_mfd_malloc(_MAX_VOLUME,"ValPath:Volume");
	dir = (char*)_mfd_malloc(_MAX_DIR,"ValPath:Dir");
	if (server == NULL)	{ MF_OutOfMemoryHandler();	return thePath;	}
	if (volume == NULL)	{ _mfd_free(volume,"ValPath:Volume"); MF_OutOfMemoryHandler();	return thePath;	}
	if (dir	 == NULL)	{ _mfd_free(volume,"ValPath:Volume"); _mfd_free(server,"ValPath:Server"); MF_OutOfMemoryHandler();	return thePath;	}

	ParsePath ( thePath.c_str() , server, volume, dir );
	ret = server;
	ret += "\\";
	ret += volume;
	ret += ":";
	ret += dir;

	_mfd_free(dir,"ValPath:Dir");
	_mfd_free(volume,"ValPath:Volume");
	_mfd_free(server,"ValPath:Server");
	
	return ret;
#else	//  N_PLAT_NLM
#ifdef __NOVELL_LIBC__
    char server[48+1], volume[256+1], directory[512+1],
    filename[256+1], extension[256+1];//, path[1024+1];
    int elements, flags;
    char ret[MAX_PATH];
      
    deconstruct(thePath.c_str(), server, volume, directory, filename, extension, &elements, &flags);
    construct(ret, server, volume, directory, filename, extension, PATH_NETWARE);
    return ret;
#endif	// __NOVELL_LIBC__
#endif	// !N_PLAT_NLM

#ifdef WIN32
	char *fullpath;
	std::string ret;

	fullpath = (char*)malloc(_MAX_PATH);
	if (fullpath == NULL) { MF_OutOfMemoryHandler();	return ""; }

	if (_fullpath( fullpath, thePath.c_str(), _MAX_PATH ) != NULL )
		ret = fullpath;

	free(fullpath);
	return ret;
#endif	// WIN32

}

int Configuration::setDefaults(std::string directory, std::string domainname)
{
	this->config_build = CurrentConfigVersion;
	if (directory != "") { this->config_directory = directory; }
	if (this->config_directory != "")
	{
		this->config_file = this->config_directory + "\\CONFIG.BIN";
	}
	
	if ( (this->DomainName == "") && (domainname != "") ) { this->DomainName = domainname; }
	if (this->DomainName != "")
	{
		this->DomainEmailPostmaster = "postmaster@" + this->DomainName;
		this->DomainEmailMailFilter = "MailFilter@" + this->DomainName;
		this->DomainHostname = "mail." + this->DomainName;
	}
	
	this->ProblemDirMaxSize = 32000; // 32 MB
	this->ProblemDirMaxAge = 120; // 120 days
	
	this->GWIAVersion = 600; // 6.00 or newer
	
	this->MailscanDirNum = 10;
	this->MailscanTimeout = 180; // 180 seconds
	
	this->NotificationAdminLogs = true;
	this->NotificationAdminMailsKilled = true;
	this->NotificationAdminDailyReport = true;
	
	this->EnableIncomingRcptCheck = false;
	this->EnableAttachmentDecoder = true;
	this->EnablePFAFunctionality = false;
	this->EnableNRMThread = false;
	this->EnableNRMRestore = true;
	
	this->MessageFooter = "";
	this->Multi2One = "";
	this->BWLScheduleTime = "";
	
	return 0;
}

Configuration::Configuration()
{
	this->NRMInitialized = false;
	this->ApplicationMode = SERVER;
	this->config_mode_strict = true;
}

Configuration::~Configuration()
{
}

bool Configuration::ReadFilterListFromConfig()
{
	long startAt;
	std::string myFilterFile;

	// select correct config file
	if (this->config_build >= 11)
	{
		myFilterFile = this->config_directory + "\\filters.bin";
		startAt = 54;
	}
	else
	{
		myFilterFile = this->config_file;
		startAt = 4000;
	}

	return this->ReadFilterList(myFilterFile,startAt);
}

bool Configuration::ReadFilterListFromRulePackage(std::string filterFile)
{
	return this->ReadFilterList(filterFile,(long)52);
}

bool Configuration::ReadFilterList(std::string filterFile, long startAt)
{
	if (this->config_build > 7)
	{

		FILE* fFile;
		long rc;
		char szTemp[1002];
		int curItem=0;
		int curFilter=0;
		int curPos=-1;
		int curChr = 0;
	#ifdef _MAILFILTER_WITH_POSIX_REGEXP_H
		regex_t re;
	#endif
	#ifdef _MAIFILTER_WITH_REGEXP_H
		pcre *re;
	#endif

		if (filterFile == "")
		{
			assert("ReadFilterList called with empty filterList");
		}

		fFile = fopen( filterFile.c_str(),"rb");
		fseek(fFile,startAt+1,SEEK_SET);

		while (!feof(fFile))
		{
			fseek(fFile,-1,SEEK_CUR);
			MailFilter_Configuration::Filter flt;
			
			flt.matchfield = (MailFilter_Configuration::FilterField)(fgetc(fFile));
			flt.notify = (MailFilter_Configuration::Notification)(fgetc(fFile));
			flt.type = (MailFilter_Configuration::FilterType)(fgetc(fFile));
			flt.action = (MailFilter_Configuration::FilterAction)(fgetc(fFile));
			flt.enabled = (bool)(fgetc(fFile));
			flt.enabledIncoming = (bool)(fgetc(fFile));
			flt.enabledOutgoing = (bool)(fgetc(fFile));

			if (this->config_build >= 11)
			{
				// stock filter
				fgetc(fFile);
				// 4 bytes for counter
				fgetc(fFile);
				fgetc(fFile);
				fgetc(fFile);
				fgetc(fFile);
			}

			
			if (filterFile == "")
			{
				if (this->config_build < 9)
				{
					switch (flt.matchfield)
					{
					case MAILFILTER_OLD_MATCHFIELD_SUBJECT:
						flt.matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_SUBJECT;
						break;
					case MAILFILTER_OLD_MATCHFIELD_SIZE:
						flt.matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_SIZE;
						break;
					case MAILFILTER_OLD_MATCHFIELD_EMAIL_FROM:
						flt.matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_FROM;
						break;
					case MAILFILTER_OLD_MATCHFIELD_EMAIL_TO:
						flt.matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_TO;
						break;
					}
				}
				if (this->config_build < 10)
				{
					switch (flt.matchfield)
					{
					case MAILFILTER_MATCHFIELD_EMAIL_TO:
						flt.matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_TOANDCC;
						break;
					case MAILFILTER_MATCHFIELD_EMAIL:
						flt.matchfield = (MailFilter_Configuration::FilterField)MAILFILTER_MATCHFIELD_EMAIL_BOTHANDCC;
						break;
					}
				}
			}
			
			rc = (long)fread(szTemp,sizeof(char),1000,fFile);
			fseek(fFile,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
			
			if (strlen(szTemp) > 0) flt.expression = szTemp;

			rc = (long)fread(szTemp,sizeof(char),1000,fFile);
			fseek(fFile,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

			if (strlen(szTemp) > 0) flt.name = szTemp;
			
			if (flt.expression == "")
			{
				break;
			}
			
			this->filterList.push_back(flt);
			
			fgetc(fFile);
		}

		fclose(fFile);
		
		return true;

	} else {
		// i cannot upgrade this.
		// sorry.
		return false;
	}
}

/*
// this one can read the V7 config file and put it in the current memory config
int MF_Filter_InitListsV7()
{
	FILE* cfgFile;
	int curItem;
	long rc1;
	long rc2;
	char szTemp[1002];

	// version 7

	cfgFile = fopen(MAILFILTER_CONFIGURATION_MAILFILTER,"rb");
	fseek(cfgFile,4000,SEEK_SET);

	for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
	{
		MFC_Filters[curItem].matchfield = (char)fgetc(cfgFile);

		switch (MFC_Filters[curItem].matchfield)
		{
		case MAILFILTER_OLD_MATCHFIELD_SUBJECT:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_SUBJECT;
			break;
		case MAILFILTER_OLD_MATCHFIELD_SIZE:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_SIZE;
			break;
		case MAILFILTER_OLD_MATCHFIELD_EMAIL_FROM:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_FROM;
			break;
		case MAILFILTER_OLD_MATCHFIELD_EMAIL_TO:
			MFC_Filters[curItem].matchfield = MAILFILTER_MATCHFIELD_EMAIL_TOANDCC;
			break;
		}
			
		MFC_Filters[curItem].notify = (char)fgetc(cfgFile);
		MFC_Filters[curItem].type = 0;
		MFC_Filters[curItem].action = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabled = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledIncoming = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledOutgoing = (char)fgetc(cfgFile);
		MFC_Filters[curItem].expression[0] = 0;
		
		rc1 = (long)fread(szTemp,sizeof(char),501,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc1+1,SEEK_CUR);
		
		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].expression[0]=0;		

		rc2 = (long)fread(szTemp,sizeof(char),61,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc2+1,SEEK_CUR);

		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].name[0]=0;
		
		if (MFC_Filters[curItem].expression[0]==0)
			break;
	}

	fclose(cfgFile);
	return 0;
}

// this one can read the V6 config file and put it in the current memory config
int MF_Filter_InitListsV6()
{
	FILE* filterList;
	int curItem=0;
	int curFilter=0;
	int curPos=-1;
	int curChr = 0;

	////////////////////
	//
	//  Begin with the ATTACHMENTS
	//

	filterList = fopen(MAILFILTER_CONFIGURATION_MAILFILTER,"rb");
	fseek(filterList,4000,SEEK_SET);

	if (filterList == NULL)
	{
		MF_DisplayCriticalError("MAILFILTER: Error while opening configuration file.\n");
		return FALSE;
	}

	while( !feof(filterList) )
	{
		if (curItem > MFT_Filter_ListAttachments_MAX-1) break;

		MFC_Filters[curFilter].expression[0]=0;
		fread(MFC_Filters[curFilter].expression,sizeof(char),50,filterList);
		MFC_Filters[curFilter].expression[50-1]=0;
		if (MFC_Filters[curFilter].expression[0]==0)
			break;

		MFC_Filters[curFilter].matchfield = MAILFILTER_MATCHFIELD_ATTACHMENT;
		MFC_Filters[curFilter].name[0] = 0;
		MFC_Filters[curFilter].enabled = true;
		MFC_Filters[curFilter].enabledIncoming = true;
		MFC_Filters[curFilter].enabledOutgoing = true;
		MFC_Filters[curFilter].enabled = (MFC_Filters[curItem].enabledIncoming == true) && (MFC_Filters[curFilter].enabledOutgoing == true);
		MFC_Filters[curFilter].notify = migrateNotify;
		MFC_Filters[curFilter].action = MAILFILTER_MATCHACTION_DROP_MAIL;
				
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curFilter++;
		curItem++;
	}

	//
	//
	////////////////////


	////////////////////
	//
	//  Next, the SUBJECT cache
	//

	fseek(filterList,11550,SEEK_SET);

	while( !feof(filterList) )
	{
		if (curItem > MFT_Filter_ListSubjects_MAX-1) break;

		MFC_Filters[curFilter].expression[0]=0;
		curPos=-1;
		curChr=0;
		fread(MFC_Filters[curFilter].expression,sizeof(char),75,filterList);
		MFC_Filters[curFilter].expression[75-1]=0;
		if (MFC_Filters[curFilter].expression[0]==0)
			break;

		MFC_Filters[curFilter].matchfield = MAILFILTER_MATCHFIELD_SUBJECT;
		MFC_Filters[curFilter].name[0] = 0;
		MFC_Filters[curFilter].enabled = true;
		MFC_Filters[curFilter].enabledIncoming = true;
		MFC_Filters[curFilter].enabledOutgoing = true;
		MFC_Filters[curFilter].enabled = (MFC_Filters[curFilter].enabledIncoming == true) && (MFC_Filters[curFilter].enabledOutgoing == true);
		MFC_Filters[curFilter].notify = migrateNotify;
		MFC_Filters[curFilter].action = MAILFILTER_MATCHACTION_DROP_MAIL;
				
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curFilter++;
	}
	//
	//
	////////////////////

	////////////////////
	//
	//  Next, the SENDER cache
	//
	fseek(filterList,87000,SEEK_SET);

	while( !feof(filterList) )
	{
		if (curItem > MFT_Filter_ListSenders_MAX-1) break;

		MFC_Filters[curFilter].expression[0]=0;

		curPos=-1;
		curChr=0;

		fread(MFC_Filters[curFilter].expression,sizeof(char),50,filterList);
		MFC_Filters[curFilter].expression[50-1]=0;

		if (MFC_Filters[curFilter].expression[0]==0)
			break;

		MFC_Filters[curFilter].matchfield = MAILFILTER_MATCHFIELD_EMAIL;
		MFC_Filters[curFilter].name[0] = 0;
		MFC_Filters[curFilter].enabledIncoming = true;
		MFC_Filters[curFilter].enabledOutgoing = true;
		MFC_Filters[curFilter].enabled = (MFC_Filters[curFilter].enabledIncoming == true) && (MFC_Filters[curFilter].enabledOutgoing == true);
		MFC_Filters[curFilter].notify = migrateNotify;
		MFC_Filters[curFilter].action = MAILFILTER_MATCHACTION_DROP_MAIL;
				
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		curFilter++;
	}

	//
	//
	////////////////////

	// ** MODIFY LISTS **

	////////////////////
	//
	//  Next, the FROM cache
	//
	curItem = 0;
	
	fseek(filterList,95000,SEEK_SET);

		fread(MFC_Multi2One,sizeof(char),50,filterList);
		MFC_Multi2One[49]=0;
	//
	////////////////////
	
	fclose(filterList);
	
	return TRUE;

}
*/

bool Configuration::CreateFromInstallFile(std::string installFile)
{
	std::string line;
	unsigned int pos;
	std::string param;
	std::string value;
	
	std::string myLicenseKey;
	std::string myHomeGwia;
	std::string myHomeMailFilter;
	std::string myDomain;
	std::string myHostname;
	unsigned int myGwiaVersion;
	std::string myConfDir;
	std::string filterFilePath;
	bool isUpgrade = false;

	std::ifstream install(installFile.c_str());
	
	printf("   Installation data file: %s\n",installFile.c_str());

	if (!install.good())
		MF_DisplayCriticalError(std::string("   MailFilter Installation: Could not open "+installFile+"!\n").c_str());
	
	while (install >> line)
	{
		if (line[0] == '/')
		{
			pos = line.find("=");
			if (pos != -1)
			{
				param = line.substr(1,pos-1);
				value = line.substr(pos+1);
#ifdef _TRACE
				printf("     config: '%s'='%s'\n",param.c_str(),value.c_str());
#endif
				
				if (param == "licensekey")
					myLicenseKey = value;

				if (param == "config-mode")
				{
				if (value == "upgrade")
					isUpgrade = true;
					else
					isUpgrade = false;
				}

				if (param == "home-gwia")
					myHomeGwia = value;
				if (param == "home-mailfilter")
					myHomeMailFilter = value;

				if (param == "domain")
					myDomain = value;
				if (param == "hostname")
					myHostname = value;

				if (param == "gwia-version")
					myGwiaVersion = (unsigned int)atoi(value.c_str());

				if (param == "config-directory")
					myConfDir = value;

				if (param == "config-importfilterfile")
					filterFilePath = value;

			}
		}
	}
	printf("   Done reading install data file!\n");
	install.close();

	if (!isUpgrade)
	{
		this->config_build = CurrentConfigVersion;

		this->filterList.clear();
		
		this->setDefaults(myConfDir,myDomain);
		if (myHostname != "")
			this->DomainHostname = myHostname;

		if (myHomeGwia != "")
			this->GWIARoot = myHomeGwia;
		if (myHomeMailFilter != "")
			this->MFLTRoot = myHomeMailFilter;

		if (myLicenseKey != "")
			this->LicenseKey = myLicenseKey;
			
		if (myGwiaVersion != 0)
		{
			if (!((myGwiaVersion == 550) || (myGwiaVersion == 600)))
				myGwiaVersion = 600;
			this->GWIAVersion = myGwiaVersion;
		}
		if (filterFilePath != "")
		{
			printf("   Importing rule package %s\n",filterFilePath.c_str());
			if (!this->ReadFilterListFromRulePackage(filterFilePath))
				printf("      ... FAILED.\n");
		}
	} else {
		this->ReadFromFile("");
	}
	
	printf("   Writing "MAILFILTER_CONFIGURATION_PATHFILE"... \n");
	std::ofstream mailfilterPathFile(MAILFILTER_CONFIGURATION_PATHFILE);
	if (mailfilterPathFile.good())
	{
		mailfilterPathFile << this->config_directory;
		mailfilterPathFile.close();
	} else {
		printf("      ... FAILED.\n");
		MF_DisplayCriticalError("MailFilter Installation: Could not write "MAILFILTER_CONFIGURATION_PATHFILE"!\n");
	}
		
	return true;
}

static bool mkBoolFromStr(std::string checkStr)
{
	if (checkStr.size() == 0)
		return false;

	if (checkStr.at(0) == '0')
		return false;

	if (checkStr.at(0) == 'n')
		return false;
		
	if (checkStr.at(0) == 'N')
		return false;
		
	if (checkStr.at(0) == '1')
		return true;

	if (checkStr.at(0) == 'y')
		return true;
		
	if (checkStr.at(0) == 'Y')
		return true;
		
	return false;
}

bool Configuration::ReadFromFile(std::string alternateFilename)
{
	int rc = 0;
	bool bOldStyleFile = false;
	unsigned long iServerNameSize = 512;
	char* szServerName;
	
	std::string pConfigFile = (alternateFilename == "" ? this->config_file : alternateFilename);

	MFD_Out(MFD_SOURCE_CONFIG,"MFC: Starting with %s.\n",pConfigFile.c_str());


	struct stat statInfo;
	std::string ConfigVersion = "";
	
	if (stat(pConfigFile.c_str(),&statInfo))
	{
		rc = 98;
	} else {

		// determine configuration file version
		ConfigVersion = MF_ConfigReadString2(pConfigFile, 0);

		if (strncmp(ConfigVersion.c_str(),MAILFILTER_CONFIGURATION_BASESIGNATURE,sizeof(MAILFILTER_CONFIGURATION_BASESIGNATURE)-1) != 0)
		{
			// hm, new style?
			#define _MF_NEWSTYLE_SIG	"# MailFilter Configuration File"
			if (strncmp(ConfigVersion.c_str(),_MF_NEWSTYLE_SIG,sizeof(_MF_NEWSTYLE_SIG)-1) != 0)
			{
				// No! error out...
				rc = 99;
			}
		} else {
			// old style config. have to upgrade it...
			bOldStyleFile = true;
			this->config_build = (unsigned int)atoi(ConfigVersion.c_str()+sizeof(MAILFILTER_CONFIGURATION_BASESIGNATURE));
			
			if (this->config_mode_strict)
			{
				// so then, error out.
				rc = 99;
			}
		}

	}
		
	// get servername
	szServerName = (char*)_mfd_malloc(iServerNameSize,"Config:Server");
	MF_GetServerName ( szServerName , iServerNameSize);
	this->ServerName = szServerName;
	_mfd_free( szServerName , "Config:Server" );
	
	// compatiblity for MFAX Licensing
	if (MFT_Local_ServerName == NULL)
	{
		MFT_Local_ServerName = (char*)_mfd_malloc(_MAX_SERVER,"Config:LocalServer");
	}
	MF_GetServerName ( MFT_Local_ServerName, _MAX_SERVER );


	if (!rc)
	{
		if (!bOldStyleFile)
		{
			// new file format.

			std::string line;
			unsigned int pos;
			std::string param;
			std::string value;
			
			std::ifstream cfgFile(pConfigFile.c_str());
			
			if (!cfgFile.good())
			{
				rc = 98;
			} else {
			
				while (cfgFile >> line)
				{
					if (line.at(0) == '/')
					{
						pos = line.find("=");
						if (pos != -1)
						{
							param = line.substr(1,pos-1);
							if (line.size() > pos)
								value = line.substr(pos+1);
							else
								value = "";
							
		#ifdef _TRACE
							MF_DisplayCriticalError("     config: '%s'='%s'\n",param.c_str(),value.c_str());
		#endif
							
							if (param == "licensekey")
								this->LicenseKey = value;

							// Strings
							if (param == "home-gwia")
								this->GWIARoot = value;
							if (param == "home-mailfilter")
								this->MFLTRoot = value;

							if (param == "login-username")
								this->LoginUserName = value;
							if (param == "login-password")
								this->LoginUserPassword = value;

							if (param == "domain")
								this->DomainName = value;
							if (param == "hostname")
								this->DomainHostname = value;
							if (param == "email-mailfilter")
								this->DomainEmailMailFilter = value;
							if (param == "email-postmaster")
								this->DomainEmailPostmaster = value;

							if (param == "multi2one")
								this->Multi2One = value;

							if (param == "schedule-time")
								this->BWLScheduleTime = value;

							// Numbers
							if (param == "version-config")
								this->config_build = (unsigned int)atoi(value.c_str());
							
							if (param == "version-gwia")
								this->GWIAVersion = (unsigned int)atoi(value.c_str());

							if (param == "scandirs")
								this->MailscanDirNum = (unsigned int)atoi(value.c_str());

							if (param == "scantimeout")
								this->MailscanTimeout = (unsigned int)atoi(value.c_str());

							if (param == "problemdir-maxage")
								this->ProblemDirMaxAge = (unsigned int)atoi(value.c_str());

							if (param == "problemdir-maxsize")
								this->ProblemDirMaxSize = (unsigned int)atoi(value.c_str());

							// enums. damn concept.
							if (param == "defaultnotify-internalsender")
								this->DefaultNotification_InternalSender = (MailFilter_Configuration::Notification)atoi(value.c_str());

							if (param == "defaultnotify-internalrecipient")
								this->DefaultNotification_InternalRecipient = (MailFilter_Configuration::Notification)atoi(value.c_str());

							if (param == "defaultnotify-externalsender")
								this->DefaultNotification_ExternalSender = (MailFilter_Configuration::Notification)atoi(value.c_str());

							if (param == "defaultnotify-externalrecipient")
								this->DefaultNotification_ExternalRecipient = (MailFilter_Configuration::Notification)atoi(value.c_str());

							if (param == "defaultnotify-adminincoming")
								this->DefaultNotification_AdminIncoming = (MailFilter_Configuration::Notification)atoi(value.c_str());

							if (param == "defaultnotify-adminoutgoing")
								this->DefaultNotification_AdminOutgoing = (MailFilter_Configuration::Notification)atoi(value.c_str());

							// bools
							if (param == "sendadmin-logs")
								this->NotificationAdminLogs = mkBoolFromStr(value);

							if (param == "sendadmin-cleanup")
								this->NotificationAdminMailsKilled = mkBoolFromStr(value);

							if (param == "sendadmin-dailyreport")
								this->NotificationAdminDailyReport = mkBoolFromStr(value);

							if (param == "check-incomingrecipients")
								this->EnableIncomingRcptCheck = mkBoolFromStr(value);

							if (param == "scan-attachments")
								this->EnableAttachmentDecoder = mkBoolFromStr(value);

							if (param == "pfa")
								this->EnablePFAFunctionality = mkBoolFromStr(value);

							if (param == "nrm-enableinprocess")
								this->EnableNRMThread = mkBoolFromStr(value);

							if (param == "nrm-enablerestore")
								this->EnableNRMRestore = mkBoolFromStr(value);
						}
					}
				}
				cfgFile.close();
			}
		} else {
		
	 		int iIntegerBase = 3500;
	 		this->LoginUserName = "";
	 		this->LoginUserPassword = "";
	 		
			// Control Password
			this->ControlPassword = MF_ConfigReadString2(pConfigFile, 60);
			// License Key || Licensing reads this itself.
			this->LicenseKey = MF_ConfigReadString2(pConfigFile, 240);

	 		this->GWIARoot = MF_MakeValidPath(MF_ConfigReadString(pConfigFile, 1));
	 		this->MFLTRoot = MF_MakeValidPath(MF_ConfigReadString(pConfigFile, 2));
	 		this->DomainName = MF_ConfigReadString(pConfigFile, 3);
	 		this->DomainEmailMailFilter = MF_ConfigReadString(pConfigFile, 4);
	 		this->DomainEmailPostmaster = MF_ConfigReadString(pConfigFile, 5);
	 		this->DomainHostname = MF_ConfigReadString(pConfigFile, 6);
	 		this->Multi2One = MF_ConfigReadString(pConfigFile, 7);
	 		this->BWLScheduleTime = MF_ConfigReadString(pConfigFile, 8);
			
			this->DefaultNotification_InternalSender = (MailFilter_Configuration::Notification)
				MF_ConfigReadInt(pConfigFile, iIntegerBase);

			this->MailscanDirNum = (unsigned int)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+2);

			this->MailscanTimeout = (unsigned int)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+6);

			this->DefaultNotification_InternalRecipient = (MailFilter_Configuration::Notification)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+10);

			this->NotificationAdminLogs = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+12);

			this->NotificationAdminMailsKilled = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+14);

			this->ProblemDirMaxAge = (unsigned int)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+16);

			this->ProblemDirMaxSize = (unsigned int)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+21);

			this->NotificationAdminDailyReport = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+29);

			this->DefaultNotification_ExternalSender = (MailFilter_Configuration::Notification)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+31);

			this->DefaultNotification_ExternalRecipient = (MailFilter_Configuration::Notification)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+33);

			this->EnableIncomingRcptCheck = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+39);
				
			this->EnableAttachmentDecoder = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+41);
				
			this->EnablePFAFunctionality = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+43);

			this->EnableNRMThread = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+45);

			this->EnableNRMRestore = (bool)
				MF_ConfigReadInt(pConfigFile, iIntegerBase+47);

			// Next start at 49
		}
		

		// fixups and stuff.
		if (this->config_mode_strict)
		{	
			if (!rc) if (this->GWIARoot == "") 			{	rc = 303;	}
			if (!rc) if (this->MFLTRoot == "") 			{	rc = 304;	}

			// Check for existence of directories ...
			if (!rc) if (chdir(this->MFLTRoot.c_str()))	{	rc=301;	}
			if (!rc) if (chdir(this->GWIARoot.c_str()))	{	rc=302;	}

		}
		
		if (this->MailscanDirNum < 10) this->MailscanDirNum = 10;

		if (!rc)
		{
			this->LogDirectory = this->MFLTRoot + IX_DIRECTORY_SEPARATOR_STR + "MFLOG" + IX_DIRECTORY_SEPARATOR_STR;
			if (chdir(this->LogDirectory.c_str()))		mkdir(this->LogDirectory.c_str(),S_IRWXU);
		
			/* read in the optional footer message */
			/*	strncpy(szTemp,MFC_ConfigDirectory,MAX_PATH);
			strncat(szTemp,"MSGFOOT.MFT",MAX_PATH);

			MFC_Message_FooterText = (char*)malloc(4000);
			MF_ConfigReadFile(szTemp,MFC_Message_FooterText,3999);
			*/	


			this->filterList.clear();

			// Initialize FilterList Cache
			if ( this->ReadFilterListFromConfig() == false ) {	rc = 299;	}
		}
	}

	// Error Handling goes here ...
	if ( rc > 0 )
	{
		MF_DisplayCriticalError(MF_Msg(MSG_CONFIG_ERRORINFO),rc);

		if (rc < 100)
		{
			MF_DisplayCriticalErrorId(CONMSG_CONFIG_ERRCONFIGFORMAT);
		} else {
			if (rc < 200)
			{
				MF_DisplayCriticalErrorId(CONMSG_CONFIG_ERRCONFIGURATION);
			} else {
				if ( rc < 300 )
				{
					MF_DisplayCriticalErrorId(CONMSG_CONFIG_ERRFILTERLIST);
				} else {
					if ( rc < 350 )
					{
						MF_DisplayCriticalErrorId(CONMSG_CONFIG_ERRDIRECTORIES);
					} else {
							MF_DisplayCriticalErrorId(CONMSG_CONFIG_ERRGENERIC);
					}
				}
			}
		}
		
		return false;
	}

	// done! success!
	return true;
}

// Helper class to print a filter record to the config file
class PrintFilter{
	FILE* m_fFile;
	public:
		explicit PrintFilter(FILE* fFile) { m_fFile = fFile; }
		
		void operator() (const MailFilter_Configuration::Filter& filter) const
		{
			fputc(filter.matchfield,m_fFile);
			fputc(filter.notify,m_fFile);
			fputc(filter.type,m_fFile);		// new in v8
			fputc(filter.action,m_fFile);
			fputc(filter.enabled,m_fFile);
			fputc(filter.enabledIncoming,m_fFile);
			fputc(filter.enabledOutgoing,m_fFile);
		
			// v11+
				// stock filter
				fputc(0,m_fFile);
				// 4 bytes for counter
				fputc(0,m_fFile);
				fputc(0,m_fFile);
				fputc(0,m_fFile);
				fputc(0,m_fFile);
			// ////
		
			fprintf(m_fFile,"%s",filter.expression.c_str());
			fputc(0,m_fFile);
			fprintf(m_fFile,"%s",filter.name.c_str());
			fputc(0,m_fFile);
		}
};

bool Configuration::WriteFilterList(std::string filterList)
{
	FILE* cfgFile = fopen(filterList.c_str(),"wb");
	if (cfgFile == NULL)
		return false;

	fprintf(cfgFile,"MAILFILTER_FILTER_EXCHANGEFILE");
	fputc(0,cfgFile);
	fprintf(cfgFile,"%s%03i",MAILFILTER_CONFIGURATION_BASESIGNATURE,MailFilter_Configuration::CurrentConfigVersion);
	fputc(0,cfgFile);
	fputc(0,cfgFile);
	fputc(0,cfgFile);
	fputc(0,cfgFile);

	for_each(this->filterList.begin(),this->filterList.end(),PrintFilter(cfgFile));

	fclose(cfgFile);

	return true;
}

bool Configuration::WriteToFile(std::string alternateFilename)
{
	int rc = 0;
	
	// place definitions here!
	std::string ConfigVersion;

	std::string pConfigFile = (alternateFilename == "" ? this->config_file : alternateFilename);
	std::string pConfigFileBackup = pConfigFile + ".BAK";

	MFD_Out(MFD_SOURCE_CONFIG,"MFC: Saving to %s.\n",pConfigFile.c_str());

	// make backup of old conf
	unlink(pConfigFileBackup.c_str());
	rename(pConfigFile.c_str(),pConfigFileBackup.c_str());

	// write out new config
	FILE* cfgFile = fopen(pConfigFile.c_str(),"wt");
	if (cfgFile == NULL)
		return false;
		
	fprintf(cfgFile,"# MailFilter Configuration File\n");
	fprintf(cfgFile,"# Do NOT MODIFY this file MANUALLY. It will NOT WORK.\n");
	
	
//	doNull(60);
//	fprintf(cfgFile,"%s",this->ControlPassword.c_str());


	// "Signature"
	fprintf(cfgFile,"/version-config=%i\n",MailFilter_Configuration::CurrentConfigVersion);

	// Values
	fprintf(cfgFile,"/version-gwia=%d\n", this->GWIAVersion);
	fprintf(cfgFile,"/licensekey=%s\n", this->LicenseKey.c_str());
	fprintf(cfgFile,"/home-gwia=%s\n",this->GWIARoot.c_str());
	fprintf(cfgFile,"/home-mailfilter=%s\n",this->MFLTRoot.c_str());
	fprintf(cfgFile,"/login-username=%s\n",this->LoginUserName.c_str());
	fprintf(cfgFile,"/login-password=%s\n",this->LoginUserPassword.c_str());
	fprintf(cfgFile,"/domain=%s\n",this->DomainName.c_str());
	fprintf(cfgFile,"/email-mailfilter=%s\n",this->DomainEmailMailFilter.c_str());
	fprintf(cfgFile,"/email-postmaster=%s\n",this->DomainEmailPostmaster.c_str());
	fprintf(cfgFile,"/hostname=%s\n",this->DomainHostname.c_str());
	fprintf(cfgFile,"/multi2one=%s\n",this->Multi2One.c_str());
	fprintf(cfgFile,"/schedule-time=%s\n",this->BWLScheduleTime.c_str());
	
	fprintf(cfgFile,"/defaultnotify-internalsender=%s\n",this->DefaultNotification_InternalSender == 0 ? "0" : "1");
	fprintf(cfgFile,"/defaultnotify-internalrecipient=%s\n",this->DefaultNotification_InternalRecipient == 0 ? "0" : "1");
	fprintf(cfgFile,"/defaultnotify-externalsender=%s\n",this->DefaultNotification_ExternalSender == 0 ? "0" : "1");
	fprintf(cfgFile,"/defaultnotify-externalrecipient=%s\n",this->DefaultNotification_ExternalRecipient == 0 ? "0" : "1");

	fprintf(cfgFile,"/scandirs=%d\n", this->MailscanDirNum);
	fprintf(cfgFile,"/scantimeout=%d\n", this->MailscanTimeout);
	
	fprintf(cfgFile,"/sendadmin-logs=%s\n",this->NotificationAdminLogs == 0 ? "0" : "1");
	fprintf(cfgFile,"/sendadmin-cleanup=%s\n",this->NotificationAdminMailsKilled == 0 ? "0" : "1");
	fprintf(cfgFile,"/sendadmin-dailyreport=%s\n", this->NotificationAdminDailyReport == 0 ? "0" : "1");

	fprintf(cfgFile,"/problemdir-maxage=%d\n", this->ProblemDirMaxAge);
	fprintf(cfgFile,"/problemdir-maxsize=%d\n", this->ProblemDirMaxSize);

	fprintf(cfgFile,"/check-incomingrecipients=%s\n",this->EnableIncomingRcptCheck == 0 ? "0" : "1");
	fprintf(cfgFile,"/scan-attachments=%s\n",this->EnableAttachmentDecoder == 0 ? "0" : "1");
	fprintf(cfgFile,"/pfa=%s\n",this->EnablePFAFunctionality == 0 ? "0" : "1");
	fprintf(cfgFile,"/nrm-enableinprocess=%s\n",this->EnableNRMThread == 0 ? "0" : "1");
	fprintf(cfgFile,"/nrm-enablerestore=%s\n",this->EnableNRMRestore == 0 ? "0" : "1");

	fclose(cfgFile);

	if (!this->WriteFilterList(this->config_directory + "\\FILTERS.BIN"))
		return false;
	
	return true;
	
}


} // namespace

/*
 *
 *
 *  --- eof ---
 *
 *
 */

