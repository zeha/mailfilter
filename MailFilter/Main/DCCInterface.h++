/* 
	DCCInterface.h++

	Author:			ch
	Description:	<describe the DCCInterface class here>
*/

#ifndef DCCInterface_H
#define DCCInterface_H

#include "MailFilter.h"
#include <sys/socket.h>
#include <string>

class DCCInterface
{
public:
	DCCInterface(const char* szDCCIfdHost, int iDCCIfdPort);
	virtual ~DCCInterface();
	
	bool SendMailLine(std::string line);
	bool Done();
	
	bool bIsSpam;
	
protected:
	SOCKET sock;
};

#endif	// DCCInterface_H
