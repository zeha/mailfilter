/* Metrowerks Standard Library
 * Copyright � 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:36 $ 
 * $Revision: 1.3 $ 
 */

// numeric.h          // hh 971207 Changed filename from numeric to numeric.h

#ifndef _NUMERIC_H             // hh 971207 added standard include guards
#define _NUMERIC_H

#include <numeric>

#ifndef _MSL_NO_CPP_NAMESPACE           // hh 971207 Added backward compatibility
	using std::accumulate;
	using std::inner_product;
	using std::partial_sum;
	using std::adjacent_difference;
#endif

#endif  // _NUMERIC_H

// hh 971207 Changed filename from numeric to numeric.h
// hh 971207 added standard include guards
// hh 971207 Added backward compatibility
// hh 990120 changed name of MSIPL flags
// hh 991113 modified using policy
