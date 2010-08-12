/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.3 $
 */


#define __NOSTRING__	//	do not include <string>

#include <ansi_parms.h>
#if _WIN32
#include <abort_exit.h>
#include <wchar.h>
#endif
#include <crtl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <CPlusLib.h>
#include "NMWException.h"

/************************************************************************/
/*	Purpose..: 	Register a global object for later destruction			*/
/*	Input....:	pointer to global object								*/
/*	Input....:	pointer to destructor function							*/
/*	Input....:	pointer to global registration structure				*/
/*	Return...:	pointer to global object (pass thru)					*/
/************************************************************************/
extern void *__cdecl __register_global_object(void *object,void *destructor,void *regmem)
{
	((DestructorChain *)regmem)->next=__global_destructor_chain;
	((DestructorChain *)regmem)->destructor=destructor;
	((DestructorChain *)regmem)->object=object;
	__global_destructor_chain=(DestructorChain *)regmem;

	return object;
}



/************************************************************************/
/* Purpose..: Destroy all constructed global objects					*/
/* Input....: ---														*/
/* Return...: ---														*/
/************************************************************************/

DestructorChain  * __global_destructor_chain;

void _MSL_CDECL __destroy_global_chain(void)
{
	DestructorChain	*gdc;

	while((gdc=__global_destructor_chain)!=0L)
	{
		__global_destructor_chain=gdc->next;
	    asm
	    {
	        	mov		eax, gdc
	        	mov		ecx, DestructorChain.object[eax]
	        	call	dword ptr DestructorChain.destructor[eax]
	    }	
	}
}

#ifndef _MAX_ATEXIT_FUNCS
	#define _MAX_ATEXIT_FUNCS 64
#endif

static DestructorChain atexit_funcs[_MAX_ATEXIT_FUNCS];
static long	atexit_curr_func = 0;

int _MSL_CDECL __register_atexit(void (*func)(void))
{
	if (atexit_curr_func == _MAX_ATEXIT_FUNCS)
		return -1;
	__register_global_object(0, func, &atexit_funcs[atexit_curr_func++]);
	return 0;
}

/* Change record:
 * cc  011203 Added _MSL_CDECL for new name mangling
 * cc  020225 Removed register_atexit prototype moved to header 
 * ejs 020319 Added __cdecl
 */