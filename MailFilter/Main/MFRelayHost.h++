/* 
	MFRelayHost.h

	Author:			ch
	Description:	<describe the MFRelayHost class here>
*/

#ifndef MFRelayHost_H
#define MFRelayHost_H

#include "MailFilter.h"
#include <string>


class MFRelayHost
{
public:
	MFRelayHost(std::string relayhost);
	~MFRelayHost();
	int LookupRBL_DNS(std::string holeZone, std::string validResult);
	int LookupDNS();
	
protected:
	std::string host;
	struct in_addr host_ipaddress;
	unsigned int state;

};

#endif	// MFRelayHost_H
