#include <winsock/novsock2.h>

#define OK_SUCCESS 0
#define ERR_RUNNING 1
#define ERR_WSAFAIL 2
#define ERR_INVALIDSOCKET 3
#define ERR_BIND 4
#define ERR_LISTEN 5
#define ERR_HOST 6
#define ERR_CONNECT 7
#define ERR_STOPPED 8


class SMTPProxy  
{
private:
	struct SSmtp
	{
		char SmtpServer[250];
		char LocalIP[250];
		char ClientIP[20];
		unsigned int SmtpPort;
		unsigned int LocalPort;
		in_addr_t SmtpServer_addr;
		in_addr_t LocalIP_addr;
		SOCKET LocalSocket;
		SMTPProxy* pSMTPProxy;
	public:
		void CopyStruct(SSmtp *pSSmtp);
	};
	struct socket_pair
	{
		SOCKET src;
		SOCKET dest;
		BOOL IsClient;
		SMTPProxy* pSMTPProxy;
	};

protected:
	BOOL AcceptFlag;
	SSmtp m_SSmtp;
	SOCKET SmtpServerSocket;	
	char ProxySignature[2048];	

public:
	int StopProxy();
	int StartProxy(const char *server, unsigned int port, const char* bindaddr, unsigned int localport);		
	SMTPProxy();
	virtual ~SMTPProxy();
	BOOL SetSignature(char *sig);
	
private:	
	static void AddToEnd(char* buff, char* b, char* sig);
	static int InsertTextSignature(char*,char*);
	static BOOL FoundBoundary(char *boundary, char *buff);
	static BOOL IsEndOfData(char*);
	static char * MakeUpper(char * );
	static BOOL IsDataCommand(char *);
	static BOOL Is220Response(char *);
	DWORD SmtpProxyThreadMain();
	static DWORD SmtpProxyThread(DWORD arg);
	void StartProxyThread(SMTPProxy* m_pSMTPProxy);
	static DWORD SmtpClientThread(DWORD arg);
	void StartClientThread(SSmtp *pSSmtp);
	static DWORD SmtpDataThread(DWORD arg);
	static void StartDataThread(socket_pair *pspair);
	
};
