/* 
	MFRelayHost.cpp

	Author:			ch
	Description:	<describe the MFRelayHost class here>
*/

#define _MFD_MODULE "MFRelayHost.c++"
#include "MFRelayHost.h++"
#include "MailFilter.h"

MFRelayHost::MFRelayHost(std::string relayhost)
{
	
	this->state = 0;
	this->host = relayhost;
	if (this->host == "")
		return;
		
	this->state = 1;
		
	struct hostent* he;

#ifdef inet_aton
	if (inet_aton (host.c_str(), &host_ipaddress))
	{
#else
	long addr = (long)inet_addr(host.c_str());
	if (addr != (-1))
	{

		host_ipaddress.S_un.S_addr = (unsigned long)addr;
#endif
		this->state = 3;	// Lookup complete
		
		MFD_Out(MFD_SOURCE_SMTP,"made %d.%d.%d.%d\n",
				host_ipaddress.S_un.S_un_b.s_b1,
				host_ipaddress.S_un.S_un_b.s_b2,
				host_ipaddress.S_un.S_un_b.s_b3,
				host_ipaddress.S_un.S_un_b.s_b4);
		
	} else {
		MFD_Out(MFD_SOURCE_SMTP,"RelayHost: gethostbyname(%s)\n",host.c_str());
		he = gethostbyname(host.c_str());
		if (!he)
		{
			MFD_Out(MFD_SOURCE_SMTP,"Relay lookup failed [no ip, no host] for '%s'\n",host.c_str());
			this->state = 2;	// Cannot Lookup
			return;
		} else {
			if (he->h_addr_list)
			{
				memcpy(&host_ipaddress.S_un.S_addr,he->h_addr_list[0],(unsigned int)he->h_length);
				this->state = 3;	// Lookup complete

			} else 
			{
				MFD_Out(MFD_SOURCE_SMTP,"Relay lookup failed [no ip from host] for '%s'\n",host.c_str());
				this->state = 2; // Cannot lookup
				return;
			}
		}
	}
		
}

int MFRelayHost::LookupRBL_DNS(std::string holeZone, std::string validResult)
{
	if (this->state < 3)
		return 0;
	
	struct hostent* he;
//	struct in_addr ad;
	int rc = 0;
	char* fullHost = (char*)_mfd_malloc(MAX_PATH+1,"Lookup|fullHost");

			
	/* rbl wants it reverse */
#ifdef snprintf
	snprintf(fullHost,MAX_PATH,
#else
	sprintf(fullHost,
#endif
			"%d.%d.%d.%d.%s",
			host_ipaddress.S_un.S_un_b.s_b4,
			host_ipaddress.S_un.S_un_b.s_b3,
			host_ipaddress.S_un.S_un_b.s_b2,
			host_ipaddress.S_un.S_un_b.s_b1,
			holeZone.c_str());
			
	if (
		 
		(	// 192.168.x.x
			(host_ipaddress.S_un.S_un_b.s_b1 == 192) && 
			(host_ipaddress.S_un.S_un_b.s_b2 == 168)
			) ||

		(	// 172.16.0.0      -   172.31.255.255
			(host_ipaddress.S_un.S_un_b.s_b1 == 172) && 
			(host_ipaddress.S_un.S_un_b.s_b2 >= 16) &&
			(host_ipaddress.S_un.S_un_b.s_b2 <= 31)
			) ||
			
		(	// 10.x.x.x
			(host_ipaddress.S_un.S_un_b.s_b1 == 10)
			)
		
		)
		{
			// do not send "private ips" to rbl name servers
			return 0;
		}

	he = gethostbyname(fullHost);
	if (!he)
	{	
		MFD_Out(MFD_SOURCE_SMTP,"Blackhole check %s result: %d\n",fullHost,h_errno);
		rc = 0;
	} else {
	
		MFD_Out(MFD_SOURCE_RULE,"Blacklist entry found for %s\n",host.c_str());

		if (validResult[0] == 0)
		{
			MFD_Out(MFD_SOURCE_RULE," --> generic blacklist, ack.\n");
		} else {
			for (int i=0; i<he->h_length;i++)
			{
				if (he->h_addr_list[i] == NULL)
					break;
				MFD_Out(MFD_SOURCE_RULE," --> BL returned: %d %s %d\n",he->h_length,he->h_name,he->h_addr_list[i]);
			}
		}
		rc = 1;
	}

	_mfd_free(fullHost,"Lookup|fullHost");
	
	return rc;
}


int MFRelayHost::LookupDNS()
{
	if (this->state == 2) //== Cannot Lookup
		return 2;
	else				//== Can Lookup or Empty String or whatever
		return 0;
}

