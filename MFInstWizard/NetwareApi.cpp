#include "StdAfx.h"
#include "netwareapi.h"

#include "E:\Novell\NDK\nwsdk\include\ntypes.h"
#include "E:\Novell\NDK\nwsdk\include\nwmisc.h"
#include "E:\Novell\NDK\nwsdk\include\nwcalls.h"
#include "E:\Novell\NDK\nwsdk\include\nwclxcon.h"
#include ".\netwareapi.h"
  
NetwareApi::NetwareApi(void)
: m_ClientConnection(-1)
{
	NWCallsInit(NULL,NULL);
}

NetwareApi::~NetwareApi(void)
{
	if (this->m_ClientConnection != -1)
		NWCCCloseConn(this->m_ClientConnection);
}

// Asks the specified server if the logged in user has console operator rights
bool NetwareApi::CheckConsoleOperatorRight()
{
	if (this->m_ClientConnection == -1)
		return false;

	if (NWCheckConsolePrivileges(this->m_ClientConnection) == 0)
		return true;

	return false;
}


// Loads the specified NLM on the specified server
bool NetwareApi::LoadNLM(const char* szNLMName)
{
	if (this->m_ClientConnection == -1)
		return false;

	if (NWSMLoadNLM(this->m_ClientConnection,szNLMName) == 0)
		return true;

	return false;
}

// Runs the specified NCF on the specified server
bool NetwareApi::ExecuteNCF(const char* szNCFName)
{
	if (this->m_ClientConnection == -1)
		return false;

	if (NWSMExecuteNCFFile(this->m_ClientConnection,szNCFName) == 0)
		return true;

	return false;
}


// Get the primary server name from Client32
bool NetwareApi::GetPrimaryServerName(CString* szServerName)
{
	nuint32			connRef;
	NWCCConnInfo	connInfo;

	if (NWCCGetPrimConnRef(&connRef))
		return false;

	/*  Get information about the primary connection. */
	if (NWCCGetAllConnRefInfo(connRef, NWCC_INFO_VERSION, &connInfo))
		return false;

	*szServerName = (char[49])(connInfo.serverName);
	return true;
}

bool NetwareApi::GetPrimaryServerName(char* szServerBuffer, signed int bufferSize)
{
	CString tmp;
	if (!this->GetPrimaryServerName(&tmp))
		return false;

	/*  print the primary connection server name.  */
	if (tmp.GetLength() < bufferSize)
	{
		strcpy(szServerBuffer,tmp);
		return true;
	}
	return false;
}


// Set the server name to use for the next actions.
bool NetwareApi::SelectServerByName(const char* szServerName)
{
	if (this->m_ClientConnection != -1)
		NWCCCloseConn(this->m_ClientConnection);

	this->m_ClientConnection = -1;
	if (NWCCOpenConnByName(0, szServerName, NWCC_NAME_FORMAT_WILD, NWCC_OPEN_LICENSED | NWCC_OPEN_PUBLIC, NWCC_TRAN_TYPE_WILD, &this->m_ClientConnection) == 0)
		return true;

	return false;
}

// Return all to the Client known Servers
bool NetwareApi::GetServerList(CStringArray &serverList)
{
	nuint32 scanIterator = 0;
	nuint32 connRef = 0;
	NWCCConnInfo connInfo;
	CString tmp;

	serverList.RemoveAll();

	while (NWCCScanConnRefs(&scanIterator,&connRef) == 0)
	{
		if (NWCCGetAllConnRefInfo(connRef, NWCC_INFO_VERSION, &connInfo) == 0)
		{
			tmp = (char[49])(connInfo.serverName);
			serverList.Add(tmp);
		}
	}
	return true;
}

// Returns the NetWare Server Version
bool NetwareApi::GetServerVersion(unsigned int &majorVersion, unsigned int &minorVersion, unsigned int &revision)
{
	majorVersion = 0;
	minorVersion = 0;
	revision = 0;

	NWCCVersion vInfo;

	if (this->m_ClientConnection == -1)
		return false;

	if (NWCCGetConnInfo(this->m_ClientConnection,NWCC_INFO_SERVER_VERSION,sizeof(NWCCVersion),&vInfo) != 0)
		return false;

	majorVersion = vInfo.major;
	minorVersion = vInfo.minor;
	revision = vInfo.revision;

	return true;
}
