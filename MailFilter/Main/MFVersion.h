/*
 *   Change something in this file every new build (for date ...)
 *
 * Be warned: changing something in here will recompile everything!
 *
 */

//#define MAILFILTER_VERSION_BETA	"001"

#ifndef _MAILFILTER_LICENSING_
#include "../Main/MailFilter.h"
#endif //_MAILFILTER_LICENSING_

#undef MAILFILTERVERNUM 

#undef MAILFILTERVER_MAJOR 
#define MAILFILTERVER_MAJOR "1"
#undef MAILFILTERVER_MINOR 
#define MAILFILTERVER_MINOR "5"
#undef MAILFILTERCOMPILED 
#define MAILFILTERCOMPILED __DATE__
#undef MAILFILTERVER_BUILD
#define MAILFILTERVER_BUILD "1127"

#ifndef MAILFITLER_VERSION_BETA
#define MAILFILTERVERNUM MAILFILTERVER_MAJOR"."MAILFILTERVER_MINOR"["MAILFILTERVER_BUILD"]"
#else
#define MAILFILTERVERNUM MAILFILTERVER_MAJOR"."MAILFILTERVER_MINOR"["MAILFILTERVER_BUILD"]-d"MAILFITLER_VERSION_BETA
#endif

