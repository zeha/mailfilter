/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.4 $
 */

// Microsoft C++ and structured exception handling runtime
//#define DEBUG 1

#pragma enumsalwaysint on

#include <ansi_parms.h> 
#include <cwchar>
#include <cstdarg>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <exception>
#include <eh.h>
#include <crtl.h>
#include "NMWException.h"
#include "ExceptionX86.h"

// Get the processor flags so we know whether to save/restore registers

static unsigned long procflags = __detect_cpu_instruction_set();

#if DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
static FILE *logfile;
static void SetupDebug(void);
static char *LookupSymbol(int addr);
static char *LookupCurrentFunc(long *framehandler);
#endif

#define CPP_EXCEPTION_CODE		(0xE06D7363)
#define MAGIC_NUMBER			(0x19930520)

// Any of these bits means handler is called in unwinding mode

#define UNWINDING_MASK			0x66

// Flags on exception types

#define TYPE_CONST				1
#define TYPE_VOLATILE			2
#define TYPE_REFERENCE			8

// Flags on the subtype structure for throws

#define SUBTYPE_NON_CLASS		1
#define SUBTYPE_VBASE			4


// This is the RTTI typeid structure

typedef struct TypeId
{
	void 		*vtab;
	char		*tid;
	char		name[];
} TypeId;

// This structure identifies a type that may be caught by
// for this thrown exception.  If the type is a base class,
// the offset, vbtab and vbptr fields tell us how to get to
// this base class from the thrown type.

typedef struct ThrowSubType
{
	long		flags;		// CATCH_TYPE flags
	TypeId		*tid;		// pointer to typeid struct for type
	long		offset;		// offset of base class
	long		vbtab;		// offset of vb table or -1
	long		vbptr;		// offset of vb ptr in vb table
	long		size;		// size of catch type data
	void		*cctor;		// copy constructor or 0
} ThrowSubType;

// This structure identifies all of the types of catchers that
// apply to a thrown object

typedef struct SubTypeArray
{
    long			count;
    ThrowSubType	*subtypes[0];
} SubTypeArray;	



// This structure identifies the type of a thrown object

typedef struct ThrowType
{
	long		flags;		// TYPE_CONST TYPE_VOLATILE
	void		*dtor;		// Destuctor address or 0
	long		unknown;	// ??
	SubTypeArray *subtypes;	// Pointer to array of catch types for this throw
} ThrowType;


typedef struct Handler Handler;
typedef struct State State;
typedef struct Catcher Catcher;

// This is the header for the catch frame data for C++ exceptoin handling

typedef struct HandlerHeader
{
	long		magic;
	long		state_count;
	State		*states;
	long		handler_count;
	Handler		*handlers;
	long		unknown1;
	long		unknown2;
} HandlerHeader;


// This struct is an entry in the state table

struct State
{
	long		next;		// index of next state
	void		*callback;	// callback to unwind this state
};

// This struct defines a specific try-catch sequence

struct Handler
{
	long		first_state;// first state handler applies to
	long		last_state;	// last state handler applies to
	long		new_state;	// state to set if handling
	long		catch_count;// number of catchers
	Catcher		*catches;	// pointer to catcher
};


// This struct defines a single catcher in a try..catch sequence

struct Catcher
{
    long		flags;		// TYPE_CONST TYPE_VOLATILE TYPE_REFERENCE
    TypeId		*tid;		// pointer to typeid or 0 or catch (...)
    long		offset;		// offset from EBP of catch argument
	void 		*catchcode;	// code of catch block
};


// This is used to link togehter all the frame handlers

typedef struct FrameHandler FrameHandler;

struct FrameHandler
{
	FrameHandler	*next;	// link to frame handler
	void 			*code;	// pointer to handler code
	long			state;	// handler state (index into state table or -1)
};


// This struct is used to establish a handler for the frame handler funciton

typedef struct HandlerHandler
{
	FrameHandler	*next;	// link to frame handler
	void 			*code;	// pointer to handler code
	FrameHandler	*fh;	// frame handler for C++ code that invoked us
	void			*dtor;	// pointer to destrucor for exception being handled
	ThrowType		*ttp;	// pointer to thrown object type
	char			*ThrowData; // pointer to the object thrown
	long			state;	// handler state (index into state table or -1)
	long			ebp;	// saved ebp of the Handler function
	long			ebx;	// saved ebx
	long			esi;	// saved esi
	long			edi;	// saved edi
	char			xmm[4][16] __attribute__((aligned(16)));	// saved XMM registers
	void			*rethrow; // address to continue for a rethrow
	char			terminate;// terminate if an exception is thrown
} HandlerHandler;


// Undocumented Win32 API for unwinding from an exception

extern "C" int __stdcall RtlUnwind(
	FrameHandler	*fh,	// handler in frame to unwind to
	void * retaddr,			// return address ???
	LPEXCEPTION_RECORD exc,	// the exception being handled
	long unused				// always zero for my purposes anyway
	);

// This is the handler that is in effect when the FrameHandler is running

