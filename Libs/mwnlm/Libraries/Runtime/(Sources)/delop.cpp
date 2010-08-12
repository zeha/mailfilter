/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.3 $
 */
 
/*	Modification History
 *	--------------------
 *
 *	5-Sep-96 KO	Made this file. The original MSL file new.cpp was split
 *	            into NewHand.cpp, NewHand.h,  newop.cpp, and delop.cpp so
 *              you can overload the new and delete operators.
 */

#include <new>
#include <stdlib.h>

#pragma exceptions off

_MSL_IMP_EXP_RUNTIME void __cdecl operator delete(void * p)
{
	_CSTD::free(p);
}

