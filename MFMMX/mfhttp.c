/*
 *
 *	mfhttp.c
 *
 *  Christian Hofstädtler's MailFilter/ax
 *
 *  MFMMX.NLM - MailFilter/ax mmx.myagent Integration Server
 *
 *  (C) Copyright 2002 Christian Hofstädtler
 *
 *
 *  ch  ;  Sep/2002 ; Initial Code
 *
 *
 */

#include <ix.h>
#include <socket.h>
#include <stdio.h>
#include <screen.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <timeval.h>
#include <memory.h>
#include <unistd.h>
#include <select.h>
#include <nks/thread.h>
#include <netinet/in.h>
#include <errno.h>

extern int MFMMX_ThreadCount;
extern int MFMMX_Exiting;
extern unsigned short MFMMX_Port;
extern char MFMMX_IPV4[17];

#define bufLen 4000

#define MAILFILTER_MATCHFIELD_NONE					0x0000
#define MAILFILTER_MATCHFIELD_ATTACHMENT			0x0001
#define MAILFILTER_MATCHFIELD_EMAIL					0x0002
#define MAILFILTER_MATCHFIELD_SUBJECT				0x0004
#define MAILFILTER_MATCHFIELD_SIZE					0x0008

static int 				MFMMX_ThreadCountMax = 10;
static int				MFMMX_ListenSocket = -1;

typedef struct
{
	NXThreadId_t	ThreadHandle;
	int				Socket;
	long			lastCmdTime;
} MFMMX_ThreadType;

static MFMMX_ThreadType MFMMX_Thread[11];

#define HTTP_MSG_OK					"HTTP/1.0 200 OK\r\nConnection: close\r\n\r\nOK"
#define HTTP_MSG_SYNTAXERROR		"HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\nBad Request"
#define HTTP_MSG_CHECKFAIL			"HTTP/1.0 500 FAIL\r\nConnection: close\r\n\r\nFAIL"

#define HTTP_CMD_NULL 0			// no command
#define HTTP_CMD_GET  1			// GET  command
#define HTTP_CMD_POST 2			// POST command

struct HTTP_CMD
{
	char	*cmd_name;
	int		cmd_code;
};
static struct HTTP_CMD	CmdTab[] =
{
	{ "GET",	HTTP_CMD_GET		},
	{ "POST",	HTTP_CMD_POST		},
	{ NULL,		HTTP_CMD_NULL		}
};


static int MFMMX_HTTP_SocketRecv(int threadID,char *buf,int len)
{

	int rc = 0;
	buf[0]=0;
	
	rc = recv(MFMMX_Thread[threadID].Socket,buf,(unsigned int)len,0);
	switch (rc)
	{
		case 0:
			return -1;
			break;
		default:
		{
			buf[rc]=0;
			return rc;
			break;
		}
	}
	
	return 0;
}

static void MFMMX_HTTP_SocketSend(int threadID, const char* format, ...)
{
	char buf[bufLen];
	
	va_list	argList;
	va_start(argList, format);

	vsprintf(buf, format, argList);
	send(MFMMX_Thread[threadID].Socket,buf,strlen(buf),NULL);

	va_end (argList);

}

static void MFMMX_HTTP_DispatchMessage(int threadID, const char* format, ...)
{
	char buf[bufLen];
	
	va_list	argList;
	va_start(argList, format);

	vsprintf(buf, format, argList);
	strcat(buf,"\r\n");
	MFMMX_HTTP_SocketSend(threadID,buf);

	va_end (argList);

}


#define MFMMX_HTTP_DispatchError MFMMX_HTTP_DispatchMessage
//		MFMMX_HTTP_DispatchError(threadID,"550 Syntax Error");
#define MFMMX_HTTP_DispatchOK(threadID) MFMMX_HTTP_DispatchMessage(threadID,HTTP_MSG_OK)

