#include <malloc.h>
#include <string.h>

extern	int			MFD_AllocCounts					;
extern	int			MFD_AllocCountsCPP					;

// MALLOC TRACEING MACROS
static inline void *_mfd_nt_malloc(size_t size, const char* szFuncName)
{
#pragma unused(szFuncName)
	return malloc(size);
}
static inline char *_mfd_nt_strdup(const char * str, const char* szFuncName)
{
#pragma unused(szFuncName)
	return strdup(str);
}
static inline void _mfd_nt_free(void *ptr, const char* szFuncName)
{	
#pragma unused(szFuncName)
	free(ptr);	
}

#ifdef _MF_MEMTRACE
static inline void _mfd_cpp_allocX( const char* szArea, size_t approxSize )
{	++MFD_AllocCountsCPP;
//	consoleprintf("MAILFILTER: ++ALLC in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCountsCPP,szArea,approxSize);
}
static inline void _mfd_cpp_freeX( const char* szArea, size_t approxSize )
{	--MFD_AllocCountsCPP;
//	consoleprintf("MAILFILTER: ++FREE in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCountsCPP,szArea,approxSize);
}

static inline void _mfd_free( void *ptr , const char* szFuncName )
{
	--MFD_AllocCounts;
#ifdef __NOVELL_LIBC__
	consoleprintf("MAILFILTER: FREE   in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCounts,szFuncName,msize(ptr));
#else
	consoleprintf("MAILFILTER: FREE   in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCounts,szFuncName,_msize(ptr));
#endif
	free(ptr);
}
static inline void *_mfd_malloc( size_t size , const char* szFuncName )
{
	++MFD_AllocCounts;
	consoleprintf("MAILFILTER: MALLOC in "_MFD_MODULE" ; count: %d ; %s ; %d\n",MFD_AllocCounts,szFuncName,size);
	return malloc(size);
}
static inline char *_mfd_strdup( const char * str , const char* szFuncName )
{
	++MFD_AllocCounts;
	ConsolePrintf("MAILFILTER: STRDUP in "_MFD_MODULE" ; count: %d ; %s\n",MFD_AllocCounts,szFuncName);
	return strdup(str);
}

#undef malloc
#undef free
#undef strdup
#define malloc(x)						_mfd_malloc(x,"")
#define free(x)							_mfd_free(x,"")
#define strdup(x)						_mfd_strdup(x,"")
#define _mfd_cpp_alloc					_mfd_cpp_allocX
#define _mfd_cpp_free					_mfd_cpp_freeX

#else
#define _mfd_malloc(x,y)				malloc(x)
#define _mfd_free(x,y)					free(x)
#define _mfd_strdup(x,y)				strdup(x)
#define _mfd_cpp_alloc					0
#define _mfd_cpp_free					0
#endif 

