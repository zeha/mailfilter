// bah, crappy hack to get winsock2 headers instead of sys/socket.h
#define MAILFILTER_WITH_WINSOCK2
#include "MailFilter.h"

#include <synch.h>

#include "SMTPProxy.h"
#include "SMTPProxy.h++"


static cond_t condMainThread;

int MailFilter_Main_RunAppSmtpProxy(bool bStandalone)
{
	SMTPProxy proxy;


	if (bStandalone)
		printf ("MailFilter SMTP PROXY: Initializing...\n");

	// Rename thread
	NXContextSetName(NXContextGet(),"MailFilterSMTP");

	ThreadSwitch();

	if (bStandalone)
		RenameScreen(getscreenhandle(),"MailFilter SMTP PROXY");


	if (!
		proxy.StartProxy(
			MF_GlobalConfiguration->SMTPProxyBackendHost.c_str(),MF_GlobalConfiguration->SMTPProxyBackendPort,
			MF_GlobalConfiguration->SMTPProxyLocalHost.c_str(),MF_GlobalConfiguration->SMTPProxyLocalPort)
		)
	{
		printf("SMTP Proxy setup failed. Aborting Startup.\n");
		return -1;
	}
	
	ThreadSwitch();
	if (cond_init ( &condMainThread , USYNC_THREAD, NULL ) != 0)
	{
		printf("Could not initialize mutex. Startup Failed\n");
	}
	else 
	{
		if (bStandalone)
			printf("MailFilter SMTP PROXY Running.\n");
	
		mutex_t mtx;
		mutex_init(&mtx, USYNC_THREAD, NULL);
		mutex_lock(&mtx);
		cond_wait(&condMainThread,&mtx);
	}
   	
	ThreadSwitch();
	if (bStandalone)
		printf("Shutdown...\n");
	
	proxy.StopProxy();

	cond_signal(&condMainThread);
	cond_destroy(&condMainThread);
	
	return 0;	
}


SMTPProxy::SMTPProxy()
{
	AcceptFlag=false;	
}	

SMTPProxy::~SMTPProxy()
{

}

in_addr_t resolveHost(const char* host)
{
	struct hostent *hp;
	in_addr_t addr;
	printf("Resolving %s ...",host);

	addr.S_un.S_addr = inet_addr(host);
	if (addr.S_un.S_addr == INADDR_NONE)
	{
		hp=gethostbyname(host);

		if(hp!=NULL)
		{
			addr = *((in_addr_t*)hp->h_addr);
		}
		
		if (addr.S_un.S_addr == INADDR_NONE)
			printf("\n   ERROR: Configured Address is not resolvable!\n");
		else 
			printf("Done!\n");
	}
		
	return addr;
}

int SMTPProxy::StartProxy(const char *server, unsigned int port, const char* bindaddr, unsigned int localport)
{
	if(AcceptFlag)
		return ERR_RUNNING;

	strncpy(m_SSmtp.SmtpServer,server,249);
	strncpy(m_SSmtp.LocalIP,bindaddr,249);
	m_SSmtp.SmtpServer[249] = 0;
	m_SSmtp.LocalIP[249] = 0;
	m_SSmtp.SmtpPort=port;
	m_SSmtp.LocalPort=localport;
	AcceptFlag=true;
	
	m_SSmtp.SmtpServer_addr = resolveHost(m_SSmtp.SmtpServer);
	if (m_SSmtp.SmtpServer_addr.S_un.S_addr == INADDR_NONE)
		return ERR_HOST;
		
	m_SSmtp.LocalIP_addr = resolveHost(m_SSmtp.LocalIP);
	if (m_SSmtp.LocalIP_addr.S_un.S_addr == INADDR_NONE)
		return ERR_HOST;
	
	StartProxyThread(this);
	return 0;
}

void SMTPProxy::StartProxyThread(SMTPProxy* m_pSMTPProxy)
{	
	
	NXContext_t ctx;
	NXThreadId_t thr;
	
	NXThreadCreateSx ( 
		(void(*)(void*))SMTPProxy::SmtpProxyThread, 
		(void*)m_pSMTPProxy,
		NX_THR_JOINABLE, 
		&ctx, 
		&thr);		
/*	_beginthreadex((void*)&sa,0,
		(unsigned(__stdcall*)(void*))SMTPProxy::SmtpProxyThread,
		(void*)m_pSMTPProxy,0,(unsigned int*)&threadid);*/

}

