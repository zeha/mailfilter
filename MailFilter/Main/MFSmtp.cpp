/*+
 +		MFSmtp.cpp
 +		
 +		SMTP Thread Code
 +		
 +		Copyright 2001 Christian Hofstädtler
 +		
 +		
 +		- Aug/2001 ; ch   ; Initial Code
 +		
 +		
+*/

//#ifdef __TEST

#define _MFD_MODULE			"MFSMTP.CPP"
#include "MailFilter.h"
//#include <list>
//using namespace std ;

static unsigned int 	MFT_SMTP_SleepTimer = 1500;
static int 				MFT_SMTP_Incoming_ThreadCountMax = 10;
// Incoming Thread/Socket Mappings
//static HANDLE 			MFT_SMTP_Incoming_ThreadID[10];
//static SOCKET			MFT_SMTP_Incoming_Socket[10];

struct MF_SMTP_T
{
	HANDLE			ThreadHandle;
	SOCKET			Socket;
//	char			curCmd[4000];
//	char			curCmd2[4000];
	unsigned long	lastCmdTime;
	int				lastCmdSuccess;
	int				haveCmdMail;	/* either 0 or 1 */
	int				haveCmdRcpt;	/* either 0 or number of rcpt commands received */
	int				haveCmdData;	/* either 0 or 1 */
	int				haveData;		/* either 0 or lines of data received */
	MailFilter_MailData* mailData;
};
/*typedef list<MF_SMTP_T> _MF_SMTP_Threads;
static _MF_SMTP_Threads MF_SMTP_Threads;
*/
#define SMTP_MSG_OK					"250 Ok"
#define SMTP_MSG_SYNTAXERROR		"500 Syntax Error"
#define SMTP_MSG_INITCONN			"220 MailFilter/ax SMTP. Ready."
#define SMTP_MSG_TERMCONN			"221 Closing transmission channel."

static MF_SMTP_T MFT_SMTP_IncomingThread[10];
#define SMTPData MFT_SMTP_IncomingThread[threadID]

#define SMTP_CMD_NULL 0			// no command
#define SMTP_CMD_QUIT 1			// QUIT command
#define SMTP_CMD_RSET 2			// RSET command
#define SMTP_CMD_MAIL 3			// MAIL (FROM) command
#define SMTP_CMD_RCPT 4			// RCPT (TO) command
#define SMTP_CMD_VRFY 5			// VRFY command
#define SMTP_CMD_EXPN 6			// EXPN command
#define SMTP_CMD_DATA 7			// DATA command
#define SMTP_CMD_HELP 8			// HELP command
#define SMTP_CMD_NOOP 9			// NOOP command

struct SMTP_CMD
{
	char	*cmd_name;
	int		cmd_code;
};
static struct SMTP_CMD	CmdTab[] =
{
	{ "MAIL",	SMTP_CMD_MAIL		},
	{ "RCPT",	SMTP_CMD_RCPT		},
	{ "DATA",	SMTP_CMD_DATA		},
	{ "RSET",	SMTP_CMD_RSET		},
	{ "VRFY",	SMTP_CMD_VRFY		},
	{ "EXPN",	SMTP_CMD_EXPN		},
	{ "HELP",	SMTP_CMD_HELP		},	
	{ "NOOP",	SMTP_CMD_NOOP		},
	{ "QUIT",	SMTP_CMD_QUIT		},
/*	{ "helo",	CMDHELO		},
	{ "ehlo",	CMDEHLO		},
	{ "etrn",	CMDETRN		},
	{ "verb",	CMDVERB		},
	{ "send",	CMDUNIMPL	},
	{ "saml",	CMDUNIMPL	},
	{ "soml",	CMDUNIMPL	},
	{ "turn",	CMDUNIMPL	},	*/
	{ NULL,		SMTP_CMD_NULL		}
};




#ifdef HAVE_WINSOCK
// Helper function to start WSA and associates ...
bool MF_SMTP_WSA_Startup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	 
	wVersionRequested = MAKEWORD( 2, 2 );
	 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
	    /* Tell the user that we could not find a usable */
	    /* WinSock DLL.                                  */
	    return false;
	}
	 
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	 
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
	        HIBYTE( wsaData.wVersion ) != 2 ) {
	    /* Tell the user that we could not find a usable */
	    /* WinSock DLL.                                  */
	    WSACleanup( );
	    return false; 
	}
	return true;
}

