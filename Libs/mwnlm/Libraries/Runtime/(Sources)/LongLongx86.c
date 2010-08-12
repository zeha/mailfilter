/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:39 $
 * $Revision: 1.4 $
 */

#include <stddef.h>

#if __cplusplus
extern "C" {
#endif

typedef unsigned long long 	u64;
typedef long long			s64;

extern s64 __stdcall _rt_mul64(s64 left,s64 right);
extern u64 __stdcall _rt_divu64(u64 left,u64 right);
extern s64 __stdcall _rt_divs64(s64 left,s64 right);
extern u64 __stdcall _rt_modu64(u64 left,u64 right);
extern s64 __stdcall _rt_mods64(s64 left,s64 right);
extern s64 __stdcall _rt_shl64(s64 left,s64 count);
extern u64 __stdcall _rt_shru64(u64 left,s64 count);
extern s64 __stdcall _rt_shrs64(s64 left,s64 count);
extern u64 __stdcall _rt_rotl64(u64 left,s64 count);
extern u64 __stdcall _rt_rotr64(u64 left,s64 count);

#if __cplusplus
}
#endif

/*	
 *	The compiler now generates many of these operations, but
 *	only under certain conditions.  The following #pragmas
 *	force intrinsic generation for this file.
 */

#if __MWERKS__ < 0x2400
#error This runtime file is too new for this compiler
#endif

#pragma longlong_expand_all on
#pragma optimize_for_size off
#pragma optimization_level 4

/************************************************************************/
/* Purpose..: Compute result = left * right								*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: right operand												*/
/* Return...: pointer to result											*/
/************************************************************************/

extern s64 __stdcall _rt_mul64(s64 left, s64 right)
{
	return left * right;
}

/************************************************************************/
/* Purpose..: Compute result = left << (right % 64)						*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: shift count												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern s64 __stdcall _rt_shl64(s64 left,s64 count)
{
	return left << count;
}

/************************************************************************/
/* Purpose..: Compute result = left >> (right % 64) (unsigned)			*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: shift count												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern u64 __stdcall _rt_shru64(u64 left,s64 count)
{
	return left >> count;
}

/************************************************************************/
/* Purpose..: Compute result = left >> (right % 64) (signed)			*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: shift count												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern s64 __stdcall _rt_shrs64(s64 left,s64 count)
{
	return left >> count;
}

/************************************************************************/
/* Purpose..: Compute result = left ROTL right							*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: rotate count												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern u64 __stdcall _rt_rotl64(u64 left, s64 count)
{
	return __rol(left, count);
}

/************************************************************************/
/* Purpose..: Compute result = left ROTR right							*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: rotate count												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern u64 __stdcall _rt_rotr64(u64 left, s64 count)
{
	return __ror(left, count);
}

#pragma mark -

/************************************************************************/
/* Purpose..: Compute result = left / right (unsigned)					*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: right operand												*/
/* Return...: pointer to result											*/
/************************************************************************/

