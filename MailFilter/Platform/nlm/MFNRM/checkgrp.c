#define _NETWARE_
#define NETWARE_386

	#include <stddef.h>

	#include <stdio.h>
	#include <stdlib.h>

//	#include <fcntl.h>
	#include <unistd.h>
// #include <nwfattr.h>


	#include <ntypes.h>

//#define _NWNSPACE_H_
#include <nwconio.h>
	#include <string.h>
	#include <nwdos.h>

	#include <ctype.h>
	#include <malloc.h>
	#include <nwcalls.h>
	#include <nwnet.h>
	#include <nwdsdc.h>

	
//extern NWDSCCODE NWCallsInit( nptr reserved1, nptr reserved2);


int checkGroupMembership(char* group, NWDSContextHandle dContext)
{

	nstr8					objectName[MAX_DN_CHARS];
	NWCCODE 			cCode;
	NWDSCCODE 			dscCode;
	NWDS_BUFFER			*inBuf;
	DWORD					flags;
	NWSYNTAX_ID            syntaxID;
	nbool8				matchGrp = 0;
	nbool8				matchUsr = 0;
	int					myRC = 2;
//	NWDSContextHandle dContext;

	cCode=NWCallsInit(NULL,NULL);
	if(cCode)	      /* initialize allowing to call nwcalls functions */
	{
		return(1);
	}
	/*-----------------------------------------------------------------------
	**	Create Context
	*/
/*	dscCode = NWDSCreateContextHandle(&dContext);
	if(dscCode < 0)
	{
		ConsolePrintf("MFNRM: NWDSCreateContextHandle failed.\n");
		return(1);
	}
*/
	/*----------------------------------------------------------------
	**	Get the current directory context flags so we can modify them.
	*/
	dscCode = NWDSGetContext(
				dContext,
				DCK_FLAGS,
				&flags
				);
				
//	ConsolePrintf("MFNRM: error %d at NWDSGetContext(%d)\n",dscCode,dContext);

	if (dscCode < 0)
		{
		ConsolePrintf("MFNRM: NWDSGetContext failed.\n");
		myRC = 3;
		goto _FreeContext;
		}

	/*-------------------------------------------------------------------
	**   Turn typeless naming on.
	**   Turn canonicalize names off.  This means we will get full names.
	*/

	flags |= DCV_TYPELESS_NAMES;
	flags &= ~DCV_CANONICALIZE_NAMES;

	/*----------------------------------------------------------------
	**	Set the directory context flags so they take effect.
	*/
	dscCode = NWDSSetContext(
				/* Context Handle */ dContext,
				/* Key            */ DCK_FLAGS,
				/* Set Flag Value */ &flags
				);

	if (dscCode < 0)
		{
		ConsolePrintf("MFNRM: NWDSSetContext failed.\n");

		myRC = 4;
		goto _FreeContext;
		}

	/*----------------------------------------------------------------
	**	Set dContext to root so DN of object can be passed in to AddObject.
	*/
	dscCode = NWDSSetContext(
				/* Context Handle */ dContext,
				/* Key            */ DCK_NAME_CONTEXT,
				/* Set Flag Value */ (char*)DS_ROOT_NAME
				);

	if (dscCode < 0)
		{
		myRC = 5;
		goto _FreeContext;
		}

	dscCode = NWDSWhoAmI(
				/* > Context */ dContext,
				/* < Obj name*/ objectName
				);


	if (dscCode < 0)
		{
		ConsolePrintf("MFNRM: NWDSWhoAmI failed.\n");
		myRC = 6;
		goto _FreeContext;
		}


	ConsolePrintf("I am: %s\n",objectName);

	dscCode=NWDSAllocBuf(DEFAULT_MESSAGE_LEN, &inBuf);
	if ( dscCode < 0 )
		{
		ConsolePrintf("MFNRM: NWDSAllocBug failed.\n");
		myRC = 7;
		goto _FreeContext;
		}

	dscCode=NWDSInitBuf(dContext, DSV_COMPARE, inBuf);
	if ( dscCode < 0 )
		{
		ConsolePrintf("MFNRM: NWDSInitBuf failed.\n");
		myRC = 8;
		goto _FreeBuf;
		}

	/*---------------------------------------------------------------------
	** Get syntaxID 
	*/
	dscCode = NWDSGetSyntaxID(dContext, "Member", &syntaxID);
	if ( dscCode < 0 )
		{
		ConsolePrintf("MFNRM: NWDSGetSyntaxID failed.\n");
		myRC = 9;
		goto _FreeBuf;
		}

	/*---------------------------------------------------------------------
	** Modify Test_Attribute 
	*/
	
	dscCode = NWDSPutAttrName(dContext, inBuf, "Member");
	if ( dscCode < 0 )
		{
		ConsolePrintf("MFNRM: NWDSPutAttrName failed.\n");
		myRC = 10;
		goto _FreeBuf;
		}

	dscCode = NWDSPutAttrVal(dContext, inBuf, syntaxID, objectName);
	if ( dscCode < 0 )
		{
		ConsolePrintf("MFNRM: NWDSPutAttrVal failed.\n");
		myRC = 11;
		goto _FreeBuf;
		}

	dscCode= NWDSCompare(dContext, group, inBuf, &matchGrp);

	if ( dscCode < 0 )
		{
		ConsolePrintf("MFNRM: NWDSCompare failed.\n");
		myRC = 12;
		goto _FreeBuf;
		}

	/*----------------------------------------------------------
	** Now check to ensure that User is member of group
	*/
/*	dscCode=NWDSInitBuf(dContext, DSV_COMPARE, inBuf);
	if ( dscCode < 0 )
		{
		myRC = 13;
		goto _FreeBuf;
		}
*/
	/*---------------------------------------------------------------------
	** Get syntaxID 
	*/
/*	dscCode = NWDSGetSyntaxID(dContext, "Group Membership", &syntaxID);
	if ( dscCode < 0 )
		{
		myRC = 14;
		goto _FreeBuf;
		}
		*/
	/*---------------------------------------------------------------------
	** Modify Test_Attribute 
	*/
/*	
	dscCode = NWDSPutAttrName(dContext, inBuf, "Group Membership");
	if ( dscCode < 0 )
		{
		myRC = 15;
		goto _FreeBuf;
		}

	dscCode = NWDSPutAttrVal(dContext, inBuf, syntaxID, group);
	if ( dscCode < 0 )
		{
		myRC = 16;
		goto _FreeBuf;
		}

	dscCode= NWDSCompare(dContext, objectName, inBuf, &matchUsr);

	if ( dscCode < 0 )
		{
		myRC = 17;
		goto _FreeBuf;
		}		*/

	if(matchGrp) // && matchUsr)
		{
		ConsolePrintf("MFNRM: group match.\n");
		NWDSFreeBuf(inBuf);
		NWDSFreeContext(dContext);
		return(0);
		}
	else 
		{
		ConsolePrintf("MFNRM: no group match.\n");
		NWDSFreeBuf(inBuf);
		NWDSFreeContext(dContext);
		return(1);
		}

	_FreeBuf:
		NWDSFreeBuf(inBuf);
	_FreeContext:
		NWDSFreeContext(dContext);

return(myRC);
}
