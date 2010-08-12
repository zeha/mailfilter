/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.2 $
 */
 
 
#include <errno.h>
#include <math.h>

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

#ifndef NAN
#define NAN 	(0.0/0.0)
#endif

/*
	These functions are referenced by the compiler, 
	and should NOT be used for general purposes.
*/

__declspec(naked) void __cdecl _inline_pow (void);
__declspec(naked) void __cdecl _reduce(void);

static double __cdecl _inline_pow2 (double x, double y, unsigned long old_eax);

/*
	This function is used by the compiler when inlining
	the pow function, it CANNOT be used as a general purpose
	routine.  It assumes that the base and exponent are already
	on the fp register stack and that the condition codes c0,c2,
	c3 are in ah as a result of the ftst function on the base x
*/

__declspec(naked) void __cdecl _inline_pow (void)
{
	asm
	{
		sub esp, 20	; allocate room for args to inline_pow2
		fstp qword ptr[esp]	; move x to first argument
		fstp qword ptr[esp+8]	; move y to second argument
		mov [esp+16], eax	; save eax as third argument
		call _inline_pow2
		add esp ,20    ; restore stack
		ret
	} 
}

static double _inline_pow2 (double x, double y, unsigned long old_eax)
{
	double _ret;
	short TEMP=1;
	int i=y;

	if((x==0.0 && y <= 0) || (x < 0 && (/*rint(y)*/ ((double)i) != y)))
	{
		// EJS 010511: standard says pow(0,0) is INF, not 1.0
		errno=EDOM;
		if (x == 0.0 && y < 0.0)
			return INFINITY;
		else
			return NAN ;
	}
	asm
	{
		fld y
		fld x
		mov eax, old_eax
		and ax,0x4000      ; If base is not positive check if its zero
			               ; true if base is zero
		jz negative_base
		mov TEMP, 0
		jmp zero_base
	negative_base:

		;; test for even or odd
		fxch
		fld st
		fistp old_eax
		mov al, byte ptr old_eax
		test al,0x01
		fxch
		fchs			; base,exp combo legit so we change
					    ; sign of base to make it >0
		jz normal_pow
		mov TEMP,-1
	normal_pow:
		fyl2x			; st=y*log2(x) overwrites st(1) and pops stack
		fld st			; backup I+f
		frndint			; st=I st(1)=y*log2(x)
		fxch
		fsub st,st(1)	; st(1)-st=st=f overwrites st(1)
		f2xm1
		fld1 
		fadd			; st=2^f st(1)=2^f-1 st(2)=I
		fscale

	zero_base:	
		fstp st(1)	
		fimul TEMP		; change sign of result if exponent is odd
		fstp _ret
	}

	return _ret;
}

__declspec(naked) void __cdecl _reduce(void)
{
	asm
	{
		fldpi			; st=pi st(1)=x
		fxch 
	reduce:
		fld dword ptr 0.5f
		fmul 
		fprem
		fnstsw ax	    ; 16 bit register
		test ax, 0400h	; check for C2 bit
		jnz  reduce
		fstp st(1)
		ret
	}
}

