/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/08/19 17:46:09 $
 * $Revision: 1.5 $
 */

#ifndef __CPLUSLIB__
#define __CPLUSLIB__

#include <ansi_parms.h>
#include <stddef.h>

//
//	#define CABI_ZEROOFFSETVTABLE		1
//		
//	forces vtable pointer to point to the first virtual function pointer,
//	the RTTI header will be at offset -sizeof(RTTIVTableHeader)
//	This flag must be set in CABI.c and in MWRTTI.cp
//

#define CABI_ZEROOFFSETVTABLE		1

#define ARRAY_HEADER_SIZE	(2*sizeof(size_t))


//
//	invisible constructor/destructor argument handling
//

#define CTORARG_TYPE			int
#define CTORARG_PARTIAL			(0)				//	construct non-virtual bases
#define CTORARG_COMPLETE		(1)				//	construct all bases	

#define CTORCALL_COMPLETE(ctor,objptr)\
	(((void (*)(void *,CTORARG_TYPE))ctor)(objptr,CTORARG_COMPLETE))

#define DTORARG_TYPE			int
#define DTORARG_DELETEFLAG		0x0001			//	delete after destruction
#define DTORARG_VECTORFLAG		0x0002			//	array destruction

#define DTORCALL_PARTIAL(dtor,objptr)\
	(((void (*)(void *))dtor)(objptr))

typedef void *ConstructorDestructor;	//	constructor/destructor function	pointer

typedef struct PTMF {
	long	this_delta;					//	delta to this pointer
	long	vtbl_offset;				//	offset of virtual function pointer in vtable (<0: non-virtual function address)
	union {
		void	*func_addr;				//	non-virtual function address
		long	ventry_offset;			//	offset of vtable pointer in class
	}	func_data;
}	PTMF;

#ifdef __cplusplus
	extern "C" {
#endif

// Type of a destructor
typedef void (*destructor)(void *object, int Free);

// Data structure for pointers to member functions
typedef struct MemberFuncPointerType {
	long	this_delta;				//	delta to this pointer
	long	vtbl_offset;			//	offset to vtable (-1: not a virtual function)
	union {
		void	*func_addr;			//	nonvirtual function address
		long	ventry_offset;		//	offset of virtual function entry in vtable
	}	func_data;
} MemberFuncPointerType;

int __cdecl 						__ptmf_test(MemberFuncPointerType *ptmf);
int __cdecl 						__ptmf_cmpr(MemberFuncPointerType *p1, MemberFuncPointerType *p2);
void * __cdecl 						__ptmf_cast(int delta, MemberFuncPointerType *in, MemberFuncPointerType *out);
void * __cdecl 						__copy(void *dest, void *source, unsigned long len);
void * __cdecl 						__clear(void *to,size_t size);
void __declspec(naked) * __cdecl 	__ptmf_call();
void __declspec(naked) * __cdecl 	__ptmf_scall();
void __declspec(naked) *__cdecl 	__ptmf_scalls();
int __cdecl 						__ptmf_test(MemberFuncPointerType *ptmf);

_MSL_IMP_EXP_RUNTIME extern void __cdecl  	__construct_array(void *block,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n);
_MSL_IMP_EXP_RUNTIME extern void __cdecl 	__destroy_arr(void *block,ConstructorDestructor dtor,size_t size,size_t n);
_MSL_IMP_EXP_RUNTIME extern void *__cdecl 	__construct_new_array(void *block,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n);

extern void __cdecl 	__destroy_new_array(void *block,ConstructorDestructor dtor);
extern void	*__cdecl 	__destroy_new_array2(void *block,ConstructorDestructor dtor);
extern void	__cdecl 	__destroy_new_array3(void *block,ConstructorDestructor dtor,void *dealloc_func,short has_size_t_param);

typedef void (*constructor)(void *);

void *__cdecl 	__new_arr(constructor c, unsigned int size, unsigned int num); 
void __cdecl 	__del_arr(char *ptr, destructor d);

extern int __cdecl __cxa_guard_acquire ( long long *guard_object );
extern void __cdecl __cxa_guard_release ( long long *guard_object );
extern void __cdecl __cxa_guard_abort ( long long *guard_object );

#ifdef __cplusplus
	}
#endif

#endif

// hh 971207 Added support for namespaces.  This involved getting everything prototyped properly
// JWW 010329 Removed 68K support and added ARRAY_HEADER_SIZE (previously in NMWException.cp)
// JWW 010402 Made __destroy_new_array variants not _MSL_IMP_EXP_RUNTIME since they're static now// EJS 020319 Cleaned up
// EJS 020319 Added __cdecl, removed non-intel stuff
// EJS 020819 Added one-time init API

