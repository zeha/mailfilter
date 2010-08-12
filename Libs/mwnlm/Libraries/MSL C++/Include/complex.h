/* Metrowerks Standard Library
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:32 $ 
 * $Revision: 1.3 $ 
 */
                                       
// complex.h        // hh 971206 Changed filename from complex to complex.h

#ifndef _COMPLEX_H         // hh 971206 Made include guards standard
#define _COMPLEX_H

#include <complex>

#ifndef _MSL_NO_CPP_NAMESPACE      // hh 971206 Backward compatibility added with "using"
#if !defined(_No_Floating_Point) && !defined(_MSL_NO_MATH_LIB)
	using std::complex;
#endif
#endif

#endif  // _COMPLEX_H

// hh  971206 Changed filename from complex to complex.h
// hh  971206 Made include guards standard
// hh  971206 Backward compatibility added with "using"
// hh  990120 changed name of MSIPL flags
// hh  991112 modified using policy
// blc 011211 Added _MSL_NO_MATH_LIB macro check