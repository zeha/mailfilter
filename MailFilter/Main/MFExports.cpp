/*+
 +		MFExports.cpp
 +		
 +		MailFilter Export Functions
 +		
 +		Copyright 2001-2002 Christian Hofstädtler.
 +		
 +		
 +		- Oct/2001 ; ch   ; Initial Code
 +		
 +		
+*/

#define _MFD_MODULE			"MFEXPORTS.CPP"
#include "MailFilter.h"

#if defined(N_PLAT_NLM) && (!defined(__NOVELL_LIBC__))
#define MFAPI_EXPORT_FUNC_DECLARE	int handlerThreadGroupID;
#define MFAPI_EXPORT_FUNC_BEGIN		++MFT_NLM_ThreadCount; handlerThreadGroupID = GetThreadGroupID(); SetThreadGroupID(mainThread_ThreadGroupID);
#define MFAPI_EXPORT_FUNC_END		SetThreadGroupID(handlerThreadGroupID);	MFT_NLM_ThreadCount--;
#else
#define MFAPI_EXPORT_FUNC_DECLARE
#define MFAPI_EXPORT_FUNC_BEGIN
#define MFAPI_EXPORT_FUNC_END
#endif

#ifdef __cplusplus
extern "C" {
#endif

long MailFilter_API_GetStats(int statID, int statSubID)
{
	MFAPI_EXPORT_FUNC_DECLARE;
	long ret =-1;

	MFAPI_EXPORT_FUNC_BEGIN;

	switch (statID)
	{
		case 0:
		{
			return (long)clock()/CLOCKS_PER_SEC;
			break;
		}
		case 1:
		{
			switch (statSubID)
			{
				case 1: {
					ret = (long)MFS_MF_MailsInputTotal;
					break;	}
				case 2: {
					ret = (long)MFS_MF_MailsInputFailed;
					break;	}
			}
			break;
		}
		case 2:
		{
			switch (statSubID)
			{
				case 1: {
					ret = (long)MFS_MF_MailsOutputTotal;
					break;	}
				case 2: {
					ret = (long)MFS_MF_MailsOutputFailed;
					break;	}
			}
			break;
		}
	}

	MFAPI_EXPORT_FUNC_END;

	return ret;
}

//
// Checks the passed String for block ...
//
// * inAttachmentName: Original attachment name
// * mailSource
// * matchfield
//
// > Returns filterNum+1 for "INFECTED" ;)
// > Returns -1: Invalid Parameters
//
long MailFilter_API_RuleCheck( char *szScan , unsigned int mailSource , unsigned int matchfield )
{
	MFAPI_EXPORT_FUNC_DECLARE;
	long ret =-1;
	long rc;
	
	MFAPI_EXPORT_FUNC_BEGIN;
	
	if (szScan == NULL)
	{	
		MFAPI_EXPORT_FUNC_END;
		return -1;
	}
		
	if (mailSource > 1)
	{
		MFAPI_EXPORT_FUNC_END;
		return -1;
	}
	
	if (matchfield > MAILFILTER_MATCHFIELD_SIZE)
	{
		MFAPI_EXPORT_FUNC_END;
		return -1;
	}

	rc = (long)MFAPI_FilterCheck( szScan , (int)mailSource , (int)matchfield );

	MFAPI_EXPORT_FUNC_END;
	
	return rc;
	
}

#ifdef __cplusplus
}
#endif

/*+
  +
  +
  + -- eof --
  +
  +*/