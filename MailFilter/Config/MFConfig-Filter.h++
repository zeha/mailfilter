/*+
 +		MFConfig-Filter.h++
 +		
 +		MailFilter/ax Configuration C++ Interface
 +		
 +		Copyright 2001-2003 Christian Hofstädtler
 +		
 +		
 +		- Oct/2003 ; ch   ; Moved from MFConfig.h++
 +		
 +		
+*/


#include <string>
#include <iostream>
#include <vector>

#ifndef __MAILFILTER_CONFIGURATION_FILTER_CPP_HEADER
#define __MAILFILTER_CONFIGURATION_FILTER_CPP_HEADER

namespace MailFilter_Configuration
{

enum FilterField { always = 0, attachment = 1, email = 2, subject = 3, size = 4, emailFrom = 5, emailTo = 6, blacklist = 7, ipUnresolvable = 8, archiveContentName = 9, archiveContentCount = 10, emailToAndCC = 11, emailBothAndCC = 12 }; 
enum FilterAction { noAction = 0, dropMail = 1, moveMail = 2, rescan = 4, pass = 8, schedule = 0x10, noschedule = 0x20, copy = 0x40 };
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

} // namespace MailFilter_Configuration

#endif // __MAILFILTER_CONFIGURATION_FILTER_CPP_HEADER
