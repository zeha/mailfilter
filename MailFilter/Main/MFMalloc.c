#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netware.h>
#include <library.h>
#include <malloc.h>
#include <errno.h>
#include "MFVersion.h"

static void *(*malloc_sym) (size_t length_);
static void *(*calloc_sym) (size_t n, size_t size);
static void* myNlmHandle = 0;
   
int __init_malloc()
{
	myNlmHandle = getnlmhandle();
	malloc_sym = (ImportPublicObject(myNlmHandle,"LIBC@malloc"));
	if (malloc_sym == NULL)
	{
		printf("MAILFILTER: Malloc Library FAILED to load.\n");
		return ENOTSUP;
	}
		
	calloc_sym = (ImportPublicObject(myNlmHandle,"LIBC@calloc"));
	if (calloc_sym == NULL)
	{
		printf("MAILFILTER: Malloc Library FAILED to load.\n");
		return ENOTSUP;
	}

	printf("MAILFILTER: Malloc Library loaded.\n");
	return 0;
}

int __deinit_malloc()
{
	if (calloc_sym)
		UnImportPublicObject(getnlmhandle(),"LIBC@calloc");
	if (malloc_sym)
		UnImportPublicObject(getnlmhandle(),"LIBC@malloc");

	printf("MAILFILTER: Malloc Library unloaded.\n");
	return 0;
}

void* malloc(size_t length)
{
	void* p = NULL;
	int cnt = 0;

	// under PAL MailFilter is very slow
	// if PAL is not used, this is not necessary
#ifdef MAILFILTER_VERSION_YESITCRASHES

#ifdef __NOVELL_LIBC__
	NXThreadYield();
#else
	ThreadSwitch();
#endif

#endif

	if (malloc_sym == NULL)
		return NULL;

	while (cnt < 20)
	{
		p = malloc_sym(length);
		if (p)
			return p;
	}
	printf("MAILFILTER: out of memory.\n");
	return NULL;
}

void* calloc(size_t n, size_t size)
{
	void* p = NULL;
	int cnt = 0;

	if (calloc_sym == NULL)
		return NULL;

	while (cnt < 20)
	{
		//p = malloc_sym(length);
		p = calloc_sym(n,size);
		if (p)
			return p;
	}
	printf("MAILFILTER: out of memory.\n");
	return NULL;
}


