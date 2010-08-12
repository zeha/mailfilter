/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.2 $
 */
 
#include <new>

#pragma exceptions on

#if __MWERKS__>=0x2400

_MSL_IMP_EXP_RUNTIME extern void __cdecl operator delete[](void* p, const _STD::nothrow_t&) throw()
{
	operator delete(p);
}

#endif
#pragma exceptions reset