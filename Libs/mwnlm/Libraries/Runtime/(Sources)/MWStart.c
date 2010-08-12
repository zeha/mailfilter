/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2001 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/09/04 17:05:34 $
 * $Revision: 1.7 $
 */

#include <stddef.h>
#include <stdlib.h>

#include <crtl.h>
#include <CPlusLib.h>
#include <ExceptionX86.h>

int __init_environment(void *reserved);
int __deinit_environment(void *reserved);

// C++ Exception Tables
static ExceptionTableHeader ExceptionTables =
{
    &__PcToActionStart,
    &__PcToActionEnd,
    0
};

/*
	CodeWarrior NLM Runtime 

	NOTE: 	The C++ language standard forbids destruction of C++
			objects when abort() is called, but due to possible
			catastrophic results of not releasing system resources
			possibly held by C++ objects in kernel NLMs, we call
			destructors anyway.  Complain to Novell if you don't
			like this.

			These functions are consumed by Novell's CLib and LibC
			prelude code linked into an NLM by including clibpre.o
			or libcpre.o in the CodeWarrior NLM project. For CLib,
			this constitutes a new prelude object that makes some
			assumptions similar to LibC's prelude (read on).

			These do not have to be done for C, but are harmless
			anyway. Indeed, they do NOT get done unless mwcrtl.lib
			is linked into the NLM. In the case where this library
			isn't linked, then __init_environment() and
			__deinit_environment() are got from LibC (do-nothing
			stubs).
*/

int __init_environment(void *reserved)
{
	/* Register our exception handling tables */
//	_RegisterExceptionTables(&ExceptionTables);

	/* initialize static objects */
	_RunInit(&___xc_a);

	/* success */
	return 0;
}

int __deinit_environment(void *reserved)
{
	/* destroy globally initialized C++ objects */
	__destroy_global_chain();	
	
	/* success */
	return 0;
}

/*
 * Change record:
 *
 * ejs 010910 Added entry/exit callbacks for LibC
 * cdo 020808 Migrated to CLib
 * ejs 020809 Fixes for _RunInit
 */
