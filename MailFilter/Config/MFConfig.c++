/*+
 +		MFConfig.c++
 +		
 +		MailFilter/ax Configuration C++ Interface Implementation
 +		
 +		Copyright 2001-2003 Christian Hofstädtler
 +		
 +		
 +		- Sep/2003 ; ch   ; Newly created from the C version
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
	if (fullpath == NULL) { MF_OutOfMemoryHandler();	return; }

	if (_fullpath( fullpath, thePath, _MAX_PATH ) != NULL )
		ret = fullpath;

	free(fullpath);
	return ret;
#endif	// WIN32

}

int Configuration::setDefaults(std::string directory, std::string domainname)
{
	this->config_build = MAILFILTER_CONFIGURATION_THISBUILD;
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
	return this->ReadFilterList("",4001);
}

bool Configuration::ReadFilterListFromRulePackage(std::string filterFile)
{
	return this->ReadFilterList(filterFile,(long)54);
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


		fFile = fopen( filterFile == "" ? this->config_file.c_str() : filterFile.c_str(),"rb");
		fseek(fFile,startAt,SEEK_SET);

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
		ConsolePrintf("MAILFILTER: Error while opening configuration file.\n");
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
		fprintf(stderr,std::string("   MailFilter Installation: Could not open "+installFile+"!\n").c_str());
	
	while (install >> line)
	{
		if (line[0] == '/')
		{
			pos = line.find("=");
			if (pos != -1)
			{
				param = line.substr(1,pos-1);
				value = line.substr(pos+1);
				printf("     config: '%s'='%s'\n",param.c_str(),value.c_str());
				
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
		this->config_build = MAILFILTER_CONFIGURATION_THISBUILD;

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
		fprintf(stderr,"MailFilter Installation: Could not write "MAILFILTER_CONFIGURATION_PATHFILE"!\n");
	}
		
	return true;
}

bool Configuration::ReadFromFile(std::string alternateFilename)
{
	int rc = 0;
	struct stat statInfo;
	
/*	try {
	
		const char* pConfigFile = (alternateFilename == "" ? config_file.c_str() : alternateFilename.c_str());
		MFD_Out(MFD_SOURCE_CONFIG,"MFC: Starting with %s.\n",pConfigFile);

		std::string fileversion;
		std::ifstream filestream(pConfigFile, std::fstream::in); // | std::fstream::binary);
		
		filestream >> fileversion;
		std::cout << "checking version..." << std::endl << fileversion << std::endl  << "..." << std::endl;
		
		filestream >> this->GWIARoot;
		std::cout << "gwia root: " << this->GWIARoot << std::endl;

		filestream.close();
		

	} catch (std::exception e)
	{
//	 	NetwareAlert(
		consoleprintf("MAILFILTER: Uncaught Exception in the configuration reader.\n\n");
	}
*/


	///////////////////////////
	//
	// --- end of test area ---
	//
	///////////////////////////
	// place definitions here!

	std::string pConfigFile = (alternateFilename == "" ? this->config_file : alternateFilename);

	consoleprintf("MAILFILTER: Configuration: %s\n",this->config_directory.c_str());
	MFD_Out(MFD_SOURCE_CONFIG,"MFC: Starting with %s.\n",pConfigFile.c_str());

	// compatiblity for MF Licensing Stages
	strncpy(MFC_ConfigFile,pConfigFile.c_str(),MAX_PATH-1);


	if (stat(pConfigFile.c_str(),&statInfo))
		rc = 98;
	if (rc != 0)	goto MF_ConfigRead_ERR;

	if (this->config_mode_strict)
	{
		if (statInfo.st_size < 3999)
			rc = 97;	
		if (rc != 0)	goto MF_ConfigRead_ERR;
	}
			
	std::string ConfigVersion;
	ConfigVersion = MF_ConfigReadString(pConfigFile, 0);
	if (this->config_mode_strict)
	{
		if (ConfigVersion != MAILFILTER_CONFIGURATION_SIGNATURE)
		{
			rc = 99;
			goto MF_ConfigRead_ERR;	
		}
	} else {
		if (strncmp(ConfigVersion.c_str(),MAILFILTER_CONFIGURATION_BASESIGNATURE,sizeof(MAILFILTER_CONFIGURATION_BASESIGNATURE)-1) != 0)
		{
			// eek
			rc = 99;
			goto MF_ConfigRead_ERR;	
		} else {
			this->config_build = (unsigned int)atoi(ConfigVersion.c_str()+sizeof(MAILFILTER_CONFIGURATION_BASESIGNATURE));
		}
	}
	
	// get servername
	unsigned long iServerNameSize = 512;
	char* szServerName = (char*)_mfd_malloc(iServerNameSize,"Config:Server");
	
	MF_GetServerName ( szServerName , iServerNameSize);
	this->ServerName = szServerName;
	
	_mfd_free( szServerName , "Config:Server" );
	
	// compatiblity for MFAX Licensing
	if (MFT_Local_ServerName == NULL)
	{
		MFT_Local_ServerName = (char*)_mfd_malloc(_MAX_SERVER,"Config:LocalServer");
	}
	MF_GetServerName ( MFT_Local_ServerName, _MAX_SERVER );

	// Control Password
	this->ControlPassword = MF_ConfigReadString2(pConfigFile, 60);
	
	// License Key || Licensing reads this itself.
	this->LicenseKey = MF_ConfigReadString2(pConfigFile, 240);


	this->GWIARoot = MF_MakeValidPath(MF_ConfigReadString(pConfigFile, 1));
	if (this->config_mode_strict && (this->GWIARoot == "")) {	rc = 303;	goto MF_ConfigRead_ERR;	}

	this->MFLTRoot = MF_MakeValidPath(MF_ConfigReadString(pConfigFile, 2));
	if (this->config_mode_strict && (this->MFLTRoot == "")) {	rc = 304;	goto MF_ConfigRead_ERR;	}