static int HandlerForHandler(LPEXCEPTION_RECORD exc, HandlerHandler *fh, 
							LPCONTEXT context, int /*retval*/)
{
	int IsCppExc;
	
#if DEBUG
	SetupDebug();
	if (logfile)
		fprintf(logfile, "\nHandlerForHandler:  code=%p, fh=%p, context->ebp=%p\n",
				exc->ExceptionCode, fh, context->Ebp);
#endif

	IsCppExc = 0;
	if (exc->ExceptionCode == CPP_EXCEPTION_CODE &&
		exc->NumberParameters == 3 &&
		exc->ExceptionInformation[0] == MAGIC_NUMBER)
	{
	    IsCppExc = 1;
	}
	
	if (exc->ExceptionFlags & UNWINDING_MASK)
	{
		if (IsCppExc && exc->ExceptionInformation[1] != 0)
		{
	#if DEBUG
			if (logfile)
				fprintf(logfile, "HandlerForHandler: unwinding, destroying exception at %p with %p (%s)\n\n",
					fh->ThrowData, fh->dtor, LookupSymbol((int)fh->dtor));
	#endif
	
			// New C++ excpeption is unwinding us, Call the destructor
			
			if (fh->dtor)
			{
				asm
				{
					mov		eax, fh
					mov		ecx, HandlerHandler.ThrowData[eax]
					call	HandlerHandler.dtor[eax]
				}
			}
			fh->dtor = 0L;
		}
		else
		{
	#if DEBUG
			if (logfile)
				fprintf(logfile, "HandlerForHandler: unwinding, not destroying exception at %p with %p\n\n",
					fh->ThrowData, fh->dtor);
	#endif
		}
	    return 1;
	}

	// Check for a rethrow
			
	if (IsCppExc &&	
		exc->ExceptionInformation[1] == 0 && 
		exc->ExceptionInformation[2] == 0)
	{
		// Rethrow.  Restore state and go back to the original handler

	#if DEBUG
		if (logfile)
			fprintf(logfile, "HandlerForHandler: rethrow" /*"; clearing dtor"*/ "\n\n");
	#endif

		asm
		{
			mov		eax, fh
			mov		ebp, HandlerHandler.ebp[eax]
			mov		ebx, HandlerHandler.ebx[eax]
			mov		esi, HandlerHandler.esi[eax]
			mov		edi, HandlerHandler.edi[eax]
			test	procflags, _CPU_FEATURE_SSE
			jz		no_sse_0
			
			movups	xmm4, HandlerHandler.xmm[eax]
			movups	xmm5, HandlerHandler.xmm[16+eax]
			movups	xmm6, HandlerHandler.xmm[32+eax]
			movups	xmm7, HandlerHandler.xmm[48+eax]
			
		no_sse_0:
			mov		HandlerHandler.dtor[eax], 0
			jmp		HandlerHandler.rethrow[eax]
		}		
	}

	// Don't allow any other exceptions during exception handling that
	// are not handled before we get back here
	
	if (IsCppExc && fh->terminate)
	{	
	    _STD::terminate();
	    
	    // In case terminate() does not
	    
	    ExitProcess(0);
	}

	// continue

	return 1;
}


// This function is called by ___CxxFrameHandler after pushing EAX, which is the HanderHeader pointer
//
// in exc->ExceptionInformation[0] is 0x19930520 magic number
// in exc->ExceptionInformation[1] is pointer to thrown data
// in exc->ExceptionInformation[2] is pointer to throw type structure

#define RESTORE_REGISTERS(context, labnum) \
		mov		eax, context								\
	asm	mov		ebx, CONTEXT.Ebx[eax]						\
	asm	mov		esi, CONTEXT.Esi[eax]						\
	asm	mov		edi, CONTEXT.Edi[eax]						\
	asm	test	procflags, _CPU_FEATURE_SSE					\
	asm	jz		no_sse_##labnum								\
	asm	movups	xmm4, CONTEXT.ExtendedRegisters[160][4*16+eax]	\
	asm	movups	xmm5, CONTEXT.ExtendedRegisters[160][5*16+eax]\
	asm	movups	xmm6, CONTEXT.ExtendedRegisters[160][6*16+eax]\
	asm	movups	xmm7, CONTEXT.ExtendedRegisters[160][7*16+eax]\
	asm no_sse_##labnum:					

#define SAVE_REGISTERS(context, labnum) \
		mov		eax, context								\
	asm	mov		CONTEXT.Ebx[eax], ebx						\
	asm	mov		CONTEXT.Esi[eax], esi						\
	asm	mov		CONTEXT.Edi[eax], edi						\
	asm	test	procflags, _CPU_FEATURE_SSE					\
	asm	jz		no_sse_##labnum								\
	asm	movups	CONTEXT.ExtendedRegisters[160][4*16+eax], xmm4	\
	asm	movups	CONTEXT.ExtendedRegisters[160][5*16+eax], xmm5\
	asm	movups	CONTEXT.ExtendedRegisters[160][6*16+eax], xmm6\
	asm	movups	CONTEXT.ExtendedRegisters[160][7*16+eax], xmm7\
	asm no_sse_##labnum:					
	

