/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/08/12 16:41:33 $
 * $Revision: 1.4 $
 */
 
#include <stdlib.h>
#include <string.h>
#include <CPlusLib.h>        // hh 971207 added <CPlusLib.h> header

// hh 971207 Moved declarations into <CPlusLib.h>

// A null pointer to member function

extern MemberFuncPointerType __ptmf_null = {0, 0, 0};	// 961217 KO


// Test a pointer to member function for null.  Returns 0 if it is NULL,
// 1 if not.

int __cdecl __ptmf_test(MemberFuncPointerType *ptmf)
{
    return (ptmf->this_delta | ptmf->vtbl_offset | ptmf->func_data.ventry_offset) != 0;
}

// Compare two pointer to member functions.  Returns 0 if equal, 1 if not

int __cdecl __ptmf_cmpr(MemberFuncPointerType *p1, MemberFuncPointerType *p2)
{
	if (p1->this_delta != p2->this_delta || 
	    p1->vtbl_offset != p2->vtbl_offset ||
	    p1->func_data.func_addr != p2->func_data.func_addr)
	{
	    return 1;
	}
	return 0;
}


void *__cdecl __ptmf_cast(int delta, MemberFuncPointerType *in, MemberFuncPointerType *out)
{
    *out = *in;
    out->this_delta = in->this_delta + delta;
    return out;
}

/*
*/

// Copy function

void *__cdecl  __copy(void *dest, void *source, unsigned long len)
{
    return memmove(dest, source, len);
}

// Memory clearing
void *__cdecl  __clear(void *to, size_t size)
{
    if (to)
    	memset(to, 0, size);
   	return to;
}


// Call a fucntion through a pointer to member.  __thiscall or __fastcall version

void __declspec(naked) *__cdecl __ptmf_scall()
{
	asm
	{
		// pop our args off the stack
		
		POP		EAX						// get return address
		PUSH	EDX						// save this guy
		MOV		EDX, 4[ESP]				// get ptmf pointer in EDX
		MOV		4[ESP], EAX				// put return address in place
		
		ADD		ECX, [EDX]				// adjust the object pointer to proper class
		CMP		DWORD PTR 4[EDX], -1	// check for a static method
		JE		NotVirt
		
		// ECX already has the object pointer
			
		MOV		EAX, 8[EDX]				// get offset to virtual table
		MOV		EAX, [ECX+EAX]			// get addr of vtable
		ADD		EAX, 4[EDX]				// get offset into vtab
		POP		EDX
		JMP		DWORD PTR [EAX] 		// get address of routine
		
	NotVirt:
		MOV		EAX, EDX
		POP		EDX
		JMP		DWORD PTR 8[EAX]
	}
}

// Call a fucntion through a pointer to member __stdcall or __cdecl version
void __declspec(naked) *__cdecl __ptmf_scalls()
{
	asm
	{
		// pop our args off the stack
		
		POP		EAX						// get return address
		POP		EDX						// get pointer to member pointer
		MOV		ECX, [ESP]				// get object pointer
		PUSH	EAX						// repush return address
				
		ADD		ECX, [EDX]				// adjust the object pointer to proper class
		MOV		4[ESP], ECX				// put back adjusted object pointer
		CMP		DWORD PTR 4[EDX], -1	// check for a static method
		JE		NotVirt
		
		// ECX already has the object pointer
			
		MOV		EAX, 8[EDX]				// get offset to virtual table
		MOV		EAX, [ECX+EAX]			// get addr of vtable
		ADD		EAX, 4[EDX]				// get offset into vtab
		JMP		DWORD PTR [EAX] 		// get address of routine
		
	NotVirt:
		JMP		DWORD PTR 8[EDX]
	}
}

/* Change History
 * 961217 KO	Changed __ptmf_null to _ptmf_null for CW11 compiler.
 * hh 971207 	added <CPlusLib.h> header
 * hh 971207 	Moved declarations into <CPlusLib.h>
 * blc 990817   Removed #ifdef'd out __ptmf_call function
 * ejs 020319	Added __cdecl
 */
 
