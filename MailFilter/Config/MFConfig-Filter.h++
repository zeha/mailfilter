/*+
 +		MFConfig-Filter.h++
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
#include <vector>

#ifndef __MAILFILTER_CONFIGURATION_FILTER_CPP_HEADER
#define __MAILFILTER_CONFIGURATION_FILTER_CPP_HEADER

#define MAILFILTER_CONFIGURATION_FILTERLISTTYPE		std::vector<MailFilter_Configuration::Filter>

namespace MailFilter_Configuration
{

enum FilterField { always = 0, attachment = 1, email = 2, subject = 3, size = 4, emailFrom = 5, emailTo = 6, blacklist = 7, ipUnresolvable = 8, archiveContentName = 9, archiveContentCount = 10, emailToAndCC = 11, emailBothAndCC = 12, virus = 13 }; 
enum FilterAction { noAction = 0, dropMail = 1, moveMail = 2, rescan = 4, pass = 8, schedule = 0x10, noschedule = 0x20, copyMail = 0x40 };
enum Notification { noNotification = 0, adminIncoming = 1, adminOutgoing = 2, senderIncoming = 4, senderOutgoing = 8, recipientIncoming = 0x10, recipientOutgoing = 0x20 };
enum FilterType { match = 0, noMatch = 1 };

class Filter
{
public:
	char *sig;
	FilterField matchfield;
	std::string expression;
	std::string name;
	bool enabled;
	bool enabledIncoming;
	bool enabledOutgoing;
	FilterType type;
	Notification notify;
	FilterAction action;
	bool fromStock;
	unsigned int hitCounter;
	
	Filter(){ sig = "FILT"; }
};

} // namespace MailFilter_Configuration

#endif // __MAILFILTER_CONFIGURATION_FILTER_CPP_HEADER