static int MyFrameHandler(HandlerHeader *hhp, void *ret_addr, 
							LPEXCEPTION_RECORD exc, FrameHandler *fh, 
							LPCONTEXT context, int /*retval*/)
{
	int state;
	Handler *hp;
	int hi, cti, sti;
//	ThrowType *ttp;
	Catcher *ctp;
	SubTypeArray *sta;
	ThrowSubType *tstp;
	char *CatchFrame, *CatchData = 0L;
	char *ThrowData = 0L;
	void *ContinueAddress;
	HandlerHandler HH;
	HandlerHandler *UnwindTo;
	long RestoreSP;
	volatile int rethrew = 1;
	long pv;
	int is_cpp_exc = 0;
	
#if DEBUG
	SetupDebug();
	if (logfile)
		fprintf(logfile, "\nMyFrameHandler:  code=%p, hhp=%p, fh=%p, context->ebp=%p (%s)\n",
				exc->ExceptionCode, hhp, fh, context->Ebp, LookupCurrentFunc((long *)fh));
#endif

	// Check magic number to make sure we were called as expected
	
	if (hhp->magic != MAGIC_NUMBER)
	{
	    _STD::terminate();
	    ExitProcess(0);
	}

	// Establish a frame handler to be in effect during handling

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: establishing handler at %p\n", &HH);
#endif

	HH.code = &HandlerForHandler;
	HH.state = -1;
	HH.fh = fh;
	HH.dtor = 0;
	HH.ThrowData = 0L;
	HH.terminate = 1;
	asm
	{
		lea		eax, HH
		mov		HandlerHandler.ebp[eax], ebp
		mov		HandlerHandler.ebx[eax], ebx
		mov		HandlerHandler.esi[eax], esi
		mov		HandlerHandler.edi[eax], edi
		test	procflags, _CPU_FEATURE_SSE
		jz		no_sse_0
		
		movups	HandlerHandler.xmm[eax], xmm4
		movups	HandlerHandler.xmm[16+eax], xmm5
		movups	HandlerHandler.xmm[32+eax], xmm6
		movups	HandlerHandler.xmm[48+eax], xmm7
		
	no_sse_0:
		mov		HandlerHandler.rethrow[eax], OFFSET rethrow
		mov		ecx, fs:[0]
		mov		HandlerHandler.next[eax], ecx
	}
	rethrew = 0;
	
	//	we can get here through a rethrow while HandlerHandler is active
	
rethrow:

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: at %s, hhp=%p\n", rethrew ? "rethrow" : "throw", hhp);
#endif
	asm 
	{
		lea		eax, HH
		mov		fs:[0], eax
	}
	
	// See if we are unwinding
	
	if (exc->ExceptionFlags & UNWINDING_MASK)
	{
#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: unwinding\n");
#endif

		// We are unwinding, time to call all our destructors

		state = fh->state;
		
rethrow_unwind:		
		while (state != -1)
		{
			void *callback;
			
#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: unwinding state=%d\n", state);
#endif

			// See if there is a callback for this state
			
			callback = hhp->states[state].callback;
			if (callback)
			{
#if DEBUG
				if (logfile)
					fprintf(logfile, "MyFrameHandler: invoking callback at %p\n", 
							callback, LookupSymbol((int)callback));
#endif
				asm
				{
					// restore registers
					
					RESTORE_REGISTERS(context, 1)

					mov		eax, callback	; get callback address
					push	ebp				; save current ebp

					mov		ebp, fh			; set up frames ebp
					add		ebp, 12
					
					call	eax				; call the guy
					
					pop		ebp				; and restore ebp

					// save registers

					SAVE_REGISTERS(context, 2)
				}
			}
			
			// go to next state
			
			state = hhp->states[state].next;
		}

		// remove our handler
		
		asm
		{
			lea		eax, HH
			mov		eax, HandlerHandler.next[eax]
			mov		fs:[0], eax
		}

#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: exiting, return 1\n");
#endif
				
		return 1;
	}
			
	// Check to see if the exception is a C++ exception, 
	// we won't handle anything else unless catching (...)
	
	is_cpp_exc = exc->ExceptionCode == CPP_EXCEPTION_CODE;

#if DEBUG
	if (!is_cpp_exc)
	{
		if (logfile)
			fprintf(logfile, "MyFrameHandler: not C++ exception\n");
	}
#endif

	// Check magic number on it:  can't deal with malformed or foreign data
	// coming through with this exception code
	
	if (is_cpp_exc 
	&& (exc->NumberParameters != 3 || exc->ExceptionInformation[0] != MAGIC_NUMBER))
	{
	    _STD::terminate();
	    ExitProcess(0);
	}
	
	// Shit! Now we have to do some real work

	// Get the state
	
	state = fh->state;

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: throwing, state = %d\n", state);
#endif
	
	// Get the esp to restore after we have handled
	
	asm
	{
		mov		eax, fh
		mov		eax, -4[eax]
		mov		RestoreSP, eax
	}
		
	// Get the throw data
	
	HH.ttp = is_cpp_exc ? (ThrowType *)exc->ExceptionInformation[2] : 0L;
	ThrowData = HH.ThrowData = is_cpp_exc ? (char *)exc->ExceptionInformation[1] : 0L;

	// ttp == 0 on a rethrow, do not look for a handler for it, it will be handled by
	// our handler from a prior invocation that is executing the catch clause, if one
	// exists.

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: ttp = %p\n", HH.ttp);
#endif

#if 1
	//	check for rethrow
	if (is_cpp_exc && !HH.ttp)
	{
		HandlerHandler *throwframe = (HandlerHandler *)fh;
		while (!IsBadReadPtr(throwframe, 12) &&
			   (
				throwframe->code != HH.code ||
				!throwframe->ttp
			   )
			  )
		{
			throwframe = (HandlerHandler *)throwframe->next;
		}

		if (!IsBadReadPtr(throwframe, 12) && throwframe->ttp)
			HH.ttp = throwframe->ttp;

#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: found rethrow ttp = %p\n", HH.ttp);
#endif
	}
#endif
	
 	if (HH.ttp || !is_cpp_exc)
 	{	
		// Loop over all handlers, looking either a catch() block with the right type

		hp = hhp->handlers;

#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: finding one of %d handlers\n", hhp->handler_count);
#endif

		for (hi=0; hi<hhp->handler_count; hi++)
		{
			if (hp->first_state <= state && state <= hp->last_state)
			{
				// This handler is active, check all of it's catch blocks

#if DEBUG
				if (logfile)
					fprintf(logfile, "MyFrameHandler: finding one of %d catch blocks\n", hp->catch_count);
#endif
				
				ctp = hp->catches;
				for (cti=0; cti<hp->catch_count; cti++)
				{
					// check for catch(...)
					
					if (ctp->tid == 0)
					{
					    goto found_catcher;
					}
					
					if (is_cpp_exc)
					{
						// a C++ exception was thrown; check types
						
						// loop over all throw sub-types seeing if we find a match
						
						sta = HH.ttp->subtypes;
						for (sti=0; sti<sta->count; sti++)
						{
							tstp = sta->subtypes[sti];
							
							// Make sure flags are compatible.  The catcher must have
							// a reference to the thrown type (ignoring CV quals) or
							// any of TYPE_CONST and TYPE_VOLATILE set if the the thrower does
							
							if ((ctp->flags & TYPE_REFERENCE) ||
								((HH.ttp->flags & (TYPE_CONST | TYPE_VOLATILE)) & ~(ctp->flags & (TYPE_CONST | TYPE_VOLATILE))) == 0)
							{
		
								// Quick compare of address of type id
								
								if (tstp->tid == ctp->tid)
								{
								    goto found_catcher;
								}
								
								// Do the string compare
								
								if (strcmp(ctp->tid->name, tstp->tid->name) == 0)
								{
									goto found_catcher;
								}
							}
												
							// on to next subtype
						    tstp++;
						}
					}
					
					// on to next catcher
				    ctp++;
				}
			}
					
			// on to next handler
		    hp++;
		}
	}

	if (rethrew)
	{
#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: rethrowing out of function, unwinding\n");
#endif
		goto rethrow_unwind;
	}

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: no catch block found, exiting\n\n");
#endif
		
	
	// remove our handler
	
	asm
	{
		lea		eax, HH
		mov		eax, HandlerHandler.next[eax]
		mov		fs:[0], eax
	}

	// no handler return status to continue search

	return 1;	
	
found_catcher:

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: found_catcher, hp->new_state=%d, hp->first_state=%d\n",
				hp->new_state, hp->first_state);