extern u64 __stdcall __declspec(naked) _rt_divu64(u64 dividend,u64 divisor)
{
	asm
	{
		push	ebx					// save ebx as per calling convention
		mov		ecx, [esp+20]		// divisor_hi
		mov		ebx, [esp+16]		// divisor_lo
		mov		edx, [esp+12]		// dividend_hi
		mov		eax, [esp+8]		// dividend_lo
		test	ecx, ecx			// divisor > 2^32-1?
		jnz		big_divisor			// yes, divisor > 32^32-1
		cmp		edx, ebx			// only one division needed? (ecx = 0)
		jae		two_divs			// need two divisions
		div		ebx					// eax = quotient_lo
		mov		edx, ecx			// edx = quotient_hi = 0 (quotient in edx:eax)
		pop		ebx					// restore ebx as per calling convention
		ret		16					// done, return to caller
	two_divs:
		mov		ecx, eax			// save dividend_lo in ecx
		mov		eax, edx			// get dividend_hi
		xor		edx, edx			// zero extend it into edx:eax
		div		ebx					// quotient_hi in eax
		xchg	eax, ecx			// ecx = quotient_hi, eax = dividend_lo
		div		ebx					// eax = quotient_lo
		mov		edx, ecx			// edx = quotient_hi (quotient in edx:eax)
		pop		ebx					// restore ebx as per calling convention
		ret		16					// done, return to caller
	big_divisor:
		push	edi					// save edi as per calling convention
		mov		edi, ecx			// save divisor_hi
		shr		edx, 1				// shift both divisor and dividend right
		rcr		eax, 1				// by 1 bit
		ror		edi, 1
		rcr		ebx, 1
		bsr		ecx, ecx			// ecx = number of remaining shifts
		shrd	ebx, edi, cl		// scale down divisor and dividend
		shrd	eax, edx, cl		// such that divisor is less than
		shr		edx, cl				// less than 2^32 (i.e. fits in ebx)
		rol		edi, 1				// restore original divisor_hi
		div		ebx					// compute quotient
		mov		ebx, [esp+12]		// dividend_lo
		mov		ecx, eax			// save quotient
		imul	edi, eax			// quotient * divisor hi-word (low only)
		mul		dword ptr [esp+20]	// quotient * divisor lo-word
		add		edx, edi			// edx:eax = quotient * divisor
		sub		ebx, eax			// dividend_lo - (quot.*divisor)_lo
		mov		eax, ecx			// get quotient
		mov		ecx, [esp+16]		// dividend_hi
		sbb		ecx, edx			// subtract divisor * quot. from dividend
		sbb		eax, 0				// adjust quotient if remainder negative
		xor		edx, edx			// clear hi-word of quot(eax<=ffffffffh)
		pop		edi					// restore edi as per calling convention
		pop		ebx					// restore ebx as per calling convention
		ret		16					// done, return to caller
	}
}

