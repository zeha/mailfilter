/****************************************************************************
**	File:	MFNRM.H
**
**	Desc:	MailFilter Remote Manager Plug-In
**
**  (C) Copyright 2001-2003 Christian Hofstädtler. All Rights Reserved.
**							
*/

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

#define NLM_FALSE 0
#define NLM_TRUE (-1)
#define NLM_SUCCESS 0


#include <stdlib.h>		/* exit()	*/
#include <signal.h>		/* SIGTERM, signal() */
#include <stdio.h>		/* sprintf() */
#include <ctype.h>		/* isspace() */
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <unistd.h>		/* F_OK */


#define NW_MAX_PORTABILITY
#include <nwadv.h>		/* AllocatResourceTag() */
#include <nwfile.h>
#include <nwtime.h>
#include <nwstring.h>
#include <nwconio.h>  	/* CreateScreen(), DestroyScreen() */
#include <nwthread.h>	/* GetNLMHandle() */
#include <nit\nwenvrn.h>
#include <nwfattr.h>    // for _MA_SERVER
#include <nwmisc.h>
#include <nwdsdc.h>


#include <httpexp.h>
extern "C" {
#include <pexports.h>
}


/* not to exceed 35 chars*/
#define MAILFILTER_NRM_HEADER "MailFilter/ax"
#define MAILFILTER_NRM_SERVICETAG_STATUS 		"MailFilterStatus"
#define MAILFILTER_NRM_SERVICETAG_STATUS_LEN 16
//(sizeof(MAILFILTER_NRM_SERVICETAG_STATUS))
#define MAILFILTER_NRM_SERVICETAG_RESTORE 		"MailFilterRestore"
#define MAILFILTER_NRM_SERVICETAG_RESTORE_LEN 17
//(sizeof(MAILFILTER_NRM_SERVICETAG_RESTORE))

/* -*- eof -*- */
