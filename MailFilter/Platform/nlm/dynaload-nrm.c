//
// (C) Copyright 2004 Christian Hofstaedtler
//

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#define N_PLAT_NLM
#include <nwadv.h>
#include <nwthread.h>

#include "nrm.h"
//
static int (*dynaload_HttpSendDataSprintf) (HINTERNET hrequest, const char *szData, ...);
static int (*dynaload_HttpSendErrorResponse) (HINTERNET hrequest, UINT32 HTTP_ERROR_CODE);
static int (*dynaload_HttpSendSuccessfulResponse) (HINTERNET hrequest, void *pzContentType);
static int (*dynaload_HttpEndDataResponse) (HINTERNET hRequest);
static BOOL (*dynaload_HttpReturnRequestMethod) (HINTERNET hndl, UINT32_PTR httpRequestMethodType);
static BOOL (*dynaload_RegisterServiceMethodEx) (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, void *tableOfContentsStruc, UINT32 requestedRights, void *pReserved, UINT32 (*pServiceMethod)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode);
static BOOL (*dynaload_DeRegisterServiceMethod) (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, UINT32 (*pServiceMethodCheck)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode);
static UINT (*dynaload_BuildAndSendHeader) (HINTERNET handle, char *windowTitle, char *pageIdentifier, char Refresh, UINT32	refreshDelay, UINT flags, void (*AddHeaderText)(HINTERNET	handle), char *bodyTagText, char *helpURL);
static UINT32 (*dynaload_BuildAndSendFooter) (HINTERNET hndl);
static char *(*dynaload_HttpReturnString)(UINT32 stringType);

int DLSetupNRM()
{
	dynaload_HttpSendDataSprintf = ImportSymbol((int)GetNLMHandle(),"HttpSendDataSprintf");
					if (dynaload_HttpSendDataSprintf == NULL) goto handle_dynaload_err;
	dynaload_HttpSendErrorResponse = ImportSymbol((int)GetNLMHandle(),"HttpSendErrorResponse");
					if (dynaload_HttpSendErrorResponse == NULL) goto handle_dynaload_err;
	dynaload_HttpSendSuccessfulResponse = ImportSymbol((int)GetNLMHandle(),"HttpSendSuccessfulResponse");
					if (dynaload_HttpSendSuccessfulResponse == NULL) goto handle_dynaload_err;
	dynaload_HttpEndDataResponse = ImportSymbol((int)GetNLMHandle(),"HttpEndDataResponse");
					if (dynaload_HttpEndDataResponse == NULL) goto handle_dynaload_err;
	dynaload_RegisterServiceMethodEx = ImportSymbol((int)GetNLMHandle(),"RegisterServiceMethodEx");
					if (dynaload_RegisterServiceMethodEx == NULL) goto handle_dynaload_err;
	dynaload_DeRegisterServiceMethod = ImportSymbol((int)GetNLMHandle(),"DeRegisterServiceMethod");
					if (dynaload_DeRegisterServiceMethod == NULL) goto handle_dynaload_err;
	dynaload_BuildAndSendHeader = ImportSymbol((int)GetNLMHandle(),"BuildAndSendHeader");
					if (dynaload_BuildAndSendHeader == NULL) goto handle_dynaload_err;
	dynaload_BuildAndSendFooter = ImportSymbol((int)GetNLMHandle(),"BuildAndSendFooter");
					if (dynaload_BuildAndSendFooter == NULL) goto handle_dynaload_err;
	dynaload_HttpReturnString = ImportSymbol((int)GetNLMHandle(),"HttpReturnString");
					if (dynaload_HttpReturnString == NULL) goto handle_dynaload_err;
	dynaload_HttpReturnRequestMethod = ImportSymbol((int)GetNLMHandle(),"HttpReturnRequestMethod");
					if (dynaload_HttpReturnRequestMethod == NULL) goto handle_dynaload_err;
	
	return 1;
	
handle_dynaload_err:
	fprintf(stderr,"MAILFILTER: ERROR: Could not get required symbol handles.\n\tPlease load PORTAL.NLM and HTTPSTK.NLM\n");
	DLDeSetupNRM();
	return 0;
}

