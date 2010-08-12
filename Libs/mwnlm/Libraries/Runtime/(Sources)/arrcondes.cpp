/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.5 $
 */

#define __NOSTRING__	//	do not include <string>

#include <ansi_parms.h>
#include <stdlib.h>
#include <CPlusLib.h>
#include <exception.h>
#include <NMWException.h>

/************************************************************************/
/*	Purpose..: 	Initialize a new allocated array of objects				*/
/*	Input....:	pointer to allocated memory (+8 bytes) (0L: error)		*/
/*	Input....:	pointer to default constructor function	(or 0L)			*/
/*	Input....:	pointer to destructor function (or 0L)					*/
/*	Input....:	size of one array element								*/
/*	Input....:	number of array elements								*/
/*	Return...:	pointer to first object in array						*/
/************************************************************************/
extern void *__cdecl __construct_new_array(void *block,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n)
{
	char	*ptr;

	if((ptr=(char *)block)!=0L)
	{
		//	store number of allocated objects and size of one object at the beginnig of the allocated block
		*(size_t *)ptr=size;
		*(size_t *)(ptr+sizeof(size_t))=n;
		ptr+=sizeof(size_t)*2;

		if(ctor)
		{	//	call constructor to initialize array
			char	*p;
			size_t	i;

			#pragma exception_arrayinit
			//	this #pragma adds partial array construction exception action (ptr,dtor,size,i)

			for(i=0,p=ptr; i<n; i++,p+=size)
			{
				asm
				{
					mov		esi, esp
		        	mov		ecx, p
		        	push	1
		        	call	dword ptr ctor
		        	mov		esp, esi
				}
			}
		}
	}
	return ptr;	//	return pointer to first object;
}



/************************************************************************/
/*	Purpose..: 	Construct an array of objects							*/
/*	Input....:	pointer to array memory									*/
/*	Input....:	pointer to default constructor function					*/
/*	Input....:	pointer to destructor function (or 0L)					*/
/*	Input....:	size of one array element								*/
/*	Input....:	number of array elements								*/
/*	Return...:	---														*/
/************************************************************************/
extern void __cdecl __construct_array(void *ptr,ConstructorDestructor ctor,ConstructorDestructor dtor,size_t size,size_t n)
{
	char	*p;
	size_t	i;

	#pragma exception_arrayinit
	//	this #pragma adds partial array construction exception action (ptr,dtor,size,i)

	for(i=0,p=(char *)ptr; i<n; i++,p+=size)
	{
				asm
				{
					mov		esi, esp
					push	1
		        	mov		ecx, p
		        	call	dword ptr ctor
		        	mov		esp, esi
				}
	}
}



/************************************************************************/
/*	Purpose..: 	Destroy an array of objects								*/
/*	Input....:	pointer to array memory									*/
/*	Input....:	pointer to destructor function							*/
/*	Input....:	size of one object										*/
/*	Input....:	number of objects										*/
/*	Return...:	---														*/
/************************************************************************/
extern void __cdecl __destroy_arr(void *block,ConstructorDestructor dtor,size_t size,size_t n)
{
	char	*p;

	for(p=(char *)block+size*n; n>0; n--)
	{
		p-=size;
		asm
		{
			mov		esi, esp
			mov		ecx, p
			call	dword ptr dtor
			mov		esp, esi
		}
	}
}

// JWW 010329 Moved __destroy_new_array to destroy.cpp
// EJS 020319 Added __cdecl