#endif

	// at this point:
	//
	//	hp is pointer to the handler
	//  cp is pointer to the catch block

	// Before handling, see if it's catch(...) for an SEH exception,
	// which we should translate

	if (!is_cpp_exc)
	{
		// maybe translate the exception

		// Check for user SEH filter:
		// it must be executed when a C++ try...catch frame exists
		// between the thrower's EBP and the SEH frame's EBP.
		_se_translator_function func;
		
		if ((func = __user_se_translator) != 0L)
		{
			EXCEPTION_POINTERS excptrs = {exc, context};
			HH.terminate = 0;
			asm
			{
				push	ebp
				
				// push arguments
				lea		eax, excptrs
				push	eax
				mov		eax, exc
				push	EXCEPTION_RECORD.ExceptionCode[eax]
				
				// restore registers
				mov		eax, context
				mov		ebx, CONTEXT.Ebx[eax]
				mov		esi, CONTEXT.Esi[eax]
				mov		edi, CONTEXT.Edi[eax]
				
				// set up original frame
				mov		ecx, func
				mov		ebp, CONTEXT.Ebp[eax]
				call 	ecx
				
				// if that returned, assume we continue the search
				add		esp, 8
				pop		ebp
			}
		}
	}

	// Get frame pointer of the catching frame
	
	CatchFrame = (char *)fh + 12;
	
	// Set new state in the catcher frame
	
	fh->state = hp->new_state;

	// Set the lowest state handled by this handler in our handler
	
	HH.state = hp->first_state;
				
	// Find the last thing to unwind.  Walk down the stack of
	// handlers till we find a handler for a previous catch
	// in this frame at lower nesting level than the one currently
	// being caught
	
	asm
	{
		mov		eax, fs:[0]
		mov		UnwindTo, eax
	}
	
	while (UnwindTo != (HandlerHandler *)fh &&
		   (
		   	 UnwindTo == &HH ||
	         UnwindTo->code != &HandlerForHandler ||
		     UnwindTo->fh != fh ||
		     UnwindTo->state > hp->first_state
		   )
		  )
	{
		UnwindTo = (HandlerHandler *)UnwindTo->next;
	}
	
#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: unwinding to %p (%s)\n", 
			UnwindTo, LookupCurrentFunc((long *)UnwindTo));