DWORD SMTPProxy::SmtpProxyThread(DWORD arg)
{
	SMTPProxy* m_pSMTPProxy=static_cast<SMTPProxy*>((void*)arg);		
	return m_pSMTPProxy->SmtpProxyThreadMain();	
}

DWORD SMTPProxy::SmtpProxyThreadMain()
{
	WSADATA wsaData;
	sockaddr_in local;
	int wsaret=WSAStartup((WORD) MAKEWORD(2, 0),&wsaData);
	if(wsaret!=0)
	{
		printf("ERR_WSAFAIL\n");
		return ERR_WSAFAIL;
	}
	local.sin_family=AF_INET;
	local.sin_addr.s_addr=INADDR_ANY;
	if (this->m_SSmtp.LocalIP_addr.S_un.S_addr != INADDR_NONE)
		local.sin_addr.s_addr=this->m_SSmtp.LocalIP_addr.S_un.S_addr;
		
	local.sin_port=htons((u_short)this->m_SSmtp.LocalPort);
	SmtpServerSocket=socket(AF_INET,SOCK_STREAM,0);
	if(SmtpServerSocket==INVALID_SOCKET)
	{
		printf("ERR_INVALIDSOCKET\n");
		return ERR_INVALIDSOCKET;
	}
	if(bind(SmtpServerSocket,(sockaddr*)&local,sizeof(local))!=0)
	{
		printf("ERR_BIND\n");
		return ERR_BIND;
	}
	if(listen(SmtpServerSocket,10)!=0)
	{
		printf("ERR_LISTEN\n");
		return ERR_LISTEN;
	}

	SOCKET SmtpClientSocket;
	sockaddr_in from;
	int fromlen=sizeof(from);

	while(AcceptFlag)
	{
		char temp[250];
		SmtpClientSocket=accept(SmtpServerSocket,
			(struct sockaddr*)&from,&fromlen);
		if(!AcceptFlag)
			break;

		sprintf(temp,"%s",inet_ntoa(from.sin_addr));
		this->m_SSmtp.LocalSocket=SmtpClientSocket;
		this->m_SSmtp.pSMTPProxy=this;

		SSmtp *pSSmtp = new SSmtp;
		m_SSmtp.CopyStruct(pSSmtp);
		StartClientThread(pSSmtp);

	}

	closesocket(SmtpServerSocket);
	WSACleanup();

	return 0;
}

void SMTPProxy::SSmtp::CopyStruct(SSmtp *pSSmtp)
{
	strcpy(pSSmtp->ClientIP,ClientIP);
	pSSmtp->LocalPort=LocalPort;
	pSSmtp->LocalSocket=LocalSocket;
	pSSmtp->SmtpPort=SmtpPort;
	strcpy(pSSmtp->SmtpServer,SmtpServer);
	pSSmtp->pSMTPProxy=pSMTPProxy;

}
void SMTPProxy::StartClientThread(SSmtp *pSSmtp)
{

	NXContext_t ctx;
	NXThreadId_t thr;
	
	NXThreadCreateSx ( 
		(void(*)(void*))SMTPProxy::SmtpClientThread, 
		(void*)pSSmtp,
		NX_THR_JOINABLE, 
		&ctx, 
		&thr);		


/*	
	_beginthreadex((void*)&sa,0,
		(unsigned(__stdcall*)(void*))SMTPProxy::SmtpClientThread,
		(void*)pSSmtp,0,(unsigned int*)&threadid);*/
}

DWORD SMTPProxy::SmtpClientThread(DWORD arg)
{	
	SSmtp* pSSmtp=static_cast<SSmtp*>((void*)arg);
	SOCKET conn;
//	struct hostent *hp;
//	unsigned int addr;
	struct sockaddr_in smtp_server;
	conn=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

/*	if(inet_addr((char*)pSSmtp->SmtpServer)==INADDR_NONE)
	{
		hp=gethostbyname(pSSmtp->SmtpServer);
	}
	else
	{
		addr=inet_addr(pSSmtp->SmtpServer);
		hp=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
	}
	if(hp==NULL)
	{		
		closesocket(conn);
		closesocket(pSSmtp->LocalSocket);
		delete pSSmtp;
		printf("ERR_HOST\n");
		return ERR_HOST;
	} */
	
	smtp_server.sin_addr.s_addr=pSSmtp->SmtpServer_addr.S_un.S_addr; //*((unsigned long*)hp->h_addr);
	smtp_server.sin_family=AF_INET;
	smtp_server.sin_port=htons(pSSmtp->SmtpPort);

	if(connect(conn,(struct sockaddr*)&smtp_server,sizeof(smtp_server)))
	{
		closesocket(conn);
		closesocket(pSSmtp->LocalSocket);
		delete pSSmtp;
		printf("ERR_CONNECT\n");
		return ERR_CONNECT;
	}

	socket_pair *fromClient=new socket_pair;
	socket_pair *fromServer=new socket_pair;

	fromClient->src=fromServer->dest=pSSmtp->LocalSocket;
	fromClient->dest=fromServer->src=conn;
	fromClient->IsClient=true;
	fromServer->IsClient=false;
	fromServer->pSMTPProxy=fromClient->pSMTPProxy=pSSmtp->pSMTPProxy;
	StartDataThread(fromClient);
	StartDataThread(fromServer);

	delete pSSmtp;
	return 0;
}


