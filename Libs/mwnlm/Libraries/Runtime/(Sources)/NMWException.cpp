/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:39 $
 * $Revision: 1.4 $
 */

#define __NOSTRING__	//	do not include <string>

#include <ansi_parms.h>
#include <stdlib.h>
#include <CPlusLib.h>
#include <exception.h>
#include <NMWException.h>		 // ra 990322 changed form "" to <>

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifndef _MSL_NO_CPP_NAMESPACE      // hh 971207 Added namespace support
	namespace std {
#endif

static void dthandler()		{ abort(); };			//	default terminate handler function
static terminate_handler	thandler = dthandler;	//	pointer to terminate handler function
static void duhandler()		{ terminate(); };		//	default unexpected handler function
static unexpected_handler	uhandler = duhandler;	//	pointer to unexpected handler function

/************************************************************************/
/*	Purpose..: 	Set a terminate handler function						*/
/*	Input....:	pointer to terminate handler function					*/
/*	Return...:	---														*/
/************************************************************************/
extern terminate_handler set_terminate(terminate_handler handler) _MSL_THROW  // hh 980102 added exception specific
{
	terminate_handler old=thandler; thandler=handler; return old;
}

/************************************************************************/
/*	Purpose..: 	Terminate exception handling							*/
/*	Input....:	---														*/
/*	Return...:	--- (shall not return to caller)						*/
/************************************************************************/
extern void terminate()
{
	thandler();
}

/************************************************************************/
/*	Purpose..: 	Set an unexpected handler function						*/
/*	Input....:	pointer to unexpected handler function					*/
/*	Return...:	---														*/
/************************************************************************/
extern unexpected_handler set_unexpected(unexpected_handler handler) _MSL_THROW  // hh 980102 added exception specific
{
	unexpected_handler old=uhandler; uhandler=handler; return old;
}

/************************************************************************/
/*	Purpose..: 	Handle unexpected exception								*/
/*	Input....:	---														*/
/*	Return...:	--- (shall not return to caller)						*/
/************************************************************************/
extern void unexpected()
{
	uhandler();
}

#ifndef _MSL_NO_CPP_NAMESPACE
	}
#endif

/************************************************************************/
/* Purpose..: Compare a throw and a catch type							*/
/* Input....: pointer to throw type										*/
/* Input....: pointer to catch type (0L: catch(...)						*/
/* Return...: true: can catch; false: cannot catch						*/
/************************************************************************/
extern char __throw_catch_compare(const char *throwtype,const char *catchtype,long *offset_result)
{
	const char	*cptr1,*cptr2;

	*offset_result=0;	
	if((cptr2=catchtype)==0)
	{	//	catch(...)
		return true;
	}

	cptr1=throwtype;

#if _WIN32
    if (IsBadReadPtr(cptr1, 1) || IsBadReadPtr(cptr2, 1))
    	terminate();
#endif

	if(*cptr2=='P')
	{	//	catch(cv T *)
		//  string is PlT where l = A-D for const and volatile
		cptr2 += 2;	// get to the type
		if(*cptr2=='X')
		{	//	catch(cv void *)
			if(*cptr1=='P' || *cptr1=='*')
			{	//	throw T*;
				return true;
			}
		}
		cptr2=catchtype;
	}

	switch(*cptr1)
	{	//	class pointer/reference throw
	case '*':
	case '!':
		if(*cptr1++!=*cptr2++) return false;
		for(;;)
		{	//	class name compare loop
			if(*cptr1==*cptr2++)
			{
				if(*cptr1++=='!')
				{	//	class match found / get offset
					long offset;

					for(offset=0; *cptr1!='!';) offset=offset*10+*cptr1++-'0';
					*offset_result=offset; return true;
				}
			}
			else
			{
				while(*cptr1++!='!') ;			//	skip class name
				while(*cptr1++!='!') ;			//	skip offset
				if(*cptr1==0) return false;		//	no more class names => no match
				cptr2=catchtype+1;				//	retry with next class name
			}
		}
		return false;
	}

	while((*cptr1=='P' || *cptr1=='Q') && *cptr1==*cptr2)
	{	//	pointer/reference catch => match cv-qualifiers
		int cv1, cv2;
		
		cptr1++; cptr2++;
		
		// Get cv qual for catcher and thrower
		
		cv1 = *cptr1++ - 'A';
		cv2 = *cptr2++ - 'A';
		
		// catcher must be at least as qualified as thrower 
		
		if ((cv1 & ~cv2) != 0) return false;
	}

	//	plain type throw catch
	for(; *cptr1==*cptr2; cptr1++,cptr2++) if(*cptr1==0) return true;
	return false;
}

/* Change History
961209 KO	Commented out true and false defines which should already be defined.
// hh 971207 Added namespace support
// rjk 980322 Removed global destructor stuff to a separate module.
// ejs 000203 Fixed precedence problem causing spurious failures when throwing
//            constant strings
// ejs 020412 Be more careful about bad stack frames
*/
