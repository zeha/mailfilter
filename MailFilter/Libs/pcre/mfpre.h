/* Copyright 2002 Christian Hofstädtler. */
/* MailFilter INTERNAL USE ONLY */

#undef _MAILFILTER_WITH_POSIX_REGEXP_H
#define _MAILFILTER_WITH_POSIX_REGEXP_H

#include "pcreposix.h"

/***** TODO ******/
// check REG_NEWLINE !!!

#define regexp regex_t
#define mf_regcomp(re,str)	regcomp(re,str,REG_ICASE)
// & REG_NEWLINE)
#define regcomp(re,str)		mf_regcomp(re,str)

/* -- */