//
// (C) Copyright 2004 Christian Hofstaedtler
//
// NLM: LibC

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#ifndef __cplusplus
#error Cplusplus required
#endif

#include "nrm.h"
//
extern int DL_HttpSendData (HINTERNET hrequest, const char *szData);
//
extern int DL_HttpSendErrorResponse(HINTERNET hrequest, UINT32 HTTP_ERROR_CODE);
//
extern BOOL DL_HttpReturnRequestMethod(HINTERNET hndl, UINT32_PTR httpRequestMethodType);
//
extern int DL_HttpSendSuccessfulResponse (HINTERNET hrequest, void *pzContentType);
//
extern int DL_HttpEndDataResponse (HINTERNET hRequest);
//
extern BOOL DL_RegisterServiceMethodEx (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, void *tableOfContentsStruc, UINT32 requestedRights, void *pReserved, UINT32 (*pServiceMethod)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode);
//
extern BOOL DL_DeRegisterServiceMethod (const char *pzServiceName, const char *pServiceTag, int serviceTagLength, UINT32 (*pServiceMethodCheck)(HINTERNET,void *pExtraInfo,UINT32 szExtraInfo,UINT32 InformationBits), void *pRTag, UINT32 *pReturnCode);
//
extern UINT DL_BuildAndSendHeader (HINTERNET handle, char *windowTitle, char *pageIdentifier, char Refresh, UINT32	refreshDelay, UINT flags, void (*AddHeaderText)(HINTERNET	handle), char *bodyTagText, char *helpURL);
//
extern UINT32 DL_BuildAndSendFooter (HINTERNET hndl);
//
extern char *DL_HttpReturnString(UINT32 stringType);
//
extern int DL_HttpReturnPathBuffers(HINTERNET hrequest, UINT32_PTR lpszPathBufs, char **path, char **cnvpath);

extern int DLSetupNRM();
extern int DLDeSetupNRM();