int DLDeSetupNRM()
{
	if (dynaload_HttpSendDataSprintf != NULL)
			UnimportSymbol((int)GetNLMHandle(),"HttpSendDataSprintf");
	if (dynaload_HttpSendErrorResponse != NULL)
			UnimportSymbol((int)GetNLMHandle(),"HttpSendErrorResponse");
	if (dynaload_HttpSendSuccessfulResponse != NULL)
			UnimportSymbol((int)GetNLMHandle(),"HttpSendSuccessfulResponse");
	if (dynaload_HttpEndDataResponse != NULL)
			UnimportSymbol((int)GetNLMHandle(),"HttpEndDataResponse");
	if (dynaload_RegisterServiceMethodEx != NULL)
			UnimportSymbol((int)GetNLMHandle(),"RegisterServiceMethodEx");
	if (dynaload_DeRegisterServiceMethod != NULL)
			UnimportSymbol((int)GetNLMHandle(),"DeRegisterServiceMethod");
	if (dynaload_BuildAndSendHeader != NULL)
			UnimportSymbol((int)GetNLMHandle(),"BuildAndSendHeader");
	if (dynaload_BuildAndSendFooter != NULL)
			UnimportSymbol((int)GetNLMHandle(),"BuildAndSendFooter");
	if (dynaload_HttpReturnString != NULL)
			UnimportSymbol((int)GetNLMHandle(),"HttpReturnString");
	if (dynaload_HttpReturnRequestMethod != NULL)
			UnimportSymbol((int)GetNLMHandle(),"HttpReturnRequestMethod");

	return 1;
}

char* DL_HttpReturnString (UINT32 stringType)
{
	char* dynaloadReturnValue;
	if (dynaload_HttpReturnString == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_HttpReturnString) (stringType);
	return dynaloadReturnValue;
}
//
int DL_HttpSendData (HINTERNET hrequest, const char *szData)
{
	int dynaloadReturnValue;
	if (dynaload_HttpSendDataSprintf == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_HttpSendDataSprintf) (hrequest, "%s", szData);
	return dynaloadReturnValue;
}
//
int DL_HttpSendErrorResponse(HINTERNET hrequest, UINT32 HTTP_ERROR_CODE)
{
	int dynaloadReturnValue;
	if (dynaload_HttpSendErrorResponse == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_HttpSendErrorResponse) (hrequest, HTTP_ERROR_CODE);
	return dynaloadReturnValue;
}
//
int DL_HttpSendSuccessfulResponse (HINTERNET hrequest, void *pzContentType)
{
	int dynaloadReturnValue;
	if (dynaload_HttpSendSuccessfulResponse == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_HttpSendSuccessfulResponse) (hrequest, pzContentType);
	return dynaloadReturnValue;
}
//
int DL_HttpEndDataResponse (HINTERNET hRequest)
{
	int dynaloadReturnValue;
	if (dynaload_HttpEndDataResponse == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_HttpEndDataResponse) (hRequest);
	return dynaloadReturnValue;
}
//
BOOL DL_HttpReturnRequestMethod(HINTERNET hRequest, UINT32_PTR httpRequestMethodType)
{
	BOOL dynaloadReturnValue;
	if (dynaload_HttpReturnRequestMethod == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_HttpReturnRequestMethod) (hRequest,httpRequestMethodType);
	return dynaloadReturnValue;
}
//
BOOL DL_RegisterServiceMethodEx (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, void *tableOfContentsStruc, UINT32 requestedRights, void *pReserved, UINT32 (*pServiceMethod)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode)
{
	BOOL dynaloadReturnValue;
	if (dynaload_RegisterServiceMethodEx == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_RegisterServiceMethodEx) (pzServiceName, pServiceTag, serviceTagLength, tableOfContentsStruc, requestedRights, pReserved, pServiceMethod, pRTag, pReturnCode);
	return dynaloadReturnValue;
}
//
BOOL DL_DeRegisterServiceMethod (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, UINT32 (*pServiceMethodCheck)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode)
{
	BOOL dynaloadReturnValue;
	if (dynaload_DeRegisterServiceMethod == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_DeRegisterServiceMethod) (pzServiceName, pServiceTag, serviceTagLength, pServiceMethodCheck, pRTag, pReturnCode);
	return dynaloadReturnValue;
}
//
UINT DL_BuildAndSendHeader (HINTERNET handle, char *windowTitle, char *pageIdentifier, char Refresh, UINT32	refreshDelay, UINT flags, void (*AddHeaderText)(HINTERNET	handle), char *bodyTagText, char *helpURL)
{
	UINT dynaloadReturnValue;
	if (dynaload_BuildAndSendHeader == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_BuildAndSendHeader) (handle,windowTitle,pageIdentifier,Refresh,refreshDelay,flags,AddHeaderText,bodyTagText,helpURL);
	return dynaloadReturnValue;
}
//
UINT32 DL_BuildAndSendFooter (HINTERNET hndl)
{
	UINT32 dynaloadReturnValue;
	if (dynaload_BuildAndSendFooter == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_BuildAndSendFooter) (hndl);
	return dynaloadReturnValue;
}
//