void SMTPProxy::StartDataThread(socket_pair *pspair)
{

	NXContext_t ctx;
	NXThreadId_t thr;
	
	NXThreadCreateSx ( 
		(void(*)(void*))SMTPProxy::SmtpDataThread, 
		(void*)pspair,
		NX_THR_JOINABLE, 
		&ctx, 
		&thr);		

/*	
	_beginthreadex((void*)&sa,0,
		(unsigned(__stdcall*)(void*))SMTPProxy::SmtpDataThread,
		(void*)pspair,0,(unsigned int*)&threadid);*/
}

DWORD SMTPProxy::SmtpDataThread(DWORD arg)
{
	socket_pair* pspair=static_cast<socket_pair*>((void*)arg);
	char *buff=new char[10240+5120];
	int n;
	int data_stage=0;
	BOOL ExistsBoundary=false;
	char *boundary=new char[512];
	BOOL alternate=false;
	BOOL mixed=false;
	while((n=recv(pspair->src,buff,10240,0))>0)
	{		
		if(pspair->IsClient)
		{
			buff[n]=0;
			if(data_stage==3)
			{
				char *tmpbuff=new char[10240+5120];
				strcpy(tmpbuff,buff);
				if(strstr(MakeUpper(tmpbuff),"MAIL FROM:")!=NULL)
				{
					data_stage=0;
					ExistsBoundary=false;
					alternate=false;
					mixed=false;
				}
				delete tmpbuff;
			}
			if(data_stage==0)
				if(IsDataCommand(buff))
					data_stage=1;
			if(data_stage==1)
			{
				if(!alternate)
				{
					if(strstr(buff,"Content-Type: multipart/alternative")!=NULL)
						alternate=true;					
				}
				if(!mixed)
				{
					if(strstr(buff,"Content-Type: multipart/mixed")!=NULL)
						mixed=true;
				}				
				if(!ExistsBoundary)
					ExistsBoundary=FoundBoundary(boundary,buff);				
				if(ExistsBoundary)
				{
					if(alternate && !mixed)
					{
						char boundary1[512+64];
						strcpy(boundary1,"\r\n--");
						strcat(boundary1,boundary);						
						unsigned int i1=strlen(buff);
						AddToEnd(buff, boundary1,pspair->pSMTPProxy->ProxySignature);
						n+=(strlen(buff)-i1);
					}
					else
					{
						char boundary1[512+64];
						strcpy(boundary1,"\r\n--");
						strcat(boundary1,boundary);
						strcat(boundary1,"--\r\n");
						char *p1;
						char *p2;
						char *buff2=new char[10240+5120];
						char *buff3=new char[10240+5120];
						strcpy(buff2,buff);
						strcpy(buff3,buff);
						if((p1=strstr(buff2,boundary1))!=NULL)
						{
							
							char sig[2048+512+512+128];
							strcpy(sig,"\r\n--");
							strcat(sig,boundary);
							strcat(sig,"\r\n");
							strcat(sig,"Content-Type: text/plain;\r\n\r\n");
							strcat(sig,pspair->pSMTPProxy->ProxySignature);
							strcat(sig,"\r\n--");
							strcat(sig,boundary);
							strcat(sig,"\r\n\r\n");												
							p2=strstr(buff3,boundary1);								
							*p1=0;						
							
							strcat(buff2,sig);
							strcat(buff2,p2);
							n+=(strlen(buff2)-strlen(buff));
							strcpy(buff,buff2);
						}
						delete buff2;
						delete buff3;
					}
				}
				if(IsEndOfData(buff))
					data_stage=2;
			}
			if(data_stage==2)
			{
				if(!ExistsBoundary)
				{					
					n+=InsertTextSignature(buff,pspair->pSMTPProxy->ProxySignature);
				}				
				data_stage=3;
			}
			
		}
		if(!pspair->IsClient)
		{
			/* Is Server */
			
			/* override Response for 220 */
			if (MF_GlobalConfiguration->SMTPProxyResponse220 != "")
			{
				if (n>4)
				{	bool bModified = false;
					char szCmp[5];
					for(int i=0;i<3;++i)
					{
						szCmp[i]=toupper(buff[i]);
					}
					szCmp[3] = '\0';

					if (szCmp[0] == '2')
					{
						if (szCmp[1] == '2')
						{
							if (szCmp[2] == '0')
							{
								sprintf(buff,"220 %s\r\n",MF_GlobalConfiguration->SMTPProxyResponse220.c_str());
								bModified = true;
							}
							if (szCmp[2] == '1')
							{
								strcpy(buff,"221 Closing transmission channel\r\n");
								bModified = true;
							}
						}
					}
					// reset 'n'
					if (bModified)
						n=strlen(buff);
				}
			}
		}
		send(pspair->dest,buff,n,0);
	}
	shutdown(pspair->src,SD_BOTH);
	closesocket(pspair->src);
	shutdown(pspair->dest,SD_BOTH);
	closesocket(pspair->dest);
	delete boundary;
	delete buff;
	delete pspair;
	return 0;
}

