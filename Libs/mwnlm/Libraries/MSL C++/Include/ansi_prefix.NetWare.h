/*  Metrowerks Standard Library  Version 4.0  1998 August 10  */

/*  $Date: 2002/07/01 21:13:31 $ 
 *  $Revision: 1.3 $ 
 *  $NoKeywords: $ 
 *
 *		Copyright © 1995-1998 Metrowerks, Inc.
 *		All rights reserved.
 */
 
/*
 *	ansi_prefix.win32.h
 *	
 */

#ifndef __ansi_prefix__
#define __ansi_prefix__

#define __dest_os	__netware_os
#include <nlm_prefix.h>
#include <os_enum.h>

#define __MSL_LONGLONG_SUPPORT__

#define _MSL_USING_NAMESPACE

/* hh 980217 
	__ANSI_OVERLOAD__ controls whether or not the prototypes in the C++ standard
	section 26.5 get added to <cmath> and <math.h> or not.  If __ANSI_OVERLOAD__
	is defined, and a C++ compiler is used, then these functions are available,
	otherwise not.
	
	There is one exception to the above rule:  double abs(double); is available
	in <cmath> and <math.h> if the C++ compiler is used.  __ANSI_OVERLOAD__ has
	no effect on the availability of this one function.
	
	There is no need to recompile the C or C++ libs when this switch is flipped.
*/
/* #define __ANSI_OVERLOAD__ */

#endif
/*	Change Log
981202 blc   created based on ansi_prefix.Win32.h
*/
