/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:39 $
 * $Revision: 1.3 $
 */

#ifndef __MWEXCEPTIONS__
#define __MWEXCEPTIONS__

#include <ansi_parms.h>

////	[this section needs to be copied from CException.h

typedef short	vbase_ctor_arg_type;
typedef char	local_cond_type;

typedef struct CatchInfo {
	void			*location;				//	pointer to complete exception object
	void			*typeinfo;				//	pointer to complete exception object's typeinfo
	void			*dtor;					//	pointer to complete exception object's destructor function (0: no destructor)
	void			*sublocation;			//	pointer to handlers subobject in complete exception object
	long			pointercopy;			//	adjusted pointer copy for pointer matches (sublocation will point to the loaction)
	void			*stacktop;				//	pointer to handlers stack top (reused for exception specification in specification handler)
}	CatchInfo;

////	[this section needs to be copied from CException.h]

typedef struct DestructorChain {			//	global destructor chain
	struct DestructorChain	*next;			//	pointer to next destructor chain element
	void					*destructor;	//	pointer to destructor function
	void					*object;		//	pointer to memory location (0L: element is marker)
}	DestructorChain;

#ifdef __cplusplus
extern "C" {
#endif

extern DestructorChain	*__global_destructor_chain;
extern void				*__cdecl __register_global_object(void *object,void *destructor,void *regmem);

_MSL_IMP_EXP_RUNTIME extern asm void	__cdecl __init__catch(CatchInfo* catchinfo);
_MSL_IMP_EXP_RUNTIME extern asm void	__cdecl __end__catch(CatchInfo* catchinfo);
_MSL_IMP_EXP_RUNTIME extern asm void	__cdecl __throw(char *throwtype,void *location,void *dtor);
_MSL_IMP_EXP_RUNTIME extern char		__cdecl __throw_catch_compare(const char *throwtype,const char *catchtype,long *offset_result);
_MSL_IMP_EXP_RUNTIME extern void		__cdecl __unexpected(CatchInfo* catchinfo);

/*	Other exception stuff */

#if _WIN32
	#if _MSL_THREADSAFE
	#include <ThreadLocalData.h>
	#define __user_se_translator	(_se_translator_function)(_GetThreadLocalData(_MSL_TRUE)->user_se_translator)
	#else
	extern _MSL_TLS _se_translator_function __user_se_translator;				/*- cc 011128 -*/
	#endif
#endif

#ifdef __cplusplus
}
#endif
#endif