static int MFMMX_HTTP_DispatchCommandGET(int threadID, const char* curCmd)
{

	char szBuffer[bufLen];
	char szURL[bufLen];
	int iField;
	unsigned int urlLen;
	int iPos;
	int iBufferPos;
	int iMode = 0;
	IX_Bool bReturned = false;
	IX_Bool bThisCharDone = false;
	int iType = 0;		// 0 = nothing, 1 = incoming, 2 = outgoing
	int iTmp;

	char* nextline = strchr(curCmd,'\r');
	if (nextline == NULL)
		nextline = strchr(curCmd,'\n');
	
	if ((curCmd-nextline) > bufLen-2)
	{	strncpy(szURL,curCmd,bufLen-2);	szURL[bufLen-2]=0;	}
	else
		strncpy(szURL,curCmd,(unsigned int)(nextline-curCmd));
	
	// this is a workaround and makes things much easier ;-)
	strcat(szURL,"&");
	
	// get string length
	urlLen = strlen(szURL);
	
	// check if incoming
	if (memicmp(szURL,"get /chcki?",11) == 0)
		iType = 1;
	// or outgoing
	if (memicmp(szURL,"get /chcko?",11) == 0)
		iType = 2;
	
	// do we have a valid url?
	if (iType)
	{
		iField = 0;
		iBufferPos = 0;
		szBuffer[0] = 0;
		iMode = 1;		// filling in field
		
		// step trough the url
		for (iPos = 11; iPos < (urlLen+1); iPos++)
		{
			bThisCharDone = IX_FALSE;
			
			if (szURL[iPos] == 0 )
				break;
			if (szURL[iPos] == ' ' )
				szURL[iPos] = '&';
			
			if ((!bThisCharDone) && (iMode == 1))
			{
				bThisCharDone = IX_TRUE;
				// fieldname complete?
				if (szURL[iPos]=='=')
				{	// looks like...
					szBuffer[iBufferPos] = 0;
					// check what field this is...
					if (memicmp(szBuffer,"from",4) == 0)			iField = MAILFILTER_MATCHFIELD_EMAIL;
					if (memicmp(szBuffer,"to",2) == 0)				iField = MAILFILTER_MATCHFIELD_EMAIL;
					if (memicmp(szBuffer,"subject",7) == 0)			iField = MAILFILTER_MATCHFIELD_SUBJECT;
					if (memicmp(szBuffer,"attachment",10) == 0)		iField = MAILFILTER_MATCHFIELD_ATTACHMENT;
					szBuffer[0] = 0;
					iBufferPos = 0;
					
					iMode = 2;			// filling in value
				} else
					szBuffer[iBufferPos++] = szURL[iPos];
			}
			
			if ((!bThisCharDone) && (iMode == 2))
			{
				bThisCharDone = IX_TRUE;
				// content complete?
				if (szURL[iPos]=='&')
				{	// looks like
					szBuffer[iBufferPos] = 0;
					
					if (MailFilter_API_RuleCheck( szBuffer , iType-1 , iField ))
					{
						MFMMX_HTTP_DispatchError(threadID,HTTP_MSG_CHECKFAIL);
						bReturned = IX_TRUE;
						break;
					}
					
					szBuffer[0] = 0;
					iBufferPos = 0;
					
					iMode = 1;			// filling in field
				} else {
					// no... 
					bThisCharDone = IX_FALSE;
					
					// as this is content, we'll have to decode some things...
					if (szURL[iPos] == '+')		{	szBuffer[iBufferPos++] = ' '; bThisCharDone = IX_TRUE;	}
					if (szURL[iPos] == '%')
					{
						iPos++;
					    iTmp = (szURL[iPos] >= 'A' ? ((szURL[iPos] & 0xdf) - 'A')+10 : (szURL[iPos] - '0'));
					    iTmp *= 16;
					    iPos++;
					    iTmp += (szURL[iPos] >= 'A' ? ((szURL[iPos] & 0xdf) - 'A')+10 : (szURL[iPos] - '0'));

						if ((iTmp > 0) && (iTmp < 255))
						{
							szBuffer[iBufferPos++] = (char)iTmp;
							bThisCharDone = IX_TRUE;
						} else {
							iPos -= 2;
						}
					}
					
					// nothing to decode... just copy over char
					if (!bThisCharDone)			{	szBuffer[iBufferPos++] = szURL[iPos]; bThisCharDone = IX_TRUE;	}
				}
			}
		}
		
		// have we already sent a message back?
		if (!bReturned)
			MFMMX_HTTP_DispatchError(threadID,HTTP_MSG_OK);	//no, just send OK
			
	} else {
		// invalid url...send syntax error
		MFMMX_HTTP_DispatchError(threadID,HTTP_MSG_SYNTAXERROR);
	}

	return 0;
}

