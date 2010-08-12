/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.4 $
 */
 
#include <stdio.h>	// 960711: Wouldn't compile without it.
#include <stdarg.h>
#include <exception>
#include <crtl.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <excpt.h>
#include <eh.h>

#include "NMWException.h"

// Any of these bits means handler is called in unwinding mode

#define UNWINDING_MASK			0x66

// Exception code for a C++ exception

#define CPP_EXCEPTION_CODE		0xE06D7363


// Microsoft C++ unhandled exception handler

extern "C" int __cdecl _MWCHandler(
   	PEXCEPTION_RECORD exc, void */*exc_stack*/, LPCONTEXT context, void *mystery);       /* hh 971207 Added prototype */

extern "C" int __cdecl _MWCHandler(
    PEXCEPTION_RECORD exc, void */*exc_stack*/, LPCONTEXT context, void */*mystery*/)
{
	// Don't do anything if we are unwinding
	
	if (exc->ExceptionFlags & UNWINDING_MASK)
	{
	    return 1;
	}

	// Check for a C++ exception
	
	if (exc->ExceptionCode == CPP_EXCEPTION_CODE)
	{
		// Unexpected exception thrown (this is the outermost handler)
		
		_STD::unexpected();
		
		// should not return
	}
    
    return 1;
}

//	Callback for unhandled exception when MS exceptions are not used
extern "C" int __cdecl _HandleUnexpectedSEHException(LPEXCEPTION_POINTERS exc);
#define EXCEPTION_NESTED_CALL		0x10
#define EXCEPTION_UNWINDING			0x2

extern "C" int __cdecl _HandleUnexpectedSEHException(LPEXCEPTION_POINTERS exc)
{
	_se_translator_function func;
	
	// Check for user SEH filter
	if (!(exc->ExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING|EXCEPTION_NESTED_CALL)) 
	&& 	(func = __user_se_translator) != 0L)
	{
		func(exc->ExceptionRecord->ExceptionCode, exc);
	}
	else
	{
		// no 'else':  we can't safely generate a C++ exception here,
		// since the MW exception model must expect to receive one.
	}
	return EXCEPTION_CONTINUE_SEARCH;
}


/*
	Change Record
 * hh 971207 Added _MWCHandler prototype
 * mf 980827 fix for CE
 * mf 980903 another fix for CE, missing comma
 * es mf 000707 fix to failure of displaying message box
                in handling unmasked exceptions when msl was
                fully optimized.  
 * ejs 011023 note: this function is not used anymore
 * ejs 020406 Reinstated function for uncaught MS C++ exceptions
 * ejs 020411 Added function to translate uncaught Win32 exceptions for zero-overhead exceptions
*/
