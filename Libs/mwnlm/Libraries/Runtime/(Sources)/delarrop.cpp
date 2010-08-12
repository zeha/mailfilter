/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.3 $
 */

#include <new>
#include <NewHand.h>
#include <stdlib.h>

#pragma exceptions on


#ifndef _MSL_NO_CPP_NAMESPACE
	namespace std {
#endif


// Global variable indicating whether a bad_alloc object should be thrown
// when a new operation failes.
extern char	__throws_bad_alloc;

#ifndef _MSL_NO_CPP_NAMESPACE
	}
#endif

/************************************************************************/
/*	Purpose..: 	Array allocation/deallocation functions					*/
/*	Input....:	---														*/
/*	Return...:	---														*/
/************************************************************************/
#if __MWERKS__>=0x2020

_MSL_IMP_EXP_RUNTIME extern void __cdecl operator delete[](void *ptr) throw()
{
	operator delete(ptr);
}
#endif
