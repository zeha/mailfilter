/*
 *
 *	main.c
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
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <screen.h>
#include <nks/thread.h>
#include <socket.h>
#include <netware.h>

int MFMMX_ThreadCount = 0;
int MFMMX_Exiting = 0;
unsigned short MFMMX_Port = 24;
char MFMMX_IPV4[17]; // = NULL;

void MFMMX_HttpStartup(void *dummy);

int MFMMX_ParseCommandLine( int argc, char **argv )
{
	int c;
	int i;
	
	MFMMX_Port = 24;
	strcpy(MFMMX_IPV4,"0.0.0.0");
	
	ix_getopt_init();

	while (1)
	{
		c = ix_getopt(argc, argv, "?hpi");
		if (c == -1)
			break;
			
		switch(c)
		{
			case '?':
			case 'h':
				consoleprintf ("MFMMX: Usage:\n	%s -p port\n",argv[0]);
				return IX_FALSE;
				break;
			case 'p':
				if (ix_optind < argc)
					MFMMX_Port = (unsigned short)atoi(argv[ix_optind]);
					else
					return IX_FALSE;
				break;			
			case 'i':
				if (ix_optind < argc)
				{	strncpy(MFMMX_IPV4,argv[ix_optind]);
					for (i=0;i<17;i++)
						if(MFMMX_IPV4[i]==' ') {MFMMX_IPV4[i]=0;break;}
				}
					else
					return IX_FALSE;
				break;			
			default:
				break;
		}
	}

	return IX_TRUE;
}

int main
(
	int	argc,
	char	*argv[],
	char	*env[]
)
{
#pragma unused(env)

	consoleprintf ("MFMMX: This is a BETA version. Use at YOUR OWN RISK!\n");
	
	if (MFMMX_ParseCommandLine(argc,argv) == IX_FALSE)
	{
		consoleprintf ("MFMMX: Invalid Commandline!\n");
		return EXIT_FAILURE;
	}

/*	myContext = NULL; myThread = NULL;
	NX_THREAD_CREATE(MFMMX_HttpStartup,NX_THR_DETACHED,0,myContext,myThread,err)
	NXThreadYield();
	
	if (myThread != NULL)
	{
		consoleprintf("MFMMX: Startup Complete!\r\n");
	} else {
		consoleprintf("MFMMX: Thread Startup Error!\r\n");
	}
	//NX*/
	
	
	// Pass Control to HttpStartup()
	MFMMX_HttpStartup(NULL);
	
	return EXIT_SUCCESS;
}
