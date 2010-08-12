/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.4 $
 */

#include <new>
#include <NewHand.h>
#include <stdlib.h>

#pragma exceptions on

#ifndef _MSL_NO_CPP_NAMESPACE
	namespace std {
#endif

	nothrow_t nothrow;           // blc 981230 added nothrow

#ifndef _MSL_NO_CPP_NAMESPACE
	}
#endif


void * __cdecl operator new(_CSTD::size_t size, const _STD::nothrow_t&) throw()
{
	try
	{
		return operator new(size);
	}
	catch(...)
	{
	}
	return 0;
}

/* Change History
 * blc 981230 moved std::nothrow defintion from newop.cpp to prevent link errors with MFC
 * blc 990830 the nothrow version of new now calls the other new,
 *            catching any thrown exceptions (IR9907-1360-1)
 * ejs 020319 added __cdecl
 */
