/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:41 $
 * $Revision: 1.3 $
 */

#define __NOSTRING__


#include <ansi_parms.h>            
#include <stdexcept>
#include <exception.h>
#include <typeinfo>

extern void __priv_throwbadcast(void);   
extern void __priv_throwbadcast(void)
{
	throw _STD::bad_cast();				 
}


/* Change record: 
 * hh  971207 <ansi_parms.h> must be first
 * hh  980205 stdexcept implementation moved to <stdexcept>
 * cc  010918 removed if else endif for modenalib, added prototype
 *            for __priv_throwbadcast and added _STD to bad_cast
 */