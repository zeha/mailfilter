/* 
	MFBlacklist.h

	Author:			ch
	Description:	<describe the MFBlacklist class here>
*/

#ifndef MFBlacklist_H
#define MFBlacklist_H

#include "MailFilter.h"


class MFBlacklist
{
public:
	MFBlacklist(std::string holeZone, std::string validResult);
	int Lookup(std::string host);
	int CheckValidName(std::string host);
	~MFBlacklist();
	
	
protected:
	std::string m_holeZone;
	std::string m_validResult;

};

#endif	// MFBlacklist_H
