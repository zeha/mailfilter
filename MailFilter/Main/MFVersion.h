/*
 *   Change something in this file every new build (for date ...)
 *
 *
 */

#define MAILFILTER_VERSION_BETA

// don't touch below:

#ifndef _MAILFILTER_LICENSING_
#include "../Main/MailFilter.h"
#endif //_MAILFILTER_LICENSING_

#undef MAILFILTERVERNUM 

#undef MAILFILTERVER_MAJOR 
#define MAILFILTERVER_MAJOR "1"
#undef MAILFILTERVER_MINOR 
#define MAILFILTERVER_MINOR "5"
#undef MAILFILTERVER_BUILD
#define MAILFILTERVER_BUILD "1134"

#undef MAILFILTERCOMPILED 
#define MAILFILTERCOMPILED __DATE__

#define MAILFILTERVERNUM MAILFILTERVER_MAJOR"."MAILFILTERVER_MINOR"-"MAILFILTERVER_BUILD""