#endif

	// Unwind any other frames
	
	asm
	{
		push	ebx
		push	esi
		push 	edi
		push	0
		push	exc
		push	ret_addr
		push	UnwindTo
		call	RtlUnwind
	ret_addr:
		pop		edi
		pop		esi
		pop		ebx
	}

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: after unwinding, reestablishing handler at %p\n", UnwindTo);
#endif

	// Reestablish our handler, since he got unwound
	
	HH.next = (FrameHandler *)UnwindTo;
	asm
	{
		lea		eax, HH
		mov		fs:[0], eax
	}
	
	// Now unwind this frame until the state is lower than
	// the state that established this handler
	
	while (state > hp->first_state)
	{
		void *callback;
		
#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: secondary unwind, state=%d\n", state);
#endif

		// See if there is a callback for this state
		
		callback = hhp->states[state].callback;
		if (callback)
		{
#if DEBUG
			if (logfile)
				fprintf(logfile, "MyFrameHandler: secondary unwind, callback at %p (%s)\n", 
						callback, LookupSymbol((int)callback));
#endif
			
			asm
			{
				// restore registers
				
				RESTORE_REGISTERS(context, 3)

				mov		eax, callback	; get callback address
				push	ebp				; save current ebp
				
				mov		ebp, fh			; set up frames ebp
				add		ebp, 12
				
				call	eax				; call the guy
				
				pop		ebp				; and restore ebp

				// save registers
				
				SAVE_REGISTERS(context, 4)
			}
		}
		
		// go to next state
		
		state = hhp->states[state].next;
	}

	// If catcher has a type, we need to move the argument
	
	if (is_cpp_exc && ctp->tid)
	{

		if (HH.ttp)
		{		
			// Get the throw data address
			
			//ThrowData = (char *)exc->ExceptionInformation[1];
			//HH.ThrowData = ThrowData;
			
			// Adjust this address to address the sub-type
			
			if (tstp->flags & SUBTYPE_VBASE)
			{
				//??
			}
			else if ((tstp->flags & SUBTYPE_NON_CLASS))
			{
				if (tstp->offset != 0)
				{
				    // pointer to class thrown and pointer to base class caught,
				    // we must adjust the pointer
				    
				    pv = *(long *)ThrowData + tstp->offset;
				    ThrowData = (char *)&pv;
				}
			}
			else
			{
				// Adjust address to point to base class
				
			    ThrowData += tstp->offset;
			}
		}

#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler: moving catch argument\n");
#endif
		
		// Copy data to the argument
		
		CatchData = CatchFrame + ctp->offset;

		if (ctp->flags & TYPE_REFERENCE)
		{
	#if DEBUG
			if (logfile)
				fprintf(logfile, "MyFrameHandler: copying reference to %p\n", ThrowData);
	#endif

			// copy the address
			
			*(long *)(CatchData) = (long)ThrowData;

			HH.dtor = HH.ttp->dtor;
			//HH.ThrowData = ThrowData;
		}
		else if (tstp->cctor)
		{
	#if DEBUG
			if (logfile)
				fprintf(logfile, "MyFrameHandler: copy construct %p = %p with %p (%s)\n", 
					CatchData, ThrowData, tstp->cctor, LookupSymbol((int)tstp->cctor));
	#endif

			// Additional arg for vbase constructors?
			if (tstp->flags & 4)
			{
				asm		push	1
			}
			asm
			{
				push	ThrowData
			    mov		ecx, CatchData

			    mov		eax, tstp
			    call	ThrowSubType.cctor[eax]
			}

			// Set this in our handler so we can destroy the guy if we unwind through
			
			HH.dtor = HH.ttp->dtor;
			HH.ThrowData = CatchData;
		}
		else
		{
	#if DEBUG
			if (logfile)
				fprintf(logfile, "MyFrameHandler: physically copy %p = %p\n", 
					CatchData, ThrowData);
	#endif
			memmove(CatchFrame + ctp->offset, ThrowData, tstp->size);

			// Set this in our handler so we can destroy the guy if we unwind through
			
			HH.dtor = HH.ttp->dtor;
			HH.ThrowData = ThrowData;
		}
	}
	
#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: done handling exception, about to execute catch block\n");
#endif

	// Exception is now handled, don't terminate if a new exception is thrown
	
	HH.terminate = 0;	
	
	// Zap the unwinding flags
	
	exc->ExceptionFlags &= ~UNWINDING_MASK;
	
	// and call the catch block

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: invoking catch block at %p (%s)\n", 
			ctp->catchcode, LookupSymbol((int)ctp->catchcode));
#endif

	rethrew = 1;
		
	asm
	{
		// restore registers

		RESTORE_REGISTERS(context, 5)
		
		mov 	eax, ctp

		push	ebp
		mov		ebp, CatchFrame
		call	Catcher.catchcode[eax]
		
		// eax now has the continue address
		
		pop		ebp
		mov		ContinueAddress, eax

		// save registers

		SAVE_REGISTERS(context, 6)
	}

	rethrew = 0;
	
	// Destroy the exception if necessary

	if (is_cpp_exc && HH.ttp->dtor && ThrowData)
	{	
		if (ThrowData == CatchData)
		{
			// pass destruction responsibility to re-thrown exception
	#if DEBUG
			if (logfile)
				fprintf(logfile, "MyFrameHandler: passing destruction of %p at %p to rethrow [state=%d]\n", 
					ThrowData, HH.ttp->dtor, state);
	#endif
		}
		else
		{
	#if DEBUG
			if (logfile)
			{
				fprintf(logfile, "MyFrameHandler: destroying exception at %p with %p (%s)\n", 
					ThrowData, HH.ttp->dtor, LookupSymbol((int)HH.ttp->dtor));
			    fflush(logfile);
			}
	#endif

			// Set this in our handler so we can destroy the guy if we unwind through
			
			HH.dtor = HH.ttp->dtor;
			HH.ThrowData = ThrowData;

			// Do terminate if the destructor throws
			
			HH.terminate = 1;

		    // destroy the thrown exception
		    asm
		    {
		    	lea		eax, HH
		    	mov		ecx, HandlerHandler.ThrowData[eax]
		    	call	HandlerHandler.dtor[eax]
		    }
		    HH.dtor = 0;
		    ThrowData = 0;
		}
	}
	else if (is_cpp_exc && HH.ttp->dtor)
	{
#if DEBUG
		if (logfile)
			fprintf(logfile, "MyFrameHandler:  not destroying exception object at %p\n", ThrowData);
#endif	
	}
	
	// And continue execution after the catcher

#if DEBUG
	if (logfile)
		fprintf(logfile, "MyFrameHandler: continuing after catcher to %p (%s)\n",
				ContinueAddress, LookupSymbol((int)ContinueAddress));
#endif
	
	asm
	{
		lea		eax, HH				; remove our handler
		mov		eax, HandlerHandler.next[eax]
		mov		fs:[0], eax
		
		cmp		eax, fh				; are we in a nested catch?
		jne		noloadesp			; if so, cannot yet reload esp
			
		mov		ecx, RestoreSP		; get new ESP setting
		mov		esp, ecx			; restore handled frames esp
		
	noloadesp:

		// restore registers

		RESTORE_REGISTERS(context, 7)
	
		mov		eax, ContinueAddress; get address to continue execution
				
		mov		ebp, CatchFrame		; restore handled frames ebp
		
		jmp		eax					; and go there, as if nothing has happened
	}	
	
	// never gets here
	
}


