/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:41 $
 * $Revision: 1.3 $
 */
 
#include <stdlib.h>

/* Visual C++ runtime function used as placeholder in pure virtual vtables
 * to cause abort during attempted execution */

extern "C" void __cdecl _purecall(void);
extern "C" void __cdecl _purecall(void)
{
	abort ();
}

/*
	Change Record
 * blc 980522 Created
*/