/*#ifndef WIN32
	MFT_InOutSameServerVolume = (MF_CheckPathSameVolume( MFC_GWIA_ROOT , MFC_MFLT_ROOT ) == 1);
	MFT_RemoteDirectories = (!MF_CheckPathSameServer( MFC_GWIA_ROOT , "SYS:SYSTEM" ));
	if (!MFT_RemoteDirectories) MFT_RemoteDirectories = (!MF_CheckPathSameServer( MFC_MFLT_ROOT , "SYS:SYSTEM" ));
#else
	MFT_InOutSameServerVolume = false;
#endif
*/
	this->LogDirectory = this->MFLTRoot + IX_DIRECTORY_SEPARATOR_STR + "MFLOG" + IX_DIRECTORY_SEPARATOR_STR; //,this->LogDirectory);
	
//	consoleprintf("Logging to: %s\n",this->LogDirectory.c_str());

	this->DomainName = MF_ConfigReadString(pConfigFile, 3);

	this->DomainEmailMailFilter = MF_ConfigReadString(pConfigFile, 4);

	this->DomainEmailPostmaster = MF_ConfigReadString(pConfigFile, 5);

	this->DomainHostname = MF_ConfigReadString(pConfigFile, 6);

	this->Multi2One = MF_ConfigReadString(pConfigFile, 7);

	this->BWLScheduleTime = MF_ConfigReadString(pConfigFile, 8);
	
	this->DefaultNotification_InternalSender = (MailFilter_Configuration::Notification)
		MF_ConfigReadInt(pConfigFile, 3500);

	this->MailscanDirNum = (unsigned int)
		MF_ConfigReadInt(pConfigFile, 3502);

	this->MailscanTimeout = (unsigned int)
		MF_ConfigReadInt(pConfigFile, 3506);

	this->DefaultNotification_InternalRecipient = (MailFilter_Configuration::Notification)
		MF_ConfigReadInt(pConfigFile, 3510);

	this->NotificationAdminLogs = (bool)
		MF_ConfigReadInt(pConfigFile, 3512);

	this->NotificationAdminMailsKilled = (bool)
		MF_ConfigReadInt(pConfigFile, 3514);

	this->ProblemDirMaxAge = (unsigned int)
		MF_ConfigReadInt(pConfigFile, 3516);

	this->ProblemDirMaxSize = (unsigned int)
		MF_ConfigReadInt(pConfigFile, 3521);

	this->NotificationAdminDailyReport = (bool)
		MF_ConfigReadInt(pConfigFile, 3529);

	this->DefaultNotification_ExternalSender = (MailFilter_Configuration::Notification)
		MF_ConfigReadInt(pConfigFile, 3531);

	this->DefaultNotification_ExternalRecipient = (MailFilter_Configuration::Notification)
		MF_ConfigReadInt(pConfigFile, 3533);

	this->GWIAVersion = (unsigned int)
		MF_ConfigReadInt(pConfigFile, 3535);

	this->EnableIncomingRcptCheck = (bool)
		MF_ConfigReadInt(pConfigFile, 3539);
		
	this->EnableAttachmentDecoder = (bool)
		MF_ConfigReadInt(pConfigFile, 3541);
		
	this->EnablePFAFunctionality = (bool)
		MF_ConfigReadInt(pConfigFile, 3543);

	this->EnableNRMThread = (bool)
		MF_ConfigReadInt(pConfigFile, 3545);

	this->EnableNRMRestore = (bool)
		MF_ConfigReadInt(pConfigFile, 3547);

	// Next start at 3549
	
	// require at least 10 dirs
	if (this->MailscanDirNum < 10) this->MailscanDirNum = 10;

	/* read in the optional footer message */
