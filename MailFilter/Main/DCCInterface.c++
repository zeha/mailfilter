/* 
	DCCInterface.c++

	Author:			ch
	Description:	<describe the DCCInterface class here>
*/

#include "DCCInterface.h++"
#define INVALID_SOCKET 0
#define SOCKET_ERROR -1

#ifdef _WITH_EXPERIMENTAL_DCC

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

static void gosend(SOCKET s, const char* data)
{
	send(s,data,strlen(data),0);
}


DCCInterface::DCCInterface(const char* szDCCIfdHost, int iDCCIfdPort)
{
	this->bIsSpam = false;
	this->sock = INVALID_SOCKET;

	char buf[2048];
	int bytesRecv;
	char recvbuf[32] = "";
	SOCKADDR_IN addr;

	addr.sin_addr.s_addr = inet_addr(szDCCIfdHost);
	addr.sin_family = AF_INET;
	addr.sin_port = htons( iDCCIfdPort );
  
	this->sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (this->sock == INVALID_SOCKET)
		return;
	if (connect(this->sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		this->sock = INVALID_SOCKET;
		return;
	}
}


DCCInterface::~DCCInterface()
{
}

#endif

