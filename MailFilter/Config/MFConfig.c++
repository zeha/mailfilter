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

#define _MFD_MODULE			"MFCONFIG.C++"
#include "MailFilter.h"
#include "MFConfig.h++"
#include "MFConfig-Filter.h++"

char MFC_ConfigFile[MAX_PATH] = "";
char* MFT_Local_ServerName = NULL;

namespace MailFilter_Configuration {

std::string MF_ConfigReadString(std::string ConfigFile, const int Entry)
{
	FILE* cfgFile;
	size_t dRead;
	char Value[MAILFILTER_CONFIGURATION_LENGTH+1];

	Value[0]=0;

	cfgFile = fopen(ConfigFile.c_str(),"rb");

	if (cfgFile == NULL)
		return "";

	fseek(cfgFile,Entry*(MAILFILTER_CONFIGURATION_LENGTH+1),SEEK_SET);
	
	dRead = fread(Value,sizeof(char),MAILFILTER_CONFIGURATION_LENGTH-2,cfgFile);
	
	fclose(cfgFile);
	Value[dRead+1]=0;

	Value[MAX_PATH-2]=0;
	
	return Value;
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
	if ( (this->config_directory == "") && (directory != "") ) { this->config_directory = directory; }
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
	
	this->MessageFooter = "";
	this->Multi2One = "";
	this->BWLScheduleTime = "";
	
	return 0;
}
/*
void delFilter(MailFilter_Configuration::Filter flt)
{
	delete(&flt);
}*/

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

bool Configuration::ReadFilterList()
{
	FILE* fFile;
	long rc;
//	long rc2;
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


	// version 8
	
	// here we go again ...

	fFile = fopen(this->config_file.c_str(),"rb");
	fseek(fFile,4000,SEEK_SET);

//	for (curItem = 0; curItem<MailFilter_MaxFilters; curItem++)
	while (!feof(fFile))
	{
	
		MailFilter_Configuration::Filter flt; // = new MailFilter_Configuration::Filter();
		
/*		MFC_Filters[curItem].matchfield = (char)fgetc(cfgFile);
		MFC_Filters[curItem].notify = (char)fgetc(cfgFile);
		MFC_Filters[curItem].type = (char)fgetc(cfgFile);
		MFC_Filters[curItem].action = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabled = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledIncoming = (char)fgetc(cfgFile);
		MFC_Filters[curItem].enabledOutgoing = (char)fgetc(cfgFile);
		
		rc1 = (long)fread(szTemp,sizeof(char),1000,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc1+1,SEEK_CUR);
		
		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].expression,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].expression[0]=0;		

		rc2 = (long)fread(szTemp,sizeof(char),1000,cfgFile);
		fseek(cfgFile,((int)(strlen(szTemp)))-rc2+1,SEEK_CUR);

		if (strlen(szTemp) > 0) strncpy(MFC_Filters[curItem].name,szTemp,strlen(szTemp)+1);
			else MFC_Filters[curItem].name[0]=0;

		if (MFC_Filters[curItem].expression[0]==0)
			break;
*/

		flt.matchfield = (MailFilter_Configuration::FilterField)(fgetc(fFile));
		flt.notify = (MailFilter_Configuration::Notification)(fgetc(fFile));
		flt.type = (MailFilter_Configuration::FilterType)(fgetc(fFile));
		flt.action = (MailFilter_Configuration::FilterAction)(fgetc(fFile));
		flt.enabled = (bool)(fgetc(fFile));
		flt.enabledIncoming = (bool)(fgetc(fFile));
		flt.enabledOutgoing = (bool)(fgetc(fFile));
		
		rc = (long)fread(szTemp,sizeof(char),1000,fFile);
		fseek(fFile,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
		
		if (strlen(szTemp) > 0) flt.expression = szTemp;

		rc = (long)fread(szTemp,sizeof(char),1000,fFile);
		fseek(fFile,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

		if (strlen(szTemp) > 0) flt.name = szTemp;
		
		if (flt.expression == "")
		{
		//	delete flt;
			break;
		}
		
//		consoleprintf("pushing filter()\n");
		this->filterList.push_back(flt);
			
/*		if (regcomp(&re,MFC_Filters[curItem].expression))
			MFD_Out("*FAILED* Filter: %s\n",curItem,MFC_Filters[curItem].expression);
			else
			regfree(&re);
*/			
	}

	fclose(fFile);
	
/*	MFT_ModifyInt_ListFrom[0][0]=0;
	
	MF_ConfigReadString(MAILFILTER_CONFIGURATION_MAILFILTER, 7*MAILFILTER_CONFIGURATIONFILE_STRING, (MFT_ModifyInt_ListFrom[0]));
	
	MFT_ModifyInt_ListFrom[1][0]=0;
*/	
	
	
	return true;
	

}
 
bool Configuration::ReadFromFile(std::string alternateFilename)
{
	int rc = 0;
	struct stat statInfo;
//	char szTemp[MAX_PATH];
	
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

	consoleprintf("MFC: Starting with %s.\n",pConfigFile.c_str());
	MFD_Out(MFD_SOURCE_CONFIG,"MFC: Starting with %s.\n",pConfigFile.c_str());

	// compatiblity for MF Licensing Stages
	strncpy(MFC_ConfigFile,pConfigFile.c_str(),MAX_PATH-1);


	if (stat(pConfigFile.c_str(),&statInfo))
		rc = 98;
	if (rc != 0)	goto MF_ConfigRead_ERR;


	if (statInfo.st_size < 3999)
		rc = 97;	
	if (rc != 0)	goto MF_ConfigRead_ERR;
	
	ConfigVersion = MF_ConfigReadString(pConfigFile, 0);
	if (ConfigVersion != MAILFILTER_CONFIGURATION_SIGNATURE)
	{
		rc = 99;
		goto MF_ConfigRead_ERR;	
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

	this->GWIARoot = MF_MakeValidPath(MF_ConfigReadString(pConfigFile, 1));
	if (this->GWIARoot == "") {	rc = 303;	goto MF_ConfigRead_ERR;	}

	this->MFLTRoot = MF_MakeValidPath(MF_ConfigReadString(pConfigFile, 2));
	if (this->MFLTRoot == "") {	rc = 304;	goto MF_ConfigRead_ERR;	}

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

//	MFC_Multi2One = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
	this->Multi2One = MF_ConfigReadString(pConfigFile, 7);

//	MFC_DNSBL_Zonename = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
//	MF_ConfigReadString(pConfigFile, (7*MAILFILTER_CONFIGURATIONFILE_STRING + (MAILFILTER_CONFIGURATIONFILE_STRING/2)), MFC_DNSBL_Zonename);

//	MFC_BWL_ScheduleTime = (char*)malloc(MAILFILTER_CONFIGURATION_LENGTH);
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

//	MFC_DropUnresolvableRelayHosts = 
//		MF_ConfigReadInt(pConfigFile, 3545);

	// Next start at 3547
	
	// require at least 10 dirs
	if (this->MailscanDirNum < 10) this->MailscanDirNum = 10;

	/* read in the optional footer message */
/*	strncpy(szTemp,MFC_ConfigDirectory,MAX_PATH);
	strncat(szTemp,"MSGFOOT.MFT",MAX_PATH);
	
	MFC_Message_FooterText = (char*)malloc(4000);
	MF_ConfigReadFile(szTemp,MFC_Message_FooterText,3999);
*/	
	if (!MFT_RemoteDirectories)
	{
		// Check for existence of directories ...
		if (chdir(this->MFLTRoot.c_str())) {	rc=301; goto MF_ConfigRead_ERR;	}
		if (chdir(this->GWIARoot.c_str())) {	rc=302; goto MF_ConfigRead_ERR;	}
	
		// Create Logging Directory if it is missing
//#ifdef WIN32
//		if (chdir(MFT_MF_LogDir))				mkdir((const char*)MFT_MF_LogDir,0);
//#else
		if (chdir(this->LogDirectory.c_str()))				mkdir(this->LogDirectory.c_str(),S_IRWXU);
//#endif
	}

	// Initialize MailFilter BlockList Cache
	//if ( MF_Filter_InitLists() == false ) {	rc = 299;	goto MF_ConfigRead_ERR;	}
	if ( this->ReadFilterList() == false ) {	rc = 299;	goto MF_ConfigRead_ERR;	}

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
		
//		MF_ConfigFree();

		return false;
	}

	// No Errors! Register Cleanup ...

//#ifndef __NOVELL_LIBC__
//	atexit(MF_ConfigFree);
//#endif
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

