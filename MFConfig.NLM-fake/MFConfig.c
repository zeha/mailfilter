/****************************************************************************
**	File:	MFCONFIG.C
**
**	Desc:	MailFilter Configuration FAKE NLM
**
**  (C) Copyright 2000-2004 Christian Hofstädtler. All Rights Reserved.
**							
*/

/****************************************************************************
**	Compiler setup.
*/
	/*------------------------------------------------------------------------
	**	ANSI
	*/
	#include <signal.h>		/* SIGTERM, signal() */
	#include <string.h>

	/*------------------------------------------------------------------------
	**	MFConfig
	*/
	#include "MFConfig.mlc"
	#include "MFConfig.mlh"

	/*------------------------------------------------------------------------
	**	NetWare
	*/
	#include <nwsnut.h>		/* NWSRestoreNut(), NWSPushList(), ...*/
	#include <time.h>
	#include <nwadv.h>		/* AllocatResourceTag() */
	#include <obsolete\process.h>	/* GetNLMHandle() */

	
/****************************************************************************
**	Global storage
*/
	NUTInfo *NLM_nutHandle 	=	NULL;
	int		NLM_screenID	=	(int)NULL;
	int		NLM_mainThreadGroupID;
	int		NLM_threadCnt;

int __init_environment( void *reserved )	
{
#pragma unused(reserved)
return 0;	
}
int __deinit_environment( void *reserved )	
{
#pragma unused(reserved)
	return 0;	
}



/****************************************************************************
** Program shut-down.
*/
void NLM_ShutDown(void)
	{

	/*------------------------------------------------------------------------
	**	Shut down NWSNUT.
	*/
	if(NLM_nutHandle != NULL)
		NWSRestoreNut(NLM_nutHandle);

	return;
	}

/****************************************************************************
** Program signal handler.
*/
void NLM_SignalHandler(int sig)
	{
	int handlerThreadGroupID;
	time_t t;

	switch(sig)
		{
  		/*---------------------------------------------------------------------
		**	SIGTERM is called when this NLM is unloaded via console command.
		*/
		case SIGTERM:

			/*------------------------------------------------------------------
			** Wait for main() to terminate.
			**
			** If main() has not terminateded within a 1/2 second, ungetch an
			**	escape key.  This will "trick" a blocking NWSList() or NWSMenu()
			**	function and wake it up. 
			*/
			handlerThreadGroupID = SetThreadGroupID(NLM_mainThreadGroupID);

			t=time(NULL);
			while(NLM_threadCnt != 0)
				{

				if(difftime(time(NULL), t) > 0.5)
					{
					ungetch(27);
					t=time(NULL);
					}

				ThreadSwitchWithDelay();
				}

			SetThreadGroupID(handlerThreadGroupID);
			break;

		}

	return;
	}

/****************************************************************************
** Program initialization.
*/
void NLM_Initialize(void)
	{
	long	cCode;
	int NLMHandle;
	LONG	tagID;
	int curItem=0;

	/*------------------------------------------------------------------------
	**	Register SIGTERM hanler to facilitate a console UNLOAD command.
	*/
	signal(SIGTERM, NLM_SignalHandler);

	/*------------------------------------------------------------------------
	**	Get misc. values.
	*/
	NLMHandle=(int)GetNLMHandle();
	NLM_mainThreadGroupID = GetThreadGroupID();				/* Needed by our SIGTERM handler   */

	NLM_screenID = GetCurrentScreen();

	/*------------------------------------------------------------------------
	**	Fire up NWSNUT on our screen.
	*/
	tagID=AllocateResourceTag(
		/*  NLMHandle			*/  (unsigned long)NLMHandle,
		/*	descriptionString	*/	(const unsigned char*)programMesgTable[PROGRAM_NAME],
		/*	resourceType		*/	AllocSignature
		);
	if(tagID == NULL)
		{
		ConsolePrintf("MFCONFIG: Error creating Resource Tag!\n");
		goto END;
		}

	cCode=(long)NWSInitializeNut(
		/*	utility				*/	PROGRAM_NAME,
		/*	version				*/	PROGRAM_VERSION,
		/*	headerType			*/	SMALL_HEADER,
		/*	compatibilityType	*/	NUT_REVISION_LEVEL,
		/*	messageTable		*/	(unsigned char**)programMesgTable,
		/*	helpScreens			*/	NULL,
		/*	screenID			*/	NLM_screenID,
		/*	resourceTag			*/	tagID,
		/*	handle				*/	&NLM_nutHandle
		);
	if(cCode != NULL)
		{
		ConsolePrintf("MFConfig: FATAL ERROR: NWSInitializeNut returned %d.\n",cCode);
		goto END;
		}


	/*------------------------------------------------------------------------
	**	Set our screen as current & active.
	*/
	DisplayScreen(NLM_screenID);

END:
	return;
	}

/****************************************************************************
** Program start.
*/

void main(void)
{

	++NLM_threadCnt;

	NLM_Initialize();
	
	NWSDisplayErrorText ( FAKEERRORMESSAGE , SEVERITY_FATAL , NLM_nutHandle , "" );
	
	NLM_ShutDown();
	
	--NLM_threadCnt;

	return;
}