BOOL SMTPProxy::IsDataCommand(char *str)
{
	char str1[11];
	BOOL rv;
	strncpy(str1,str,9);
	str1[9] = '\0';
	if(strcmp(MakeUpper(str1),"DATA\r\n")==0)	
		rv= true;
	else
		rv= false;
	return rv;
}

BOOL SMTPProxy::Is220Response(char *str)
{
	char str1[5];
	BOOL rv;
	strncpy(str1,str,4);
	str1[4] = '\0';
	if(strcmp(MakeUpper(str1),"220")==0)	
		rv= true;
	else
		rv= false;
	return rv;
}

char* SMTPProxy::MakeUpper(char* str)
{	
	char* tmp=str;
	while(*tmp)
	{
		if(islower(*tmp))
			*tmp=toupper(*tmp);
		tmp++;
	}
	return str;
}

BOOL SMTPProxy::IsEndOfData(char *str)
{
	if(strstr(str,"\r\n.\r\n")!=NULL)
		return true;
	else 
		return false;
}

BOOL SMTPProxy::FoundBoundary(char *boundary, char *buff)
{
	char *buff1=new char[10240+5120];
	strcpy(buff1,buff);
	char *tmp=strstr(buff1,"boundary=\"");
	if(tmp==NULL)
	{
		delete buff1;
		return false;
	}
	tmp+=10;
	char *tmp1=strstr(tmp,"\"\r\n");
	if(tmp1==NULL)
	{
		delete buff1;
		return false;
	}
	*tmp1=0;
	strcpy(boundary,tmp);
	delete buff1;
	return true;
}

int SMTPProxy::InsertTextSignature(char *buff, char* sig)
{
	char *tmp=strstr(buff,"\r\n.\r\n");
	*tmp=0;
	strcat(buff,sig);
	strcat(buff,"\r\n.\r\n");
	return strlen(sig);
}


BOOL SMTPProxy::SetSignature(char *sig)
{
	if(strlen(sig)>2047)
		return false;
	strcpy(ProxySignature,sig);
	return true;
}


void SMTPProxy::AddToEnd(char *buff, char *b, char* sig)
{
	char *newbuff1=new char[10240+5120];
	char *newbuff2=new char[10240+5120];
	strcpy(newbuff1,buff);	
	char *p1;
	int skip=1;
	for(int i=0;i<=1;i++)
	{
		p1=newbuff1;
		for(int j=0;j<skip;j++)
		{
			p1=strstr(p1,b);
			if(p1==NULL)
			{
				delete newbuff1;
				delete newbuff2;
				return;
			}
			p1++;
		}
		p1=strstr(p1,b);
		if(p1==NULL)
		{
			delete newbuff1;
			delete newbuff2;
			return;
		}
		strcpy(newbuff2,p1);
		*p1=0;
		strcat(newbuff1,sig);
		strcat(newbuff1,newbuff2);		
		skip++;
	}

	strcpy(buff,newbuff1);

	delete newbuff1;
	delete newbuff2;
}

int SMTPProxy::StopProxy()
{
	if(!AcceptFlag)
	{
		printf("ERR_STOPPED\n");
		return ERR_STOPPED;
	}
	AcceptFlag=false;
	closesocket(SmtpServerSocket);
	return OK_SUCCESS;
}
