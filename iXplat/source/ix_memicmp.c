/*
 * ic_memicmp.c
 *
 */
#define _IX_LIBRARY
#include "ix.h"

#undef _MT

/***
*memicmp.c - compare memory, ignore case
*
*       Copyright (c) 1988-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _memicmp() - compare two blocks of memory for lexical
*       order.  Case is ignored in the comparison.
*
*******************************************************************************/

//#include <cruntime.h>
#include <string.h>
//#include <mtdll.h>
#include <ctype.h>
//#include <setlocal.h>
//#include <locale.h>

/***
*int _memicmp(first, last, count) - compare two blocks of memory, ignore case
*
*Purpose:
*       Compares count bytes of the two blocks of memory stored at first
*       and last.  The characters are converted to lowercase before
*       comparing (not permanently), so case is ignored in the search.
*
*Entry:
*       char *first, *last - memory buffers to compare
*       size_t count - maximum length to compare
*
*Exit:
*       returns < 0 if first < last
*       returns 0 if first == last
*       returns > 0 if first > last
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

static int __cdecl __ascii_memicmp (
        const void * first,
        const void * last,
        size_t count
        )
{
        int f = 0;
        int l = 0;
        while ( count-- )
        {
            if ( (*(unsigned char *)first == *(unsigned char *)last) ||
                 ((f = tolower( *(unsigned char *)first )) ==
                  (l = tolower( *(unsigned char *)last ))) )
            {
                    first = (char *)first + 1;
                    last = (char *)last + 1;
            }
            else
                break;
        }
        return ( f - l );
}


int __cdecl ix_memicmp (
        const void * first,
        const void * last,
        unsigned int count
        )
{
        int f = 0, l = 0;
        const char *dst = (const char*)first, *src = (const char*)last;
/*        if ( __lc_handle[LC_CTYPE] == _CLOCALEHANDLE )
        {
            return __ascii_memicmp(first, last, count);
        }
        else {*/
            while (count-- && f==l)
            {
                f = tolower( (unsigned char)(*(dst++)) );
                l = tolower( (unsigned char)(*(src++)) );
            }
        //}

        return ( f - l );
}