static int MFMMX_HTTP_DispatchCommand(int threadID, char* curCmd, char* curCmd2)
{
#pragma unused(curCmd2)

	int iCommand = HTTP_CMD_NULL;
	unsigned int iCCStrLen = strlen(curCmd);
	register struct HTTP_CMD *volatile c = NULL;
	int iCCDelimPos = 0;
	
	for (iCCDelimPos = 0;iCCDelimPos < iCCStrLen;iCCDelimPos++)
	{	if (curCmd[iCCDelimPos] == ' ') break;	}
	
	if (iCCDelimPos > 7)
	{
		MFMMX_HTTP_DispatchError(threadID,HTTP_MSG_SYNTAXERROR);
		return 1;
	}
	
	/* decode command */
	for (c = CmdTab; c->cmd_name != NULL; c++)
	{
		if (memcmp(c->cmd_name, curCmd,strlen(c->cmd_name)) == 0)
		{	iCommand = c->cmd_code;
			break;	}
	}

	MFMMX_Thread[threadID].lastCmdTime = time(NULL);

	switch(iCommand)
	{
		case HTTP_CMD_NULL:
			MFMMX_HTTP_DispatchError(threadID,HTTP_MSG_SYNTAXERROR);
			break;
			
		case HTTP_CMD_GET:
			MFMMX_HTTP_DispatchCommandGET(threadID,curCmd);
			break;
		
		case HTTP_CMD_POST:
			MFMMX_HTTP_DispatchError(threadID,HTTP_MSG_SYNTAXERROR);
			break;
		
		default:
			MFMMX_HTTP_DispatchError(threadID,HTTP_MSG_SYNTAXERROR);
			break;
	}

	return 1;	// Success and kill connection.
}

// 
// **** HTTP Thread: Incoming Connection Thread Code ****
//
#ifndef WIN32
void MFMMX_HttpChildStartup(void* in_threadID)
#else
DWORD WINAPI MFMMX_HttpChildStartup(void* in_threadID)
#endif // WIN32
{
	int threadID = (int)in_threadID;
	int shallTerminate = 0;
	char buf[bufLen];
	char curCmd[bufLen];
	char curCmd2[bufLen];
	int rc = 0;
	IX_Bool haveNewCommand = false;
	unsigned long lBlockingMode = 0;
	struct timeval timeWait;
	int cPos;
	int myConnSock = 0;
	fd_set fdsetSelect;
	
	MFMMX_ThreadCount++;

	timeWait.tv_sec = 3;
	timeWait.tv_usec = 0;
	curCmd[0] = 0;

	myConnSock = MFMMX_Thread[threadID].Socket;
	MFMMX_Thread[threadID].lastCmdTime = time(NULL) + 600;

	while (shallTerminate == 0)
	{

		FD_ZERO(&fdsetSelect);
		FD_SET(myConnSock,&fdsetSelect);

		rc = MFMMX_HTTP_SocketRecv(threadID,buf,bufLen);
		if (rc > 0)
		{
			strcat(curCmd,buf);
			for (cPos = 0;cPos < bufLen;cPos++)
			{
				if (curCmd[cPos] == 0)	break;
				if (curCmd[cPos] == '\n')
				{
					cPos++;
					if (curCmd[cPos] == '\r') cPos++;
					if (curCmd[cPos] == 0)	break;
					if (curCmd[cPos] == '\n')
					{
						strcpy(curCmd2,curCmd+cPos+1);
						curCmd[cPos]=0;
						haveNewCommand = true;
						break;
					}
					cPos--;
				}
			}
		}

		if (haveNewCommand)
		{
			// Handle Commands ...
			shallTerminate = MFMMX_HTTP_DispatchCommand(threadID,curCmd,curCmd2);
			//
		
			strncpy(curCmd,curCmd2,4000);
			haveNewCommand = false;
			curCmd2[0]=0;
		}
					
		if (rc < 0)
			shallTerminate = 1;
				
		// Timeout ... 5 Minutes.
		if ((MFMMX_Thread[threadID].lastCmdTime + (5*60)) < time(NULL))
		{
			shallTerminate = 1;
		}

#ifdef __NOVELL_LIBC__		
		NXThreadYield();
#endif
		
		if (MFMMX_Exiting)
			shallTerminate = MFMMX_Exiting;
	} 
	
	shutdown ( myConnSock , SHUT_RDWR );
	close ( myConnSock );
	
	MFMMX_ThreadCount--;

	// kill our info structure...
	MFMMX_Thread[threadID].ThreadHandle = NULL;
	MFMMX_Thread[threadID].Socket = NULL;

#ifdef WIN32
	return 0;
#endif
}

