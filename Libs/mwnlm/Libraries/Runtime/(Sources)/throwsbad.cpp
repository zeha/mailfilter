/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:41 $
 * $Revision: 1.2 $
 */
 
#include <stdlib.h>

#ifndef _MSL_NO_CPP_NAMESPACE
    namespace std {
#endif

// Global variable indicating whether a bad_alloc object should be thrown
// when a new operation failes.

char    __throws_bad_alloc = 0;

#ifndef _MSL_NO_CPP_NAMESPACE
    }
#endif

// Change History
// blc 990427  moved __throws_bad_alloc definition to throwsbad.cpp
