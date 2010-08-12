/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:39 $
 * $Revision: 1.3 $
 */
 
/*
 *	Modification History
 *	--------------------
 *
 *	5-Sep-96 KO	Made this file. The original MSL file new.cpp was split
 *	            into NewHand.cpp, NewHand.h,  newop.cpp, and delop.cpp so
 *              you can overload the new and delete operators.
 */

#include <new>
#include <NewHand.h>
#include <stdio.h>
#pragma exceptions off

#ifndef _MSL_NO_CPP_NAMESPACE
    namespace std {
#endif

static void default_new_handler()// throw(bad_alloc)
{
	//throw(bad_alloc());
	printf("MAILFILTER: bad_alloc!\n");
}

new_handler	_new_handler_func = default_new_handler;

new_handler set_new_handler(new_handler newer_handler) throw()  // hh 980108 added exception specification
{
    new_handler old_handler = _new_handler_func;
    
    _new_handler_func = newer_handler;
    
    return(old_handler);
}

#ifndef _MSL_NO_CPP_NAMESPACE
    }
#endif

// hh 971207 Added namespace support