/*	strncpy(szTemp,MFC_ConfigDirectory,MAX_PATH);
	strncat(szTemp,"MSGFOOT.MFT",MAX_PATH);
	
	MFC_Message_FooterText = (char*)malloc(4000);
	MF_ConfigReadFile(szTemp,MFC_Message_FooterText,3999);
*/	
	if (this->config_mode_strict && (!MFT_RemoteDirectories))
	{
		// Check for existence of directories ...
		if (chdir(this->MFLTRoot.c_str())) {	rc=301; goto MF_ConfigRead_ERR;	}
		if (chdir(this->GWIARoot.c_str())) {	rc=302; goto MF_ConfigRead_ERR;	}

		if (chdir(this->LogDirectory.c_str()))				mkdir(this->LogDirectory.c_str(),S_IRWXU);
	}

	this->filterList.clear();

	// Initialize FilterList Cache
	if ( this->ReadFilterListFromConfig() == false ) {	rc = 299;	goto MF_ConfigRead_ERR;	}

// Error Handling goes here ...
MF_ConfigRead_ERR:
	if ( rc > 0 )
	{
#ifdef N_PLAT_NLM
		printf(MF_Msg(MSG_CONFIG_ERRORINFO),rc);
#endif

		if (rc < 100)
		{
			MF_DisplayCriticalError(CONMSG_CONFIG_ERRCONFIGFORMAT);
		} else {
			if (rc < 200)
			{
				MF_DisplayCriticalError(CONMSG_CONFIG_ERRCONFIGURATION);
			} else {
				if ( rc < 300 )
				{
					MF_DisplayCriticalError(CONMSG_CONFIG_ERRFILTERLIST);
				} else {
					if ( rc < 350 )
					{
						MF_DisplayCriticalError(CONMSG_CONFIG_ERRDIRECTORIES);
					} else {
							MF_DisplayCriticalError(CONMSG_CONFIG_ERRGENERIC);
					}
				}
			}
		}
		
		return false;
	}

	// No Errors! Register Cleanup ...

//#ifndef __NOVELL_LIBC__
//	atexit(MF_ConfigFree);
//#endif
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
		
			fprintf(m_fFile,"%s",filter.expression.c_str());
			fputc(0,m_fFile);
			fprintf(m_fFile,"%s",filter.name.c_str());
			fputc(0,m_fFile);
		}
};

