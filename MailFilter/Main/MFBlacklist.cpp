/* 
	MFBlacklist.cpp

	Author:			ch
	Description:	<describe the MFBlacklist class here>
*/

#define _MFD_MODULE "MFBlacklist.cpp"
#include "MFBlacklist.h"
#ifdef __NOVELL_LIBC__
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

//#ifdef NETDB_DEFINE_CONTEXT 
//NETDB_DEFINE_CONTEXT 
//#endif

MFBlacklist::~MFBlacklist()
{
}

MFBlacklist::MFBlacklist(std::string holeZone, std::string validResult)
{
MFD_Out(MFD_SOURCE_RULE,"got %s / %s\n",holeZone.c_str(),validResult.c_str());
	this->m_holeZone = holeZone; //,"MFBlacklist::MFBlacklist [1]");
	this->m_validResult = validResult; //,"MFBlacklist::MFBlacklist [2]");
}

int MFBlacklist::Lookup(std::string host)
{
	struct hostent* he;
	struct in_addr ad;
	int rc = 0;
	char* fullHost = (char*)_mfd_malloc(MAX_PATH+1,"Lookup|fullHost");

#ifdef snprintf
		snprintf(fullHost,MAX_PATH,
#else
		sprintf(fullHost,
#endif
		"%s.%s",host.c_str(),this->m_holeZone.c_str(),this->m_validResult.c_str());

//	MF_StatusText("  Blacklist Check. This may take a while.");

#ifdef inet_aton
	if (inet_aton (host.c_str(), &ad))
	{
#else
	long addr = (long)inet_addr(host.c_str());
	if (addr != (-1))
	{

		ad.S_un.S_addr = (unsigned long)addr;
#endif
		MFD_Out(MFD_SOURCE_SMTP,"made %d.%d.%d.%d\n",
				ad.S_un.S_un_b.s_b1,
				ad.S_un.S_un_b.s_b2,
				ad.S_un.S_un_b.s_b3,
				ad.S_un.S_un_b.s_b4);
				
		/* rbl wants it reverse */
#ifdef snprintf
		snprintf(fullHost,MAX_PATH,
#else
		sprintf(fullHost,
#endif
				"%d.%d.%d.%d.%s",
				ad.S_un.S_un_b.s_b4,
				ad.S_un.S_un_b.s_b3,
				ad.S_un.S_un_b.s_b2,
				ad.S_un.S_un_b.s_b1,
			this->m_holeZone.c_str());

	} else 
		{
		MFD_Out(MFD_SOURCE_SMTP,"  Blacklist: gethostbyname(%s)\n",host.c_str());
		he = gethostbyname(host.c_str());
		if (!he)
		{
			MFD_Out(MFD_SOURCE_SMTP,"  Blackhole check failed [no ip, no host] for '%s'\n",host.c_str());
			_mfd_free(fullHost,"Lookup|fullHost");
			return 2;
		} else {
			if (he->h_addr_list)
			{
				memcpy(&ad.S_un.S_addr,he->h_addr_list[0],he->h_length);

#ifdef __NOVELL_LIBC__
				snprintf(fullHost,MAX_PATH,
#else
				sprintf(fullHost,
#endif
					"%d.%d.%d.%d.%s",
					ad.S_un.S_un_b.s_b4,
					ad.S_un.S_un_b.s_b3,
					ad.S_un.S_un_b.s_b2,
					ad.S_un.S_un_b.s_b1,
					this->m_holeZone.c_str());
				
			}
				else
				{
					MFD_Out(MFD_SOURCE_SMTP,"  Blackhole check failed [no ip from host] for '%s'\n",host.c_str());
					_mfd_free(fullHost,"Lookup|fullHost");
					return 0;
				}
		}
	}

	MFD_Out(MFD_SOURCE_SMTP,"  Blackhole check for %s\n",fullHost);
	he = gethostbyname(fullHost);
	if (!he)
	{	
		MFD_Out(MFD_SOURCE_SMTP,"  Blacklist result: %d\n",h_errno);
		rc = 0;
	} else {
	
		MFD_Out(MFD_SOURCE_RULE,"  Blacklist entry found for %s\n",host.c_str());

		if (this->m_validResult[0] == 0)
		{
			MFD_Out(MFD_SOURCE_RULE,"   --> generic blacklist, ack.\n");
		} else {
			for (int i=0; i<he->h_length;i++)
			{
				if (he->h_addr_list[i] == NULL)
					break;
				MFD_Out(MFD_SOURCE_RULE,"   --> BL returned: %d %s %d\n",he->h_length,he->h_name,he->h_addr_list[i]);
			}
		}
		rc = 1;
	}

	_mfd_free(fullHost,"Lookup|fullHost");
	
	return rc;
}


int MFBlacklist::CheckValidName(std::string host)
{
	struct hostent* he;
//	struct in_addr ad;
	int rc = 0;

/*#ifdef inet_aton
	if (inet_aton (host, &ad))
	{
#else	*/
	long addr = (long)inet_addr(host.c_str());
	if (addr != (-1))
	{
/*#endif*/
		// ok, an ip is listed
		rc = 0;
		
	} else 
		{
		
		MFD_Out(MFD_SOURCE_SMTP,"  CheckValidName: gethostbyname(%s)\n",host.c_str());
		
		he = gethostbyname(host.c_str());
		if (!he)
		{	// no result from gethostbyname
			// --> no ip for the named host.
			rc = 2;
		} else {
			// we got back a result, whatever it might be.
			// --> assume it's ok
			rc = 0;
		}
	}
	
	return rc;
}

