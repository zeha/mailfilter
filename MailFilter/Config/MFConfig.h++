/*+
 +		MFConfig.h++
 +		
 +		MailFilter Configuration C++ Interface
 +		
 +		Copyright 2001-2004 Christian Hofstädtler
 +		
 +		
 +		
 +		
+*/


#include <string>
#include <iostream>
#include <vector>

#include "MFConfig-Filter.h++"

#ifndef __MAILFILTER_CONFIGURATION_CPP_HEADER
#define __MAILFILTER_CONFIGURATION_CPP_HEADER

namespace MailFilter_Configuration
{

#include "MFConfig-defines.h"

//#define MAILFILTER_CONFIGURATION_SIGNATURE			"MAILFILTER_R001_009"		//*** MODIFY THESE
//#define MAILFILTER_CONFIGURATION_THISBUILD			9							//    TWO BUILD NUMBERS ***
//#define MAILFILTER_CONFIGURATION_LENGTH				329

	enum ApplicationModeType { 
		SERVER, 
		CONFIG, 
		RESTORE, 
		NRM,
		INSTALL
		};

class Configuration
{
public:
	std::string config_directory;
	std::string config_file;
	std::string config_file_backup;
	std::string config_file_void;
	bool config_mode_strict;
	
	unsigned int config_build;
	std::string ServerName;
	std::string GWIARoot;
	std::string MFLTRoot;
	std::string DomainName;
	std::string DomainHostname;
	std::string DomainEmailPostmaster;
	std::string DomainEmailMailFilter;
	std::string LicenseKey;
	std::string ControlPassword;
	MailFilter_Configuration::Notification DefaultNotification_InternalRecipient;
	MailFilter_Configuration::Notification DefaultNotification_InternalSender;
	MailFilter_Configuration::Notification DefaultNotification_ExternalRecipient;
	MailFilter_Configuration::Notification DefaultNotification_ExternalSender;
	unsigned int MailscanDirNum;
	unsigned int MailscanTimeout;
	bool NotificationAdminLogs;
	bool NotificationAdminMailsKilled;
	bool NotificationAdminDailyReport;
	unsigned int ProblemDirMaxSize;
	unsigned int ProblemDirMaxAge;
	unsigned int GWIAVersion;
	bool EnableIncomingRcptCheck;
	bool EnableAttachmentDecoder;
	bool EnablePFAFunctionality;
	bool EnableNRMThread;
	bool EnableNRMRestore;
	std::string MessageFooter;
	std::string BWLScheduleTime;
	std::string Multi2One;
	std::string LogDirectory;
	
	std::string	MessageFooterText;
	
	//std::vector<MailFilter_Configuration::Filter> filterList;
	MAILFILTER_CONFIGURATION_FILTERLISTTYPE filterList;

	ApplicationModeType ApplicationMode;
	bool NRMInitialized;

	int setDefaults(std::string directory = "", std::string domainname = "");

	bool isCurrentVersion() { return config_build == MAILFILTER_CONFIGURATION_THISBUILD; }
	
	bool ReadFromFile(std::string alternateFilename);
	bool WriteToFile(std::string alternateFilename);

	// don't use ReadFilterList -- use ReadFilterListFromConfig or ReadFilterListFromRulePackage
	bool ReadFilterList(std::string filterFile, long startAt);
	bool ReadFilterListFromConfig();
	bool ReadFilterListFromRulePackage(std::string filterFile);

	bool CreateFromInstallFile(std::string installFile);
	
	Configuration();
	~Configuration();

private:
//	std::iostream filestream;

};

} // namespace MailFilter_Configuration

#endif // __MAILFILTER_CONFIGURATION_CPP_HEADER