// Helper function to shut down WSA and associates ...
bool MF_SMTP_WSA_Shutdown()
{
	WSACleanup( );
	return true;
}
#else
#define MF_SMTP_WSA_Startup	true
#define MF_SMTP_WSA_Shutdown	true
#endif /* HAVE_WINSOCK */

static int MF_SMTP_SocketRecv(int threadID,char FAR *buf,int len)
{

	int rc = 0;
	buf[0]=0;
	
	rc = recv(MFT_SMTP_IncomingThread[threadID].Socket,buf,len,0);
	switch (rc)
	{
		case SOCKET_ERROR:
		{
			rc = WSAGetLastError();
			switch(rc)
			{
			case WSAEWOULDBLOCK:
				return 0;
				break;
			default:
				return -2; 
			}
			break;
		}
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

static void MF_SMTP_SocketSend(int threadID, const char* format, ...)
{
	char buf[4000];
	
	va_list	argList;
	va_start(argList, format);

	vsprintf(buf, format, argList);
	send(MFT_SMTP_IncomingThread[threadID].Socket,buf,(int)strlen(buf),NULL);

	va_end (argList);

}
static void MF_SMTP_DispatchMessage(int threadID, const char* format, ...)
{
	char buf[4000];
	
	va_list	argList;
	va_start(argList, format);

	vsprintf(buf, format, argList);
	strcat(buf,"\r\n");
	MF_SMTP_SocketSend(threadID,buf);

	va_end (argList);

}
#define MF_SMTP_DispatchError MF_SMTP_DispatchMessage
//		MF_SMTP_DispatchError(threadID,"550 Syntax Error");
#define MF_SMTP_DispatchOK(threadID) MF_SMTP_DispatchMessage(threadID,SMTP_MSG_OK)

static int MF_SMTP_DispatchCommandMAIL(int threadID, char* curCmd, char* curCmd2)
{
	threadID = threadID;
	curCmd = curCmd;
	curCmd2 = curCmd2;
	
	SMTPData.haveCmdMail = true;
//	MFD_Out("cc: %s, cc2: %s\n",curCmd,curCmd2);
//	SMTPData.mailData->szMailFrom
	
	return 0;
}
static int MF_SMTP_DispatchCommandRCPT(int threadID, char* curCmd, char* curCmd2)
{
	threadID = threadID;
	curCmd = curCmd;
	curCmd2 = curCmd2;
	return 0;
}
static int MF_SMTP_DispatchCommandDATA(int threadID, char* curCmd, char* curCmd2)
{
	threadID = threadID;
	curCmd = curCmd;
	curCmd2 = curCmd2;
	return 0;
}

static int MF_SMTP_DispatchCommand(int threadID, char* curCmd, char* curCmd2)
{
	curCmd = curCmd;
	curCmd2 = curCmd2;

	int iCommand = SMTP_CMD_NULL;
	unsigned int iCCStrLen = strlen(curCmd);
	register struct SMTP_CMD *volatile c = NULL;
	int iCCDelimPos = 0;
	
	for (iCCDelimPos = 0;iCCDelimPos < iCCStrLen;iCCDelimPos++)
	{	if (curCmd[iCCDelimPos] == ' ') break;	}
	
	if (iCCDelimPos > 7)
	{
		MF_SMTP_DispatchError(threadID,SMTP_MSG_SYNTAXERROR);
		return 0;
	}
	
	/* decode command */
	for (c = CmdTab; c->cmd_name != NULL; c++)
	{
		if (memicmp(c->cmd_name, curCmd,strlen(c->cmd_name)) == 0)
		{	iCommand = c->cmd_code;
			break;	}
	}
	MFD_Out(MFD_SOURCE_SMTP,"SMTP[%d]: cmd %d, '%s'\n",threadID,iCommand,curCmd);
	MFT_SMTP_IncomingThread[threadID].lastCmdTime = time(NULL);

	switch(iCommand)
	{
		case SMTP_CMD_NULL:
			MF_SMTP_DispatchError(threadID,"502 Error: Not Implemented");
			break;
			
		case SMTP_CMD_QUIT:
			// Return 1 to quit session by MailFilter/ax.
			// The SMTP Thread will kill the socket itself.
			return 1;
		
		case SMTP_CMD_VRFY:
			MF_SMTP_DispatchError(threadID,"550 Unable to verify");
			break;
		
		case SMTP_CMD_EXPN:
			MF_SMTP_DispatchError(threadID,SMTP_MSG_SYNTAXERROR);
			break;
		
		case SMTP_CMD_NOOP:
			MF_SMTP_DispatchOK(threadID);
			break;
			
		case SMTP_CMD_MAIL:
			MF_SMTP_DispatchCommandMAIL(threadID,curCmd,curCmd2);
			break;
			
		case SMTP_CMD_RCPT:
			MF_SMTP_DispatchCommandRCPT(threadID,curCmd,curCmd2);
			break;
			
		case SMTP_CMD_DATA:
			MF_SMTP_DispatchCommandDATA(threadID,curCmd,curCmd2);
			break;
			
		case SMTP_CMD_RSET:
			MailFilter_MailDestroy(SMTPData.mailData);
			SMTPData.mailData = MailFilter_MailInit(NULL,0);
	//		MF_SMTP_DispatchError(threadID,SMTP_MSG_SYNTAXERROR);
			MF_SMTP_DispatchOK(threadID);
			break;
			
		case SMTP_CMD_HELP:
			MF_SMTP_DispatchError(threadID,"502 Error: No Help Available");
			break;
			
		default:
			MF_SMTP_DispatchError(threadID,SMTP_MSG_SYNTAXERROR);
			break;
	}

	return 0;	// Success.
}

// 
// **** SMTP Thread: Incoming Connection Thread Code ****
//
#ifndef WIN32
void MF_SMTP_Incoming_Startup(void* in_threadID)
#else
DWORD WINAPI MF_SMTP_Incoming_Startup(void* in_threadID)
#endif // WIN32
{
	MFT_NLM_ThreadCount++;

	int threadID = (int)in_threadID;
	int shallTerminate = 0;
#define bufLen 4000
	char buf[bufLen];
#define curCmdLen 4000
	char curCmd[curCmdLen];
	char curCmd2[curCmdLen];
	int rc = 0;
	bool haveNewCommand = false;
	struct timeval timeWait;
	fd_set fdsetSelect;
	timeWait.tv_sec = 3;
	timeWait.tv_usec = 0;

	SOCKET myConnSock = MFT_SMTP_IncomingThread[threadID].Socket;
	MF_StatusText("Incoming SMTP Connection accepted.");

	MFD_Out(MFD_SOURCE_SMTP,"SMTP[%d]: Accepting Connection...\n",threadID);
	MFT_SMTP_IncomingThread[threadID].lastCmdTime = time(NULL) + 600;

#ifdef HAVE_WINSOCK
	// Put socket into blocking mode ...
	unsigned long lBlockingMode = 0;
	ioctlsocket ( myConnSock , FIONBIO , &lBlockingMode );
#endif
	
#ifdef N_PLAT_NLM
#ifndef __NOVELL_LIBC__
	// Rename this Thread
	RenameThread(GetThreadID(),programMesgTable[THREADNAME_SMTPWORK]);
#endif
#endif

	MFT_SMTP_IncomingThread[threadID].Socket = myConnSock;
	MF_SMTP_DispatchMessage( threadID , SMTP_MSG_INITCONN );
	
	SMTPData.mailData = MailFilter_MailInit(NULL,0);
	
	while (shallTerminate == 0)
	{
		FD_ZERO(&fdsetSelect);
		FD_SET(myConnSock,&fdsetSelect);

		rc = select(FD_SETSIZE,&fdsetSelect,NULL,NULL,&timeWait);
		if (rc == SOCKET_ERROR)
			MFD_Out(MFD_SOURCE_SMTP,"SMTP[%d]: Select returned %d. %d\n",threadID,rc,WSAGetLastError());
		
		if (rc == 1)
		{
			
			rc = MF_SMTP_SocketRecv(threadID,buf,bufLen);
			if (rc > 0)
			{
				strcat(curCmd,buf);
				for (int cPos = 0;cPos < curCmdLen;cPos++)
				{
					if (curCmd[cPos] == 0)	break;
					
					// strip out \r
					if (curCmd[cPos] == '\r')
						curCmd[cPos] = 0;
						
					// command complete
					if (curCmd[cPos] == '\n')
					{
						strcpy(curCmd2,curCmd+cPos+1);
						curCmd[cPos]=0;
						haveNewCommand = true;
						break;
					}
				}
				if (haveNewCommand)
					MFD_Out(MFD_SOURCE_SMTP,"SMTP[%d]: Command: %s\n",threadID,curCmd);
			}

			if (haveNewCommand)
			{
				// Handle Commands ...
				shallTerminate = MF_SMTP_DispatchCommand(threadID,curCmd,curCmd2);
				//
			
				strncpy(curCmd,curCmd2,4000);
				haveNewCommand = false;
				curCmd2[0]=0;
			}
						
			if (rc < 0)
				shallTerminate = 1;
				
			
		}
		
		// Timeout ... 5 Minutes.
		if ((MFT_SMTP_IncomingThread[threadID].lastCmdTime + (5*60)) < time(NULL))
		{
			MFD_Out(MFD_SOURCE_SMTP,"SMTP[%d]: Timeout!\n",threadID);
			shallTerminate = 1;
		}

#ifdef N_PLAT_NLM		
		ThreadSwitch();
#endif
		
		if (MFT_NLM_Exiting)
			shallTerminate = MFT_NLM_Exiting;
	} 
	
	MF_SMTP_DispatchMessage( threadID , SMTP_MSG_TERMCONN );
	
#ifdef HAVE_WINSOCK
	shutdown ( myConnSock , SD_SEND );
#else
	shutdown ( myConnSock , SHUT_RDWR );
#endif
	closesocket ( myConnSock );
	
	MF_StatusText("Closing SMTP Connection.");
	
	MFT_NLM_ThreadCount--;
	
#ifdef WIN32
	return 0;
#endif
}

// 
// **** SMTP Thread: Main Thread Code ****
//
#ifndef WIN32
void MF_SMTP_Startup(void *dummy)
#else
DWORD WINAPI MF_SMTP_Startup(void *dummy)
#endif // WIN32
{
	MFT_NLM_ThreadCount++;


	SOCKET MFT_SMTP_ListenSocket = NULL;
	sockaddr_in *saddr = (sockaddr_in*)malloc(sizeof(sockaddr_in));
	bool haveFreeThread = false;
	HANDLE newThread = NULL;
	int *addrlen = (int*)malloc(sizeof(int));
	*addrlen = sizeof(sockaddr);
	in_addr inaddr;
	int rc;
	
	dummy=dummy;	// Keep Compiler Quiet.

#ifdef N_PLAT_NLM
#ifndef __NOVELL_LIBC__
	// Rename this Thread
	RenameThread(GetThreadID(),programMesgTable[THREADNAME_SMTP]);
	// 
#endif
#endif
#ifdef HAVE_WINSOCK
	// WSA Startup
	if (!MF_SMTP_WSA_Startup ( ))
		MF_StatusText("Error Initializing WS2_32.");
#endif
#ifdef N_PLAT_NLM
	ThreadSwitch();
#endif

	// Initialize Socket/Thread Mappings
	int curThread;
	for (curThread = 0; curThread < MFT_SMTP_Incoming_ThreadCountMax; curThread++)
	{
		//MFT_SMTP_Incoming_ThreadID	[curThread] = NULL;
		//MFT_SMTP_Incoming_Socket	[curThread] = NULL;
		// = (MF_SMTP_T)NULL;
		memset(&MFT_SMTP_IncomingThread[curThread], 0, sizeof(MF_SMTP_T));
	}
	
	// OK, Create Socket.	
	MFT_SMTP_ListenSocket = socket(
			AF_INET,		/* Internet Protocol */
			SOCK_STREAM,	/* Stream Socket */
			IPPROTO_TCP		/* TCP/IP */
		);
	if (MFT_SMTP_ListenSocket == INVALID_SOCKET)
	{
		MF_StatusText( "SMTP: Error Creating ListenSocket." );
		ConsolePrintf( "MailFilter: Error: WSASocket returned %d!\n" , WSAGetLastError() );
	}

	// Create Socket Bind Address
	memset ( (void*)saddr , 0 , sizeof(SOCKADDR_IN) );
	saddr->sin_family = AF_INET;
    saddr->sin_port = ntohs(26);	// Convert Port Adress to Network Format
    inaddr.s_addr = inet_addr( "0.0.0.0" );

	saddr->sin_addr = inaddr;

	char bSetSockOpt = TRUE;
	setsockopt( MFT_SMTP_ListenSocket , SOL_SOCKET , SO_DONTLINGER , &bSetSockOpt , sizeof(BOOL) );
	
	// Bind Socket
	if ( bind( MFT_SMTP_ListenSocket, (sockaddr*)saddr, sizeof(sockaddr) ) == SOCKET_ERROR )
	{
		MF_StatusText( "Error Binding ListenSocket." );
		ConsolePrintf( "MailFilter: Error: bind returned %d!\n" , WSAGetLastError() );
	}

	// Set Socket to Listen State
	if ( listen( MFT_SMTP_ListenSocket , SOMAXCONN /*MFT_SMTP_Incoming_ThreadCountMax*/ ) != 0 )
	{
		MF_StatusText( "Error Opening ListenSocket." );
		ConsolePrintf( "MailFilter: Error: listen returned %d!\n" , WSAGetLastError() );
	}
	
	
	// Get SockAddr.
	getsockname( MFT_SMTP_ListenSocket , (sockaddr*)saddr , addrlen );
	inaddr = (in_addr)saddr->sin_addr;

#ifdef HAVE_WINSOCK
	// Put socket into nonblocking mode ...
	unsigned long lBlockingMode = 1;
	ioctlsocket ( MFT_SMTP_ListenSocket , FIONBIO , &lBlockingMode );
#endif	

	/* THREAD: Worker Part */
	do
	{
		haveFreeThread = false;
		for (curThread = 0; curThread < MFT_SMTP_Incoming_ThreadCountMax; curThread++)
		{
			if (
				(MFT_SMTP_IncomingThread[curThread].ThreadHandle == NULL) &&
				(MFT_SMTP_IncomingThread[curThread].Socket == NULL)
			   )
			   {
				   	haveFreeThread = true;
				   	break;
			   }
		}

		if (haveFreeThread)
		{
				*addrlen = sizeof(sockaddr);
				 
				// We just try to accept a WinSock Connection...
/*				MFT_SMTP_Incoming_Socket[curThread] = accept(
						MFT_SMTP_ListenSocket,
						(SOCKADDR*)saddr,
						addrlen
					);
*/					
				MFT_SMTP_IncomingThread[curThread].Socket = WSAAccept(
						MFT_SMTP_ListenSocket,
						(sockaddr*)saddr,
						addrlen,
						NULL,
						NULL
					);
					
					
				if (MFT_SMTP_IncomingThread[curThread].Socket != INVALID_SOCKET)
				{
#ifdef N_PLAT_NLM
					// Start Thread: ** SMTP INCOMING CONNECTION **
/*TODO DEBUG 					
					newThread = BeginThread(MF_SMTP_Incoming_Startup,NULL,16384,(void*)curThread);		// Set 16k Stack for new thread, and pass curThread to the thread function
					if( newThread == EFAILURE )
					{
//						ConsolePrintf(programMesgTable[CONMSG_MAIN_ERRTHREADSTART]);
						
						newThread = NULL;
						MFT_SMTP_IncomingThread[curThread].Socket = NULL;
						
						MF_StatusText("SMTP: Error Creating Thread!");
						
					} else {
						
						MFT_SMTP_IncomingThread[curThread].ThreadHandle = newThread;
					}*/
#endif
				
				} else {
				
					MFT_SMTP_IncomingThread[curThread].Socket = NULL;
					
					rc = WSAGetLastError();
					// Check for WouldBlock == no pending connections ...
					if (rc != WSAEWOULDBLOCK)
					{
						MF_StatusText( "SMTP: Error while accepting connection!" );
//						ConsolePrintf( "MailFilter: SMTP Error: accept returned %d!\n" , WSAGetLastError() );
					}					
				}
				

					
		} else {
			MF_StatusText("SMTP: Not enough threads to handle all connections !");
			delay(MFT_SMTP_SleepTimer);
		}
	
		delay(20);
		
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif

	} while (MFT_NLM_Exiting == FALSE);


	/* THREAD: Shutdown */

	// Free Address Buffer
	free(saddr);
	free(addrlen);
	
	// Close Socket
	if (closesocket( MFT_SMTP_ListenSocket ) == SOCKET_ERROR)
	{
		MF_StatusText( "Error closing ListenSocket." );
		ConsolePrintf( "MailFilter/ax: Error: closesocket returned %d!\n" , WSAGetLastError() );
	}

#ifdef HAVE_WINSOCK
	// Shutdown WSA
	MF_SMTP_WSA_Shutdown( );
#endif

	// Terminate
	MFT_NLM_ThreadCount--;

#ifdef WIN32
	return 0;
#endif
} // MF_SMTP_Startup

//#endif