bool Configuration::WriteToFile(std::string alternateFilename)
{
	int rc = 0;
	
/*	try {
	
		const char* pConfigFile = (alternateFilename == "" ? config_file.c_str() : alternateFilename.c_str());
		MFD_Out(MFD_SOURCE_CONFIG,"MFC: Starting with %s.\n",pConfigFile);

		std::string fileversion;
		std::ifstream filestream(pConfigFile, std::fstream::in); // | std::fstream::binary);
		
		filestream >> fileversion;
		std::cout << "checking version..." << std::endl << fileversion << std::endl  << "..." << std::endl;
		
		filestream >> this->GWIARoot;
		std::cout << "gwia root: " << this->GWIARoot << std::endl;

		filestream.close();
		

	} catch (std::exception e)
	{
//	 	NetwareAlert(
		consoleprintf("MAILFILTER: Uncaught Exception in the configuration reader.\n\n");
	}
*/


	///////////////////////////
	//
	// --- end of test area ---
	//
	///////////////////////////
	// place definitions here!
	std::string ConfigVersion;

	std::string pConfigFile = (alternateFilename == "" ? this->config_file : alternateFilename);
	std::string pConfigFileBackup = pConfigFile + ".BAK";

	consoleprintf("MFC: Saving to %s.\n",pConfigFile.c_str());
	MFD_Out(MFD_SOURCE_CONFIG,"MFC: Saving to %s.\n",pConfigFile.c_str());

	rename(pConfigFile.c_str(),pConfigFileBackup.c_str());


	// config fix-ups

	// require at least 10 dirs
	if (this->MailscanDirNum < 10) this->MailscanDirNum = 10;


/*	
	ConfigVersion = MF_ConfigReadString(pConfigFile, 0);
	if (ConfigVersion != MAILFILTER_CONFIGURATION_SIGNATURE)
	{
		rc = 99;
		goto MF_ConfigRead_ERR;	
	}
*/

	FILE* cfgFile = fopen(pConfigFile.c_str(),"wb");
	if (cfgFile == NULL)
		return false;
	
	long iMax, iCnt = 0; // macro helpers
#define doNull(howMany)	{ ThreadSwitch(); iMax = ftell(cfgFile); for (iCnt=0;iCnt<(howMany-iMax);iCnt++) fputc(0,cfgFile); }

	// write out header:	
	fprintf(cfgFile,"%s",MAILFILTER_CONFIGURATION_SIGNATURE);
	
	doNull(60);
	fprintf(cfgFile,"%s",this->ControlPassword.c_str());

	// License Key
	doNull(240);
	fprintf(cfgFile,"%s",this->LicenseKey.c_str());

	// Values
	doNull(330);
	fprintf(cfgFile,"%s",this->GWIARoot.c_str());

	doNull(660);
	fprintf(cfgFile,"%s",this->MFLTRoot.c_str());

	doNull(990);
	fprintf(cfgFile,"%s",this->DomainName.c_str());

	doNull(1320);
	fprintf(cfgFile,"%s",this->DomainEmailMailFilter.c_str());

	doNull(1650);
	fprintf(cfgFile,"%s",this->DomainEmailPostmaster.c_str());
	
	doNull(1980);
	fprintf(cfgFile,"%s",this->DomainHostname.c_str());
	
	// version 7
	doNull(2310);
	fprintf(cfgFile,"%s",this->Multi2One.c_str());
	
	// // removed in a later v8
	//doNull(2475);
	//fprintf(cfgFile,"%s",MFC_DNSBL_Zonename);

	// version 8
	doNull(2640);
	fprintf(cfgFile,"%s",this->BWLScheduleTime.c_str());
	
	doNull(2970);
	// next one goes here...

	doNull(3500);
	fprintf(cfgFile, this->DefaultNotification_InternalSender == 0 ? "0" : "1");
	
	doNull(3502);
	fprintf(cfgFile, "%d", this->MailscanDirNum);
	
	doNull(3506);
	fprintf(cfgFile, "%d", this->MailscanTimeout);
	
	doNull(3510);
	fprintf(cfgFile, this->DefaultNotification_InternalRecipient == 0 ? "0" : "1");
	
	doNull(3512);
	fprintf(cfgFile, this->NotificationAdminLogs == 0 ? "0" : "1");

	doNull(3514);
	fprintf(cfgFile, this->NotificationAdminMailsKilled == 0 ? "0" : "1");

	doNull(3516);
	fprintf(cfgFile, "%d", this->ProblemDirMaxAge);

	doNull(3521);
	fprintf(cfgFile, "%d", this->ProblemDirMaxSize);
	
	doNull(3529);
	fprintf(cfgFile, this->NotificationAdminDailyReport == 0 ? "0" : "1");
	
	doNull(3531);
	fprintf(cfgFile, this->DefaultNotification_ExternalSender == 0 ? "0" : "1");
	
	doNull(3533);
	fprintf(cfgFile, this->DefaultNotification_ExternalRecipient == 0 ? "0" : "1");
	
	doNull(3535);
	fprintf(cfgFile, "%d", this->GWIAVersion);

	doNull(3539);
	fprintf(cfgFile, this->EnableIncomingRcptCheck == 0 ? "0" : "1");

	doNull(3541);
	fprintf(cfgFile, this->EnableAttachmentDecoder == 0 ? "0" : "1");

	doNull(3543);
	fprintf(cfgFile, this->EnablePFAFunctionality == 0 ? "0" : "1");

	// version 10
	doNull(3545);
	fprintf(cfgFile, this->EnableNRMThread == 0 ? "0" : "1");
	
	//removed in a later v8
	//fprintf(cfgFile, MFC_DropUnresolvableRelayHosts == 0 ? "0" : "1");

	doNull(3547);
	fprintf(cfgFile, this->EnableNRMRestore == 0 ? "0" : "1");

	// Next start at 3549

	// version 9
	doNull(4000);

	for_each(this->filterList.begin(),this->filterList.end(),PrintFilter(cfgFile));

	fclose(cfgFile);

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

