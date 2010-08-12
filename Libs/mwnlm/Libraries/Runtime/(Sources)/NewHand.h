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

#ifndef NEWHAND_H
#define NEWHAND_H

#include <new>

#ifndef _MSL_NO_CPP_NAMESPACE
	namespace std {
#endif

// Regular MSL uses this variable as a static variable in new.cpp called
// new_handler_func (no leading underscore).
_MSL_IMP_EXP_RUNTIME extern new_handler     _new_handler_func;

#ifndef _MSL_NO_CPP_NAMESPACE
	}
#endif

#endif

// hh  971207 Added namespace support
// blc 990830 Allow compilation with MSL 5.1 namespace options
