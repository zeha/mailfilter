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

/************************************************************************/
/*	Purpose..: 	Array allocation/deallocation functions					*/
/*	Input....:	---														*/
/*	Return...:	---														*/
/************************************************************************/
#if __MWERKS__>=0x2020

extern void *__cdecl operator new[](_CSTD::size_t size,const _STD::nothrow_t& nt) throw()
{
	return operator new(size,nt);
}
#endif