// This function is called by each frame as the system checks for a handler for
// a structured exception.
extern "C" int _MSL_IMP_EXP_RUNTIME __declspec(naked) 
__CxxFrameHandler(LPEXCEPTION_RECORD /*exc*/, void */*exc_stack*/, 
				LPCONTEXT /*context*/, int /*retval*/);
extern "C" int _MSL_IMP_EXP_RUNTIME __declspec(naked) 
__CxxFrameHandler(LPEXCEPTION_RECORD /*exc*/, void */*exc_stack*/, 
				LPCONTEXT /*context*/, int /*retval*/)
{	
	// We get passed the following:
	// In EAX is the pointer to our frame handler data
	asm
	{
		
		push	eax
		call	MyFrameHandler
				
		// If he returned, no handler, continue search
		
		add		esp, 4
	    retn
	}
}



// Throw a C++ excpeption
extern "C" _MSL_IMP_EXP_RUNTIME void __stdcall _CxxThrowException(void *ThrowData, ThrowType *tt);
extern "C" _MSL_IMP_EXP_RUNTIME void __stdcall _CxxThrowException(void *ThrowData, ThrowType *tt)
{
    DWORD Arguments[3];
    
#if DEBUG
	SetupDebug();
	if (logfile)
		if (tt)
			fprintf(logfile, "CxxThrowException: td=%p tt=%x/%p/(%d)\n", ThrowData, tt->flags, tt->dtor, tt->subtypes->count);
		else
			fprintf(logfile, "CxxThrowException: td=%p tt=NULL\n", ThrowData);
#endif

    Arguments[0] = MAGIC_NUMBER;
    Arguments[1] = (DWORD)ThrowData;
    Arguments[2] = (DWORD)tt;
    
	RaiseException(CPP_EXCEPTION_CODE, EXCEPTION_NONCONTINUABLE, 3, Arguments);
}

#if 0
#pragma mark -
#endif

/*
 *	Frame handler for __try...__except
 *
 *	The compiler sets up the Win32 exception frame with:
 *
 *		push <trylevel>
 *		push <state table>
 *		push <__SEHFrameHandler>
 *		push dword ptr fs:[0]
 *
 *	<state table> is an array of TryExceptState structs, which contain
 *	pointers to the code for the __except() filter and the __except { } code.
 *	The initial state is the outer trylevel to unwind to, ending at -1.
 */

#define EXCEPTION_NONCONTINUABLE	0x1
#define EXCEPTION_UNWINDING			0x2
#define EXCEPTION_EXIT_UNWIND		0x4
#define EXCEPTION_STACK_INVALID		0x8
#define EXCEPTION_NESTED_CALL		0x10

#define TRYLEVEL_STOP				0xffffffff
 
// one element of state table
struct TryExceptState
{
	long	nexttrylevel;	// level to skip to when unwinding
	int		(*filter)();	// call-able code for filter expression, or NULL for __finally
	void	(*handler)();	// jmp-able code for __except or __finally handler
};

// frame handler layout (on stack)
struct TryExceptFrame
{
	struct 	TryExceptFrame	*outer;
	void	(*handler)();
	struct 	TryExceptState	*state;
	int		trylevel;
	char	*ebp;
};

// unwinding frame handler layout (on stack)
struct TryExceptUnwindFrame
{
	struct 	TryExceptFrame	*outer;
	void	(*handler)();
	int		trylevel;
	char	*frame;
};

//	Unwind states in state table
static EXCEPTION_DISPOSITION
__cdecl __SEHUnwindHandler(LPEXCEPTION_RECORD exc, 
					struct TryExceptFrame *frame, 
					LPCONTEXT /*context*/, 
					struct TryExceptFrame **frameptr)
{
	if (exc->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND))
	{
		// short-circuit unwinding
		*frameptr = frame;
		return ExceptionCollidedUnwind;
	}
	return ExceptionContinueSearch;
}

static void SEHUnwindState(struct TryExceptFrame *frame, LPCONTEXT context, int trylevel)
{
	asm 
	{
		// setup handler for unwinder
		push 	frame
		push 	-2						; dummy try level
		push	offset __SEHUnwindHandler
		push	dword ptr fs:[0]
		mov		dword ptr fs:[0], esp
	}
	
	while (frame->trylevel != TRYLEVEL_STOP
		&& frame->trylevel != trylevel)
	{
		struct TryExceptState *state = &frame->state[frame->trylevel];
		register int nexttry = state->nexttrylevel;
		void (*handler) = !state->filter ? state->handler : 0;
		
		// patch try level for handler
		asm mov dword ptr [esp+8], nexttry
		frame->trylevel = nexttry;
		
		// call __finally handler
		if (handler)
		{
			if (frame->trylevel == TRYLEVEL_STOP)
			asm
			{
				// restore registers
				mov		eax, context
				mov		ebx, CONTEXT.Ebx[eax]
				mov		esi, CONTEXT.Esi[eax]
				mov		edi, CONTEXT.Edi[eax]
			}

			asm
			{
				push 	ebp

				mov		eax, handler
				
				// get EBP for excepting function's context
				mov 	ebp, frame
				lea 	ebp, TryExceptFrame.ebp[ebp]
				call 	eax

				pop 	ebp
			}

			if (frame->trylevel == TRYLEVEL_STOP)
			asm
			{
				// save registers
				mov		eax, context
				mov		CONTEXT.Ebx[eax], ebx
				mov		CONTEXT.Esi[eax], esi
				mov		CONTEXT.Edi[eax], edi
			}
		}
	}
	
	asm 
	{
		// remove unwinder handler
		pop		dword ptr fs:[0]
		pop		ecx
		pop		ecx
		pop		ecx
	}
}

extern "C" _MSL_IMP_EXP_RUNTIME EXCEPTION_DISPOSITION
__cdecl __SEHFrameHandler(LPEXCEPTION_RECORD exc, 
					struct TryExceptFrame *frame, 
					LPCONTEXT context, 
					int /*retval*/);

