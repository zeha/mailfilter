/* 
	MFBlacklist-dummy.cpp
	NOTE: THIS IS NOT A WORKING IMPLEMENTATION; IT SERVES AS A STUB
		FOR OLD CLIB MAILFILTER!

	Author:			ch
	Description:	<describe the MFBlacklist class here>
*/

#define _MFD_MODULE "MFBlacklist.cpp"
#include "MFBlacklist.h"

MFBlacklist::MFBlacklist(char* holeZone)
{
	this->m_holeZone = _mfd_strdup(holeZone,"new MFBlacklist");
}

MFBlacklist::~MFBlacklist()
{
	if (!this->m_holeZone)
		return;
	
	_mfd_free(this->m_holeZone,"~MFBlacklist");
	
}

int MFBlacklist::LookupIpAsync(char* host)
{
	return this->Lookup(host);
}

int MFBlacklist::Lookup(char* host)
{
	struct hostent* he;
	struct in_addr ad;
	int rc = 0;
	char* fullHost = (char*)_mfd_malloc(MAX_PATH,"LookupAsync");
	sprintf(fullHost,"%s.%s",host,this->m_holeZone);


		MFD_Out(MFD_SOURCE_SMTP,"Blacklist: gethostbyname(%s)\n",host);
		he = gethostbyname(host);
		if (!he)
		{
			MFD_Out(MFD_SOURCE_SMTP,"Blackhole check failed [no ip, no host] for '%s'\n",host);
			_mfd_free(fullHost,"LookupAsync|fullHost");
			return 0;
		} else {
			if (he->h_addr_list)
			{
				memcpy(&ad.S_un.S_addr,he->h_addr_list[0],he->h_length);
				sprintf(fullHost,"%d.%d.%d.%d.%s",
					ad.S_un.S_un_b.s_b4,
					ad.S_un.S_un_b.s_b3,
					ad.S_un.S_un_b.s_b2,
					ad.S_un.S_un_b.s_b1,
					this->m_holeZone);
				
			}
				else
				{
					MFD_Out(MFD_SOURCE_SMTP,"Blackhole check failed [no ip from host] for '%s'\n",host);
					_mfd_free(fullHost,"LookupAsync|fullHost");
					return 0;
				}
		}

	MFD_Out(MFD_SOURCE_SMTP,"Blackhole check for %s\n",fullHost);
	he = gethostbyname(fullHost);
	if (!he)
	{	
		MFD_Out(MFD_SOURCE_SMTP,"Blacklist error %d\n",h_errno);
		rc = 0;
	} else {
	
		MFD_Out(MFD_SOURCE_SMTP,"Blacklist entry found for %s\n",host);
		rc = 1;
	}

	_mfd_free(fullHost,"LookupIpAsync");
	


	return rc;
}

