/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.4 $
 */

#include <crtl.h>
#include <CPlusLib.h>
#include "NMWException.h"
#include <stdlib.h>

#ifdef __cplusplus           /* hh 971207 Added namespace support */
	extern "C" {
#endif

// Run the static init code, passing the initialization table,
// which is a list of pointers to functions to call, 
// terminated by a zero entry.  We simply loop through, 
// calling each function.

void _MSL_CDECL _RunInit(char *funcsptr)
{
	typedef void (**pfn)(void);
   
	pfn func;
    
    if (!funcsptr) return;
    
	func = (pfn)(funcsptr + 4);

	while (*func)
	{
		(*func)();
		func++;
	}
}

unsigned long _MSL_CDECL __detect_cpu_instruction_set(void)
{
	asm 
	{
	;; check whether CPUID is supported
	;; (bit 21 of Eflags can be toggled)
	pushfd					; save Eflags
	pop eax					; transfer Eflags into EAX
	mov edx, eax			; save original Eflags
	xor eax, 0x00200000		; toggle bit 21
	push eax				; put new value of stack
	popfd					; transfer new value to Eflags
	pushfd					; save updated Eflags
	pop eax					; transfer Eflags to EAX
	xor eax, edx			; updated Eflags and original differ?
	jnz check_CPUID			; no diff, bit 21 cannot be toggled

	xor edx, edx			; clear flags (this is at best a 486!)
	jmp NO_CPUID
	
check_CPUID:
	xor eax, eax			; check vendor string
	cpuid
	xor ebx, ecx
	xor ebx, edx
	bswap ebx				; get bytes in expected order
	cmp ebx, 'Auth' ^ 'enti' ^ 'cAMD'	; check for AMD string
	je check_AMD			; likely an AMD chip

	mov eax, 1				; basic feature check
	cpuid					; get feature flags
	jmp mask_and_ret

check_AMD:
	
	mov eax, 0x80000001		; call extended function 80000001h
	cpuid					; get extended feature flags

mask_and_ret:	
	and edx, _CPU_FEATURE_MMX | 
			_CPU_FEATURE_AMD_3DNOW |
			_CPU_FEATURE_CMOV_FCOMI |
			_CPU_FEATURE_SSE |
			_CPU_FEATURE_SSE_2
							; mask features directly from flags
NO_CPUID:
	mov eax, edx			; return features bitmask
	}
}

#ifdef __cplusplus         /* hh 971207 Added namespace support */
	}
#endif

/* Change Record:
 * hh 971207 Added namespace support
 * cc 000316 Added prototype for __detect_cpu_features
 * mf/gm 000717 removed call to atexit in _Runinit because the atexit function 
                array is global and therefore all global destructors were getting
                called when the process exited which was before any dll would unload.
                Therefore all the dll's global objects were destroyed before they had
                a chance to unload and do their own cleanup which in some cases required
                the use of their global objects.
 * ejs 011031 Fix AMD checking logic, and add SSE-2 feature
 * cc  011203 Added _MSL_CDECL for new name mangling 
 * ejs 011206 Cleanup of init/term code
 */