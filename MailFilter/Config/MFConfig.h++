/*+
 +		MFConfig.h++
 +		
 +		MailFilter/ax Configuration C++ Interface
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
#include <vector>

#ifndef __MAILFILTER_CONFIGURATION_CPP_HEADER
#define __MAILFILTER_CONFIGURATION_CPP_HEADER

namespace MailFilter_Configuration
{

#include "MFConfig-defines.h"

/*
enum FilterField { noField = 0, attachment = 1, email = 2, subject = 3, size = 4, emailFrom = 5, emailTo = 6, blacklist = 7, ipUnresolvable = 8 }; 
enum FilterAction { noAction = 0, dropMail = 1, moveMail = 2, rescan = 4, pass = 8, schedule = 0x10, noschedule = 0x20 };
enum Notification { noNotification = 0, adminIncoming = 1, adminOutgoing = 2, senderIncoming = 4, senderOutgoing = 8, recipientIncoming = 0x10, recipientOutgoing = 0x20 };
enum FilterType { match = 0, noMatch = 1 };

class Filter
{
public:
	FilterField matchfield;
	std::string expression;
	std::string name;
	bool enabled;
	bool enabledIncoming;
	bool enabledOutgoing;
	FilterType type;
	Notification notify;
	FilterAction action;
};

// Compatiblity Only:
struct OldC_FilterStruct
{
	char matchfield;
	char expression[500];
	char name[60];
	char enabled;
	char enabledIncoming;
	char enabledOutgoing;
	char type;
	char notify;
	char action;
//	char action2[1000];
};
*/

#include "MFConfig-Filter.h++"

//#define MAILFILTER_CONFIGURATION_SIGNATURE			"MAILFILTER_R001_009"		//*** MODIFY THESE
//#define MAILFILTER_CONFIGURATION_THISBUILD			9							//    TWO BUILD NUMBERS ***
//#define MAILFILTER_CONFIGURATION_LENGTH				329

class Configuration
{
public:
	std::string config_directory;
	std::string config_file;
	std::string config_file_backup;
	std::string config_file_void;
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
	std::string MessageFooter;
	std::string BWLScheduleTime;
	std::string Multi2One;
	std::string LogDirectory;
	
	std::string	MessageFooterText;
	
	std::vector<MailFilter_Configuration::Filter> filterList;

	int setDefaults(std::string directory = "", std::string domainname = "");

	bool isCurrentVersion() { return config_build == MAILFILTER_CONFIGURATION_THISBUILD; }
	
	bool ReadFromFile(std::string alternateFilename);
	bool WriteToFile(std::string alternateFilename);

	bool ReadFilterList();
	
	Configuration();
	~Configuration();

private:
//	std::iostream filestream;

};

} // namespace MailFilter_Configuration

#endif // __MAILFILTER_CONFIGURATION_CPP_HEADER
