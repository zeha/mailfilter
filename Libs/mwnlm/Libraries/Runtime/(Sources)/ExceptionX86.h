/* Metrowerks x86 Runtime Support Library 
 * Copyright © 1995-2002 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2002/09/10 16:37:46 $
 * $Revision: 1.4 $
 */

#ifndef _EXCEPTX86_H
#define _EXCEPTX86_H

#include <ansi_parms.h>

#pragma pack(push, 1)

#ifdef __cplusplus    // hh 971207 Added namespace support
	extern "C" {
#endif 

//	This data structue is created for each function and is pointed to by the function table
typedef struct ExceptionRecord {
	char    		register_mask;		//	register restore mask
	long			actions_offset;		//  offset from function start to the action table
	unsigned short  num_offsets;		//  number of offset/action pairs, which must follow
}	ExceptionRecord;

#pragma pack(pop)

//	action table definitions:

typedef unsigned short exaction_type;

//	register bits and values (note: FP and MMX registers never saved)

#define REG_EBX 	3
#define REG_ESI		6
#define REG_EDI		7

#define REG_XMM4	1
#define REG_XMM5	2
#define REG_XMM6	4
#define REG_XMM7	5

#define EXLREFREGMASK					0xFFFFFF00	//	local register ref mask
#define EXLREFREGBITS					0x80000000	//	local register bits

#define EXLREFEBXFRAME					0x80000000	//	if unset, EBX is base for locals

#define EXACTION_ENDBIT					0x8000	//	action table end bit
#define EXACTION_MASK					0x00FF	//	action mask

#define EXACTION_ENDOFLIST				0x0000	//	end of action list
#define EXACTION_ERROR					0x00FF	//	error

#pragma pack(push, 2)

#define EXACTION_DESTROYLOCAL			0x0001	//	destroy local 
typedef struct ex_destroylocal {
	exaction_type	action;						//	EXACTION_DESTROYLOCAL
	long			local;						//	reference to local variable (frame offset)
	void			*dtor;						//	pointer to destructor function
}	ex_destroylocal;

#define EXACTION_DESTROYLOCALCOND		0x0002	//	destroy local 
typedef struct ex_destroylocalcond {
	exaction_type	action;						//	EXACTION_DESTROYLOCALCOND
	long			local;						//	reference to local variable (frame offset)
	//[ EJS 000801: these two items have been generated in (swapped) order since Pro3!
	void			*dtor;						//	pointer to destructor function
	long			cond;						//	reference to conditional variable (frame offset or register)
	//]
}	ex_destroylocalcond;

//#define EXACTION_DESTROYLOCALOFFSET	0x0003	//	destroy local 
//typedef struct ex_destroylocaloffset {
//	exaction_type	action;						//	EXACTION_DESTROYLOCALOFFSET
//	long			local;						//	reference to local variable
//	void			*dtor;						//	pointer to destructor function
//	long			offset;						//	offset
//}	ex_destroylocaloffset;

#define EXACTION_DESTROYLOCALPOINTER	0x0004	//	destroy local 
typedef struct ex_destroylocalpointer {
	exaction_type	action;						//	EXACTION_DESTROYLOCALPOINTER
	long			pointer;					//	reference to local pointer
	void			*dtor;						//	pointer to destructor function
}	ex_destroylocalpointer;

#define EXACTION_DESTROYLOCALARRAY		0x0005	//	destroy local 
typedef struct ex_destroylocalarray {
	exaction_type	action;						//	EXACTION_DESTROYLOCALPOINTER
	long			localarray;					//	reference to local pointer
	void			*dtor;						//	pointer to destructor function
	long			elements;					//	number of array elements
	long			element_size;				//	size of one array element
}	ex_destroylocalarray;

#define EXACTION_DESTROYPARTIALARRAY	0x0006	//	destroy local 
typedef struct ex_destroypartialarray {
	exaction_type	action;						//	EXACTION_DESTROYPARTIALARRAY
	long			arraypointer;				//	reference to array pointer
	long			arraycounter;				//	reference to array counter
	long			dtor;						//	pointer to destructor function
	long			element_size;				//	size of one array element
}	ex_destroypartialarray;

#define EXACTION_DESTROYMEMBER			0x0007	//	destroy member
#define EXACTION_DESTROYBASE			0x000C	//  destroy base class
typedef struct ex_destroymember {
	exaction_type	action;						//	EXACTION_DESTROYMEMBER
	long			objectptr;					//	pointer to complete object pointer
	void			*dtor;						//	pointer to destructor function
	long			offset;						//	offset of member in complete object
}	ex_destroymember;

#define EXACTION_DESTROYMEMBERCOND		0x0008	//	destroy member
typedef struct ex_destroymembercond {
	exaction_type	action;						//	EXACTION_DESTROYMEMBERCOND
	long			objectptr;					//	pointer to complete object pointer
	long			cond;						//	pointer to condition flag object
	void			*dtor;						//	pointer to destructor function
	long			offset;						//	offset of member in complete object
}	ex_destroymembercond;

#define EXACTION_DESTROYMEMBERARRAY		0x0009	//	destroy member array
typedef struct ex_destroymemberarray {
	exaction_type	action;						//	EXACTION_DESTROYMEMBERARRAY
	long			objectptr;					//	pointer to complete object pointer
	void			*dtor;						//	pointer to destructor function
	long			offset;						//	offset of member in complete object
	long			elements;					//	number of array elements
	long			element_size;				//	size of one array element
}	ex_destroymemberarray;

#define EXACTION_DELETEPOINTER			0x000A	//	delete pointer
typedef struct ex_deletepointer {
	exaction_type	action;						//	EXACTION_DELETEPOINTER
	long			pointerobject;				//	pointer object reference
	void			*deletefunc;				//	pointer to delete function
}	ex_deletepointer;

#define EXACTION_DELETEPOINTERCOND		0x000B	//	delete pointer
typedef struct ex_deletepointercond {
	exaction_type	action;						//	EXACTION_DELETEPOINTERCOND
	long			pointerobject;				//	pointer object reference
	void			*deletefunc;				//	pointer to delete function
	long			cond;						//	reference to conditional variable
}	ex_deletepointercond;

#define EXACTION_CATCHBLOCK				0x0010	//	catch block
typedef struct ex_catchblock {
	exaction_type	action;						//	EXACTION_CATCHBLOCK
	char			*catch_type;				//	pointer to catch type info (0L: catch(...))
	long			catch_pcoffset;				//	function relative catch label offset
	long			cinfo_ref;					//	reference to local CatchInfo struct
}	ex_catchblock;

#define EXACTION_ACTIVECATCHBLOCK		0x0011	//	active catch block
typedef struct ex_activecatchblock {
	exaction_type	action;						//	EXACTION_ACTIVECATCHBLOCK
	long			cinfo_ref;					//	reference to local CatchInfo struct
}	ex_activecatchblock;

#define EXACTION_TERMINATE				0x0012	//	active catch block
typedef struct ex_terminate {
	exaction_type	action;						//	EXACTION_TERMINATE
}	ex_terminate;


#define EXACTION_SPECIFICATION			0x0013	//	active catch block
typedef struct ex_specification {
	exaction_type	action;						//	EXACTION_SPECIFICATION
	unsigned short	specs;						//	number of specializations (0-n)
	long			pcoffset;					//	function relative offset
	long			cinfo_ref;					//	reference to local CatchInfo struct
	char			*spec[];					//	array of catch type pointers
}	ex_specification;

#define EXACTION_ABORTINIT				0x0014	//	abort a thread-safe initialization
typedef struct ex_abortinit {
	exaction_type	action;						//	EXACTION_ABORTINIT
	void			*guardvar;					//	guard variable
}	ex_abortinit;


// Struct of the function mapping table

typedef struct FunctionTableEntry
{
    char *Pc;
    char *exctable;
} FunctionTableEntry;

// Struct used to register exception tables

typedef struct ExceptionTableHeader
{
	FunctionTableEntry *First;
	FunctionTableEntry *Last;
	struct ExceptionTableHeader *Next;
} ExceptionTableHeader;



// Register and deregister exception tables for an app or dll
_MSL_IMP_EXP_RUNTIME extern void _MSL_CDECL _RegisterExceptionTables(ExceptionTableHeader *header);
_MSL_IMP_EXP_RUNTIME extern void _MSL_CDECL _UnRegisterExceptionTables(ExceptionTableHeader *header);

// Pointers to the start and end of the function mapping table

#if _WIN32
extern FunctionTableEntry _PcToActionStart, _PcToActionEnd;
#else
extern FunctionTableEntry __PcToActionStart, __PcToActionEnd;
#endif

// Private structs/functions

int ExX86_FindCatcher(char *retaddr);


#ifdef __cplusplus    // hh 971207 Added namespace support
	}
#endif

#pragma pack(pop)

//	code -> action mapping tables

#define EXX86_DIRECTSTOREOFFSET	0x80000000L	//	direct store offset bit

#endif

// hh  971207 Added namespace support
// ejs 000801  reversed "cond" and "dtor" in ex_destroylocalcond
// ejs 020507 Fixes for _set_se_translator() (only called when try...catch available)
// ejs 020910 Add EAT_ABORTINIT action for thread-safe static local initializers
