/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/07/01 21:13:40 $
 * $Revision: 1.2 $
 */

#include <CPlusLib.h>

/************************************************************************/
/*	Purpose..: 	Delete an array of objects								*/
/*	Input....:	pointer to first object	(can be NULL)					*/
/*	Input....:	pointer to destructor function							*/
/*	Return...:	---														*/
/************************************************************************/
extern void __cdecl __destroy_new_array(void *block,ConstructorDestructor dtor)
{
	if(block)
	{
		if(dtor)
		{
			size_t	i,objects,objectsize;
			char	*p;

			objectsize=*(size_t *)((char *)block-2*sizeof(size_t));
			objects=*(size_t *)((char *)block-sizeof(size_t));
			p=(char *)block+objectsize*objects;
			for(i=0; i<objects; i++)
			{
				p-=objectsize;
				asm
				{
					mov		esi, esp
					mov		ecx, p
					push	-1
					call	dword ptr dtor
					mov		esp, esi
				}
			}
		}
		::operator delete[] ((char *)block-2*sizeof(size_t));
	}
}

/************************************************************************/
/*	Purpose..: 	Destroy a new allocated array of objects				*/
/*	Input....:	pointer to first object	(can be NULL)					*/
/*	Input....:	pointer to destructor function (not NULL)				*/
/*	Return...:	pointer to complete array block							*/
/************************************************************************/
extern void *__cdecl __destroy_new_array2(void *block,ConstructorDestructor dtor)
{
	size_t	i,objects,objectsize;
	char	*p;

	if(block==NULL) return NULL;

	if(dtor)
	{	//	destroy array members
		objectsize=*(size_t *)((char *)block-2*sizeof(size_t));
		objects=*(size_t *)((char *)block-sizeof(size_t));
		p=(char *)block+objectsize*objects;
		for(i=0; i<objects; i++)
		{
			p-=objectsize;
			asm
			{
				mov		esi, esp
				mov		ecx, p
				push	-1
				call	dword ptr dtor
				mov		esp, esi
			}
		}
	}

	//	return pointer to complete array block
	return (char *)block-2*sizeof(size_t);
}

/************************************************************************/
/*	Purpose..: 	Destroy/delete a new allocated array of objects			*/
/*	Input....:	pointer to first object	(or NULL)						*/
/*	Input....:	pointer to destructor function (or NULL)				*/
/*	Input....:	pointer to deallocation function (or NULL)				*/
/*	Input....:	1: deallocation has size_t parameter					*/
/*	Return...:	pointer to complete array block							*/
/************************************************************************/
extern void __cdecl __destroy_new_array3(
	void *block,
	ConstructorDestructor dtor,
	void *dealloc_func,
	short has_size_t_param)
{
	size_t	i,elements,elementsize;
	char	*p;

	if(block)
	{
		elements	= *(size_t *)((char *)block-sizeof(size_t));
		elementsize	= *(size_t *)((char *)block-2*sizeof(size_t));

		if(dtor!=NULL)
		{	//	destroy array elements
			p=(char *)block+elements*elementsize;
			for(i=0; i<elements; i++)
			{
				p-=elementsize;
				asm
				{
					mov		esi, esp
					mov		ecx, p
					push	-1
					call	dword ptr dtor
					mov		esp, esi
				}
			}
		}

		if(dealloc_func!=NULL)
		{	//	call deallocation functiuon
			p=(char *)block-2*sizeof(size_t);
			if(has_size_t_param)
			{
				((void (*)(void *,size_t))dealloc_func)(p,elements*elementsize+2*sizeof(size_t));
			}
			else ((void (*)(void *))dealloc_func)(p);
		}
	}
}

// JWW 010329 Moved __destroy_new_array out of arrcondes.cpp
// ejs 020319 Added __cdecl
