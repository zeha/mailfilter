//
// (C) Copyright 2004 Christian Hofstaedtler
//
// NLM: LibC

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netware.h>
#include <library.h>

#include "nrm.h"

//
static int (*dynaload_HttpSendDataSprintf) (HINTERNET hrequest, const char *szData, ...);
static int (*dynaload_HttpSendErrorResponse) (HINTERNET hrequest, UINT32 HTTP_ERROR_CODE);
static int (*dynaload_HttpSendSuccessfulResponse) (HINTERNET hrequest, void *pzContentType);
static int (*dynaload_HttpEndDataResponse) (HINTERNET hRequest);
static BOOL (*dynaload_RegisterServiceMethodEx) (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, void *tableOfContentsStruc, UINT32 requestedRights, void *pReserved, UINT32 (*pServiceMethod)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode);
static BOOL (*dynaload_DeRegisterServiceMethod) (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, UINT32 (*pServiceMethodCheck)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode);
static UINT (*dynaload_BuildAndSendHeader) (HINTERNET handle, char *windowTitle, char *pageIdentifier, char Refresh, UINT32	refreshDelay, UINT flags, void (*AddHeaderText)(HINTERNET	handle), char *bodyTagText, char *helpURL);
static UINT32 (*dynaload_BuildAndSendFooter) (HINTERNET hndl);
static char *(*dynaload_HttpReturnString)(UINT32 stringType);
static BOOL (*dynaload_HttpReturnRequestMethod) (HINTERNET hndl, UINT32_PTR httpRequestMethodType);

int DLSetupNRM()
{
	dynaload_HttpSendDataSprintf = ImportPublicObject(getnlmhandle(),"HttpSendDataSprintf");
					if (dynaload_HttpSendDataSprintf == NULL) goto handle_dynaload_err;
	dynaload_HttpSendErrorResponse = ImportPublicObject(getnlmhandle(),"HttpSendErrorResponse");
					if (dynaload_HttpSendErrorResponse == NULL) goto handle_dynaload_err;
	dynaload_HttpSendSuccessfulResponse = ImportPublicObject(getnlmhandle(),"HttpSendSuccessfulResponse");
					if (dynaload_HttpSendSuccessfulResponse == NULL) goto handle_dynaload_err;
	dynaload_HttpEndDataResponse = ImportPublicObject(getnlmhandle(),"HttpEndDataResponse");
					if (dynaload_HttpEndDataResponse == NULL) goto handle_dynaload_err;
	dynaload_RegisterServiceMethodEx = ImportPublicObject(getnlmhandle(),"RegisterServiceMethodEx");
					if (dynaload_RegisterServiceMethodEx == NULL) goto handle_dynaload_err;
	dynaload_DeRegisterServiceMethod = ImportPublicObject(getnlmhandle(),"DeRegisterServiceMethod");
					if (dynaload_DeRegisterServiceMethod == NULL) goto handle_dynaload_err;
	dynaload_BuildAndSendHeader = ImportPublicObject(getnlmhandle(),"BuildAndSendHeader");
					if (dynaload_BuildAndSendHeader == NULL) goto handle_dynaload_err;
	dynaload_BuildAndSendFooter = ImportPublicObject(getnlmhandle(),"BuildAndSendFooter");
					if (dynaload_BuildAndSendFooter == NULL) goto handle_dynaload_err;
	dynaload_HttpReturnString = ImportPublicObject(getnlmhandle(),"HttpReturnString");
					if (dynaload_HttpReturnString == NULL) goto handle_dynaload_err;
	dynaload_HttpReturnRequestMethod = ImportPublicObject(getnlmhandle(),"HttpReturnRequestMethod");
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
			UnImportPublicObject(getnlmhandle(),"HttpSendDataSprintf");
	if (dynaload_HttpSendErrorResponse != NULL)
			UnImportPublicObject(getnlmhandle(),"HttpSendErrorResponse");
	if (dynaload_HttpSendSuccessfulResponse != NULL)
			UnImportPublicObject(getnlmhandle(),"HttpSendSuccessfulResponse");
	if (dynaload_HttpEndDataResponse != NULL)
			UnImportPublicObject(getnlmhandle(),"HttpEndDataResponse");
	if (dynaload_RegisterServiceMethodEx != NULL)
			UnImportPublicObject(getnlmhandle(),"RegisterServiceMethodEx");
	if (dynaload_DeRegisterServiceMethod != NULL)
			UnImportPublicObject(getnlmhandle(),"DeRegisterServiceMethod");
	if (dynaload_BuildAndSendHeader != NULL)
			UnImportPublicObject(getnlmhandle(),"BuildAndSendHeader");
	if (dynaload_BuildAndSendFooter != NULL)
			UnImportPublicObject(getnlmhandle(),"BuildAndSendFooter");
	if (dynaload_HttpReturnString != NULL)
			UnImportPublicObject(getnlmhandle(),"HttpReturnString");
	if (dynaload_HttpReturnRequestMethod != NULL)
			UnImportPublicObject(getnlmhandle(),"HttpReturnRequestMethod");

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
int DL_HttpSendErrorResponse (HINTERNET hrequest, UINT32 HTTP_ERROR_CODE)
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
BOOL DL_HttpReturnRequestMethod (HINTERNET hrequest, UINT32_PTR methodType)
{
	BOOL dynaloadReturnValue;
	if (dynaload_HttpReturnRequestMethod == NULL)
		return NULL; // symbol not found, sorry!
	dynaloadReturnValue = (*dynaload_HttpReturnRequestMethod) (hrequest, methodType);
	return dynaloadReturnValue;
}