/************************************************************************/
/* Purpose..: Compute result = left / right (signed)					*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: right operand												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern s64 __stdcall _rt_divs64(s64 left,s64 right)
{
    s64 result;

	if (left < 0)
	{
		left = -left;
		if (right < 0)
		{	//	left<0 / right<0
			right = -right;
			result = _rt_divu64(left,right);
		}
		else
		{	//	left>=0 / right<0
			result = - _rt_divu64(left,right);
		}
	}
	else
	{
		if (right < 0)
		{	//	left<0 / right>=0
			right = -right;
			result = - _rt_divu64(left,right);
		}
		else
		{	//	left>=0 / right>=0
			result = _rt_divu64(left,right);
		}
	}
	return result;
}

/************************************************************************/
/* Purpose..: Compute result = left % right (unsigned)					*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: right operand												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern u64 __stdcall __declspec(naked) _rt_modu64(u64 left,u64 right)
{
	asm
	{
		push	ebx				//save ebx as per calling convention
		mov		ecx, [esp+20]	// divisor_hi
		mov		ebx, [esp+16]	// divisor_lo
		mov		edx, [esp+12]	// dividend_hi
		mov		eax, [esp+8]	// dividend_lo
		test	ecx, ecx		// divisor > 2^32-1?
		jnz		r_big_divisor	// yes, divisor > 32^32-1
		cmp		edx, ebx		// only one division needed? (ecx = 0)
		jae		r_two_divs		// need two divisions
		div		ebx				// eax = quotient_lo
		mov		eax, edx		// eax = remainder_lo
		mov		edx, ecx		// edx = remainder_hi = 0
		pop		ebx				// restore ebx as per calling convention
		ret		16
	r_two_divs:
		mov		ecx, eax		// save dividend_lo in ecx
		mov		eax, edx		// get dividend_hi
		xor		edx, edx		// zero extend it into edx:eax
		div		ebx				// eax = quotient_hi, edx = intermediate remainder
		mov		eax, ecx		// eax = dividend_lo
		div		ebx				// eax = quotient_lo
		mov		eax, edx		// eax = remainder_lo
		xor		edx, edx		// edx = remainder_hi = 0
		pop		ebx				// restore ebx as per calling convention
		ret		16				// done, return to caller
	r_big_divisor:
		push	edi				// save edi as per calling convention
		mov		edi, ecx		// save divisor_hi
		shr		edx, 1			// shift both divisor and dividend right
		rcr		eax, 1			//  by 1 bit
		ror		edi, 1
		rcr		ebx, 1
		bsr		ecx, ecx		// ecx = number of remaining shifts
		shrd	ebx, edi, cl	// scale down divisor and dividend such
		shrd	eax, edx, cl	//  that divisor is less than 2^32
		shr		edx, cl			//  (i.e. fits in ebx)
		rol		edi, 1			// restore original divisor (edi:esi)
		div		ebx				// compute quotient
		mov		ebx, [esp+12]	// dividend lo-word
		mov		ecx, eax		// save quotient
		imul	edi, eax		// quotient * divisor hi-word (low only)
		mul		dword ptr [esp+20] // quotient * divisor lo-word
		add		edx, edi		// edx:eax = quotient * divisor
		sub		ebx, eax		// dividend_lo - (quot.*divisor)Ðlo
		mov		ecx, [esp+16]	// dividend_hi
		mov		eax, [esp+20]	// divisor_lo
		sbb		ecx, edx		// subtract divisor * quot. from dividend
		sbb		edx, edx		// (remainder < 0)? 0xffffffff : 0
		and		eax, edx		// (remainder < 0)? divisor_lo : 0
		and		edx, [esp+24]	// (remainder < 0)? divisor_hi : 0
		add		eax, ebx		// remainder += (remainder < 0)?
		adc		edx, ecx		// divisor : 0
		pop		edi				// restore edi as per calling convention
		pop		ebx				// restore ebx as per calling convention
		ret		16				// done, return to caller
	}
}

/************************************************************************/
/* Purpose..: Compute result = left % right (signed)					*/
/* Input....: pointer to result											*/
/* Input....: left operand												*/
/* Input....: right operand												*/
/* Return...: pointer to result											*/
/************************************************************************/
extern s64 __stdcall _rt_mods64(s64 left,s64 right)
{
    s64 result;

	if (left < 0)
	{
		left = -left;
		if (right < 0)
		{	//	left<0 / right<0
			right = -right;
			result = _rt_modu64(left,right);
		}
		else
		{	//	left<0 / right>=0
			result = _rt_modu64(left,right);
		}
		result = -result;
	}
	else // left >= 0
	{
		if (right < 0)
		{	//	left>=0 / right<0
			right = -right;
			result = _rt_modu64(left,right);
		}
		else
		{	//	left>=0 / right>=0
			result = _rt_modu64(left,right);
		}
	}
	return result;
}

#pragma mark -

/*
 *	These operations are always inlined now, but preserved for
 *	backward compatibility.
 */

extern int __stdcall _rt_cmpu64(u64 left,u64 right);
extern int __stdcall _rt_cmps64(s64 left,s64 right);

/************************************************************************/
/* Purpose..: Compare left and right (unsigned)							*/
/* Input....: left operand												*/
/* Input....: right operand												*/
/* Return...: -1 : left < right; 0: left == right; 1: left > right		*/
/************************************************************************/
extern __declspec(naked) int __stdcall _rt_cmpu64(u64 left,u64 right)
{
	asm
	{
		mov		eax, 8[esp]
		cmp		eax, 16[esp]
		jne		done

		mov		eax, 4[esp]
		cmp		eax, 12[esp]

	done:
		ret		16
	}
}

/************************************************************************/
/* Purpose..: Compare left and right (unsigned)							*/
/* Input....: left operand												*/
/* Input....: right operand												*/
/* Return...: -1 : left < right; 0: left == right; 1: left > right		*/
/************************************************************************/
extern __declspec(naked) int __stdcall _rt_cmps64(s64 left,s64 right)
{
	asm
	{
		mov		eax, 8[esp]
		cmp		eax, 16[esp]
		jne		done

		mov		eax, 4[esp]
		cmp		eax, 12[esp]
		je		done
		ja		high

		mov		eax, 0
		cmp		eax, 1
		ret		16

	high:
		mov		eax, 1
		cmp		eax, 0

	done:
		ret		16
	}
}