extern "C" _MSL_IMP_EXP_RUNTIME EXCEPTION_DISPOSITION 
__cdecl __SEHFrameHandler(LPEXCEPTION_RECORD exc, 
					struct TryExceptFrame *frame, 
					LPCONTEXT context, 
					int /*retval*/)
{
	// unwinding?
	if (exc->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND))
	{
		// unwind all frames
		SEHUnwindState(frame, context, TRYLEVEL_STOP);
		return ExceptionContinueSearch;
	}
	else
	{
		EXCEPTION_POINTERS excptrs;	
		int trylevel;
		struct TryExceptState *state;
		_se_translator_function func;
		
		// Set up info for GetExceptionInformation in called functions
		excptrs.ExceptionRecord = exc;
		excptrs.ContextRecord = context;

		// Check for user SEH filter:
		// it must be executed when a C++ try...catch frame exists
		// between the thrower's EBP and the SEH frame's EBP.
		if (!(exc->ExceptionFlags & (EXCEPTION_NESTED_CALL|EXCEPTION_UNWINDING)) 
		&& 	(func = __user_se_translator) != 0L)
		{
			DWORD *EBPFrame = (DWORD *)context->Ebp;
			while (!IsBadReadPtr(EBPFrame, sizeof(DWORD *)*2) 
			&& (DWORD *)*EBPFrame != (DWORD *)frame->ebp)
			{
				// get return address
				DWORD retaddr = *(EBPFrame+1);
				
				// see if we have a zero-overhead catch() associated
				if (ExX86_FindCatcher((char *)retaddr))
				{
					// if so, we allow user's SE translator to throw whatever exception
					// might be caught

					// Unwind frame, saving registers that Win32 kindly trashes for us
					asm 
					{
						push	ebx
						push	esi
						push 	edi
						push	0
						push	exc
						push	ret_addr0
						push	frame
						call	RtlUnwind
					ret_addr0:
						pop		edi
						pop		esi
						pop		ebx
					}
					
					asm
					{
						push	ebp
						
						// push arguments
						lea		eax, excptrs
						push	eax
						mov		eax, exc
						push	EXCEPTION_RECORD.ExceptionCode[eax]
						
						// restore registers
						mov		eax, context
						mov		ebx, CONTEXT.Ebx[eax]
						mov		esi, CONTEXT.Esi[eax]
						mov		edi, CONTEXT.Edi[eax]
						
						// set up original frame
						mov		ecx, func
						mov		ebp, CONTEXT.Ebp[eax]
						call 	ecx
						
						// if that returned, assume we continue the search
						add		esp, 8
						pop		ebp
					}
					break;
				}
				
				// step up through the stack
				EBPFrame = (DWORD *)*EBPFrame;
			}
		}
		
		// setup pointer to SEH into in handler frame, fixed at [EBP-0x14]
		*(long *)(((char *)&frame->ebp) - 0x14) = (long)&excptrs;

		trylevel = frame->trylevel;
		state = frame->state;
		
		// top of function?
		while (trylevel != TRYLEVEL_STOP)
		{
			// __try...__except?
			if (state[trylevel].filter)
			{
				int retval;
				int (*filter)() = state[trylevel].filter;
				void (*handler)() = state[trylevel].handler;
				
				// execute filter
				asm
				{
					push	ebp
					
					// restore registers
					mov		eax, context
					mov		ebx, CONTEXT.Ebx[eax]
					mov		esi, CONTEXT.Esi[eax]
					mov		edi, CONTEXT.Edi[eax]
					
					// set up handler frame
					mov		eax, filter
					mov		ebp, frame
					lea 	ebp, TryExceptFrame.ebp[ebp]
					call 	eax		// returns in EAX

					pop		ebp
					mov		retval, eax

					// save changed registers
					mov		eax, context
					mov		CONTEXT.Ebx[eax], ebx
					mov		CONTEXT.Esi[eax], esi
					mov		CONTEXT.Edi[eax], edi
					
				}
				
				if (retval <= EXCEPTION_CONTINUE_EXECUTION)
				{
					return ExceptionContinueExecution;
				}
				else if (retval >= EXCEPTION_EXECUTE_HANDLER)
				{
					// Unwind frame, saving registers that Win32 kindly trashes for us
					asm 
					{
						push	ebx
						push	esi
						push 	edi
						push	0
						push	exc
						push	ret_addr
						push	frame
						call	RtlUnwind
					ret_addr:
						pop		edi
						pop		esi
						pop		ebx
					}
					
					// unwind frames skipped to this point
					SEHUnwindState(frame, context, trylevel);
					
					// update trylevel for called code
					frame->trylevel = state[trylevel].nexttrylevel;

					// call __except block, which does not return:
					// it resets ESP on entry
					asm
					{
						// restore registers
						mov		eax, context
						mov		ebx, CONTEXT.Ebx[eax]
						mov		esi, CONTEXT.Esi[eax]
						mov		edi, CONTEXT.Edi[eax]

						mov		eax, handler
						mov		ebp, frame
						lea 	ebp, TryExceptFrame.ebp[ebp]
						call 	eax	
						
						// should not get here	
						int3		
					}
				}
			}
			
			// continue up through the __try blocks in this function
			state = frame->state;
			trylevel = state[trylevel].nexttrylevel;
		}
		
		// Reached end of states
		return ExceptionContinueSearch;
	}
}

//	Intrinsic callback for GetExceptionCode
asm unsigned long __cdecl _exception_code(void);
asm unsigned long __cdecl _exception_code(void)
{
	asm
	{
		mov eax, [ebp-0x14]
		mov eax, EXCEPTION_POINTERS.ExceptionRecord[eax]
		mov eax, EXCEPTION_RECORD.ExceptionCode[eax]
		ret
	}
}