// 
// **** SMTP Thread: Main Thread Code ****
//
#ifndef WIN32
void MFMMX_HttpStartup(void *dummy)
#else
DWORD WINAPI MFMMX_HttpStartup(void *dummy)
#endif // WIN32
{
#pragma unused(dummy)

	int MFT_SMTP_ListenSocket = NULL;
	struct sockaddr_in saddr; // = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	struct sockaddr sa;
	IX_Bool haveFreeThread = false;
	unsigned int addrlen; // = 0; // = (unsigned int*)malloc(sizeof(unsigned int));
	struct in_addr inaddr;
	int curThread;
	char bSetSockOpt = FALSE;
	NXContext_t		*myContext;
	NXThreadId_t	myThread;
	int				err;
	unsigned int namelen;
	addrlen = sizeof(struct sockaddr);

	MFMMX_ThreadCount++;

#ifdef __NOVELL_LIBC__
	NXThreadYield();
#endif

	// Initialize Socket/Thread Mappings
	for (curThread = 0; curThread < MFMMX_ThreadCountMax; curThread++)
		memset(&MFMMX_Thread[curThread], 0, sizeof(MFMMX_ThreadType));
	
	// OK, Create Socket.	
	MFMMX_ListenSocket = socket(
			AF_INET,		/* Internet Protocol */
			SOCK_STREAM,	/* Stream Socket */
			IPPROTO_TCP		/* TCP/IP */
		);
	if (MFMMX_ListenSocket < 0)
	{
		perror("MFMMX: socket");
		MFMMX_ThreadCount--;
		return;
	}

	// Create Socket Bind Address
	memset ( &saddr , 0 , sizeof(struct sockaddr) );
	saddr.sin_family = AF_INET;
    saddr.sin_port = ntohs(MFMMX_Port);	// Convert Port Adress to Network Format
    inaddr.s_addr = (unsigned long)inet_addr( MFMMX_IPV4 );

	saddr.sin_addr = inaddr;

	bSetSockOpt = FALSE;
	setsockopt( MFMMX_ListenSocket , SOL_SOCKET , SO_LINGER , &bSetSockOpt , sizeof(IX_Bool) );
	
	// Bind Socket
	namelen = sizeof(struct sockaddr);
	memcpy(&sa,&saddr,sizeof(struct sockaddr));
	if ( bind( MFMMX_ListenSocket, &sa, namelen ) < 0 )
	{
		//MF_StatusText( "Error Binding ListenSocket." );
		perror("MFMMX: bind");
		MFMMX_ThreadCount--;
		return;
	}

	// Set Socket to Listen State
	if ( listen( MFMMX_ListenSocket , SOMAXCONN /*MFT_SMTP_Incoming_ThreadCountMax*/ ) != 0 )
	{
		//MF_StatusText( "Error Opening ListenSocket." );
		perror("MFMMX: listen");
		MFMMX_ThreadCount--;
		return;
	}
	
	
	// Get SockAddr.
	memcpy(&sa,&saddr,sizeof(struct sockaddr));
	getsockname( MFMMX_ListenSocket , &sa , &addrlen );
	inaddr = (struct in_addr)saddr.sin_addr;

	/* THREAD: Worker Part */
	do
	{
		haveFreeThread = false;
		for (curThread = 0; curThread < MFMMX_ThreadCountMax; curThread++)
		{
			if (
				(MFMMX_Thread[curThread].ThreadHandle == NULL) &&
				(MFMMX_Thread[curThread].Socket == NULL)
			   )
			   {
				   	haveFreeThread = true;
				   	break;
			   }
		}

		if (haveFreeThread)
		{
				addrlen = sizeof(struct sockaddr);
				 
				MFMMX_Thread[curThread].Socket = accept(
						MFMMX_ListenSocket,
						&sa,
						&addrlen
					);
					
				if (MFMMX_Thread[curThread].Socket > -1)
				{
#ifdef __NOVELL_LIBC__
					// Start Thread: ** SMTP INCOMING CONNECTION **
					myThread = NULL; myContext = NULL;

					NX_THREAD_CREATE(MFMMX_HttpChildStartup,NX_THR_DETACHED,(void*)curThread,myContext,myThread,err);

					// Create New Thread, and pass curThread to Http_ChildStartup
					if( myThread == NULL )
					{
						MFMMX_Thread[curThread].Socket = NULL;
						consoleprintf("MFMMX: Error Creating Thread!");
						perror("MFMMX: NX_THREAD_CREATE");
					} else {
						MFMMX_Thread[curThread].ThreadHandle = myThread;
						NXThreadYield();
					}
#endif
				} else {
					MFMMX_Thread[curThread].Socket = -1;
				}
		} else {
			NXThreadYield();
			delay(1000);
		}
	
		delay(20);
		
#ifdef __NOVELL_LIBC__
		NXThreadYield();
#endif

	} while (MFMMX_Exiting == IX_FALSE);

	/* THREAD: Shutdown */

	// Close Socket
	if (close( MFMMX_ListenSocket ) < 0)
	{
		//MF_StatusText( "Error closing ListenSocket." );
		perror("MFMMX: closesocket");
	}

	// Terminate
	MFMMX_ThreadCount--;

#ifdef WIN32
	return 0;
#endif
} // MFMMX_HttpStartup
