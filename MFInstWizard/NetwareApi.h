#pragma once

#include "E:\Novell\NDK\nwsdk\include\ntypes.h"

class NetwareApi
{
public:
	NetwareApi(void);
	~NetwareApi(void);

	// Set the server name to use for the next actions.
	bool SelectServerByName(const char* szServerName);

	// Asks the specified server if the logged in user has console operator rights
	bool CheckConsoleOperatorRight();

	// Loads the specified NLM on the specified server
	bool LoadNLM(const char* szNLMName);

	// Runs the specified NCF on the specified server
	bool ExecuteNCF(const char* szNCFName);

	// Get the primary server name from Client32
	bool GetPrimaryServerName(char* szServerBuffer, signed int bufferSize);

	// Get the primary server name from Client32
	bool GetPrimaryServerName(CString* szServerName);

protected:
	unsigned int m_ClientConnection;
};