//	Intrinsic callback for GetExceptionInformation
_MSL_IMP_EXP_RUNTIME asm void * __cdecl _exception_info(void);
_MSL_IMP_EXP_RUNTIME asm void * __cdecl _exception_info(void)
{
	asm
	{
		mov eax, [ebp-0x14]
		ret
	}
}

//	Intrinsic callback for AbnormalTermination,
// 	used to tell whether a __finally block was called
//	due to a fallthrough of the previous block (not abnormal)
//	or due to an exception being thrown or a jump from that block
// 	(abnormal).  
//
//	NOTE:  Currently we don't support this correctly for
//	jumps out of the block, which are treated as fallthroughs.

_MSL_IMP_EXP_RUNTIME asm int __cdecl _abnormal_termination(void);
_MSL_IMP_EXP_RUNTIME asm int __cdecl _abnormal_termination(void)
{
	asm
	{
		// are we unwinding at the moment?
		// if not, no possible way an exception caused __finally to be called
		mov eax, 0
		mov edx, fs:[0]
		mov ebx, TryExceptUnwindFrame.handler[edx]
		cmp ebx, offset __SEHUnwindHandler
		jne done
		// else, see if we're unwinding from the same try block
		mov ebx, TryExceptUnwindFrame.frame[edx]
		mov ebx, TryExceptFrame.trylevel[ebx]
		cmp ebx, TryExceptUnwindFrame.trylevel[edx]
		sete al
	done:
		ret
	}
}

//	Setup SEH --> C++ exception mapping function
_MSL_IMP_EXP_RUNTIME _se_translator_function 
_MSL_CDECL _set_se_translator( _se_translator_function se_trans_func )
{
	_se_translator_function old = __user_se_translator;
	__user_se_translator = se_trans_func;
	return old;
}

#if 0
#pragma mark -
#endif

#if DEBUG

typedef struct SymList
{
	struct SymList *next;
	char *symbol;
	long addr;
}	SymList;

typedef struct SymEntry
{
	char *symbol;
	long addr;
}	SymEntry;

SymEntry *syms;
int numsyms;

static void SetupDebug(void)
{
	if (!logfile)
	{
		logfile=fopen("\\logfile.txt", "wb");
		if (logfile)
		{
			char path[256],line[256]; 
			FILE *map;
			
			// open link map
			GetModuleFileName(0, path, 256);
			strcat(path, ".MAP");
			map = fopen(path, "r");
			if (map)
			{
				long addr, size;
				SymList *symhead = 0L, *symlast = 0L, *sym;
				
				// read and ignore first two lines
				fgets(line, 256, map);
				fgets(line, 256, map);
				while (!feof(map))
				{
					fgets(line, 256, map);
					if (*line == '\r' || *line == '\n')
						break;
					if (sscanf(line, "%x %x", &addr, &size) == 2)
					{
						// symbol is at end, between ()
						char *ptr = strchr(line, '('), *end = strrchr(line, ')');
						if (ptr && end)
						{
							ptr++;
							if (end > ptr + 255)
								end = ptr + 255;
						
							sym = (SymList *)_alloca(sizeof(SymList));
							sym->next = 0L;
							sym->addr = addr;
							sym->symbol = (char *)_alloca(end-ptr+1);
							strncpy(sym->symbol, ptr, end-ptr);
							sym->symbol[end-ptr] = 0;

							if (symlast)
							{
								symlast = symlast->next = sym;
							}
							else
							{
								symhead = symlast = sym;
							}
							numsyms++;
							
							//fprintf(logfile, "got addr=%x, symbol=%s\n", sym->addr, sym->symbol);
						}
					}
				}
				fclose(map);
				
				if (symhead)
				{
					SymEntry *ptr;
					
					// make linear array
					ptr = syms = (SymEntry *)malloc(sizeof(SymEntry) * numsyms);
					while (symhead)
					{
						ptr->addr = symhead->addr;
						ptr->symbol = (char *)malloc(strlen(symhead->symbol)+1);
						strcpy(ptr->symbol, symhead->symbol);
						symhead = symhead->next;
						ptr++;
					}
				}
			}
		}
	}
}

static SymEntry *LookupEntry(int addr)
{
	int min, max;
	
	// binary search for the return address
	min = 0;
	max = numsyms;
	while (min + 1 != max)
	{
	   int mid = (max + min) >> 1;
	   
	   // See if it is in the range
	   if (syms[mid].addr <= addr)
	   {
	       min = mid;
	   }
	   else
	   {
	       max = mid;
	   }
	}
	// min is index of correct entry

	return &syms[min];
}

static char *LookupSymbol(int addr)
{
	if (!syms) return "???";
	if (!addr) return "(null)";

	SymEntry *ent = LookupEntry(addr);
	static char symbol[256+16];
	
	sprintf(symbol, "%.256s+0x%x (0x%x)", ent->symbol, addr - ent->addr, addr);
	return symbol;
}

static char *LookupCurrentFunc(long *framehandler)
{
	if (!syms) return "???";

	// get address of inter-function handler from stack
	SymEntry *ent = LookupEntry(framehandler[1]);
	return ent->symbol;
}

#endif

/* Change record:
 * cc  011128 Made __tls
 * ejs 011129 Added _MSL_IMP_EXP_RUNTIME to __SEHFrameHandler
 * JWW 020130 Changed _MWMT to _MSL_THREADSAFE for consistency's sake
 * ejs 020404 Cleanup and some register sanity
 * ejs 020416 More error checking, register saving, etc.
 * ejs 020507 Fixes for _set_se_translator() (only called when try...catch available)
 */