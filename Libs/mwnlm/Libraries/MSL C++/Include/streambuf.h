/* Metrowerks Standard Library
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:37 $ 
 * $Revision: 1.3 $ 
 */

// streambuf.h           // hh 971207 Changed filename from streambuf to streambuf.h

#ifndef _STREAMBUF_H              // hh 971207 added standard include guards
#define _STREAMBUF_H

#include <streambuf>

#ifndef _MSL_NO_CPP_NAMESPACE      // hh 971207 Added backward compatibility
#ifndef _MSL_NO_IO
	using std::basic_streambuf;
	using std::streambuf;
#ifndef _MSL_NO_WCHART_CPP_SUPPORT
	using std::wstreambuf;
#endif
#endif
#endif

#endif  // _STREAMBUF_H

// hh 971207 Changed filename from streambuf to streambuf.h
// hh 971207 added standard include guards
// hh 971207 Added backward compatibility
// hh 990120 changed name of MSIPL flags
// hh 991113 modified using policy
