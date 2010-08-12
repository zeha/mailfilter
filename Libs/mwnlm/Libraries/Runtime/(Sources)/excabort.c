/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2001 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2001/09/07 23:29:25 $
 * $Revision: 1.2 $
 */

#include <windef.h>
#include <winbase.h>
#include <winuser.h>

// abort for CPPEXP.DLL

void abort()
{
    MessageBox(NULL, "Unhandled CPP Exception", "CPP RTl", MB_OK);
    ExitProcess(0);
}