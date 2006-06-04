//
// MFLic.h  -  MailFilter Licensing Header/Include File
//
// Copyright 2001, 2002 Christian Hofstädtler.
//


#ifndef _MAILFILTER_LICENSING_
#define _MAILFILTER_LICENSING_

#define MAILFILTER_MC_M_VIRUSSCAN	0xFFFFFFFE
#define MAILFILTER_MC_M_USERREDIR	0xFFFFFFFD
#define MAILFILTER_MC_M_MAILCOPY	0xFFFFFFFB
#define MAILFILTER_MC_M_BWTHCNTRL	0xFFFFFFF7

#define MAILFILTER_OPSYS_NETWARE 0
#define MAILFILTER_OPSYS_LINUX 1
#define MAILFILTER_OPSYS_WIN32 2
#ifdef N_PLAT_NLM
#define MAILFILTER_OPSYS MAILFILTER_OPSYS_NETWARE
#endif
#ifdef __NOVELL_LIBC__
#define MAILFILTER_OPSYS MAILFILTER_OPSYS_NETWARE
#endif
#ifdef WIN32
#define MAILFILTER_OPSYS MAILFILTER_OPSYS_WIN32
#endif

#ifndef _MFLIC_CPP
extern int MFL_Certified;
extern int MFL_Init(int runLevel);


#endif // !_MFLIC_CPP

#ifndef _MAILFILTER_

#include "../Main/MFVersion.h"

union keyA_chkSumB_u
{
	char			str[4];
	unsigned int	sum;
} keyA_chkSumB;

#endif // _MAILFILTER_

#endif // _MAILFILTER_LICENSING_

