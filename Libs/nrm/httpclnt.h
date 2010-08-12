/*****************************************************************************
 *
 * (C) 2003 Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 *
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked or uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 ****************************************************************************/

#ifndef __HTTPCLNT_H__
#define __HTTPCLNT_H__
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef __UINT__
#define __UINT__

#ifndef UINT
typedef unsigned int	UINT;
#endif

#ifndef UINT8
typedef unsigned char	UINT8;
#endif

#ifndef UINT16
typedef unsigned short	UINT16;
#endif

#ifndef UINT32
typedef unsigned long	UINT32;
#endif

#endif	// __UINT__

#define	HINTERNET void *
#define	UINT32_PTR	LONG *

#ifndef __BOOL__
#define	__BOOL__
#define	BOOL unsigned int
#endif

#define	HTTP_11_VERSION_STRING			"HTTP/1.1"
#define	HTTP_11_VERSION_STRING_LEN		8

//
//
//	Options used for HttpQueryOption() and HttpSetOption()
//
//
#define HTTPCLNT_OPTION_CONNECT_TIMEOUT                3

//
//
//
//
//
extern HINTERNET HttpClientOpen(
	IN char *appName,
	IN struct proxyInfoStructure *pProxyInfo,
	IN UINT32 dwFlags,
	IN UINT32 (*pServiceFunction)(
				IN HINTERNET hndl,
				IN void *pRes,
				IN UINT32 szRes,
				IN UINT32 informationBits),
	IN void *pReserved,
	IN void *pRTag,
	IN OUT UINT32 *pFailureReasonCode);

extern int HttpClientClose(
	IN HINTERNET hndl,
	IN char* appName,
	IN void *pRTag,
	IN UINT32 dwFlags);

extern int HttpQueryOption(
	IN HINTERNET hndl,
	IN UINT32 option,
	OUT void *pBuffer,
	IN OUT UINT32 *pBufferLen);

extern int HttpSetOption(
	IN HINTERNET hndl,
	IN UINT32 option,
	IN void *pBuffer,
	IN UINT32 bufferLen);

extern int HttpOpenURL(
	IN HINTERNET hndl,
	IN char *pURL,
	IN char *pHeaders,
	IN UINT32 headersLen,
	IN UINT32 flags,
	IN void *pContext,
	IN void *pCallBackRoutine);

extern int HttpOpenRequest(
	IN HINTERNET hndl,
	IN char *pVerb,
	IN char *pObjectName,
	IN char *pVersion,
	IN char *pReferer,
	IN char **ppAcceptTypes);

extern HINTERNET HttpConnect(
	IN HINTERNET hndl,
	IN char *pServerName,
	IN UINT32 serverPort,
	IN UINT32 flags,
	IN OUT UINT32 *pFailureReasonCode);

extern int HttpAddRequestHeaderTag(
	IN HINTERNET hndl,
	IN void *pHeaders,
	IN UINT32 szHeadersLength);

extern int HttpAddRequestHeaderContentLengthTag(
	IN HINTERNET hndl,
	IN UINT32 FileSizeHigh,
	IN UINT32 FileSizeLow);

extern int HttpAddRequestHeaderCookieTag(
	IN HINTERNET hndl,
	IN void *pCookie,
	IN UINT32 szCookie);

extern int HttpAddRequestHeaderAuthorizationBasic(
	IN HINTERNET hndl,
	IN char *pzUsername,
	IN char *pzPassword);

extern int HttpAddRequestHeaderTransferEncodingChunked(IN HINTERNET hndl);

extern int HttpSendRequest(
	IN HINTERNET hndl,
	IN void *pDataBuffer,
	IN UINT32 dataBufferLen,
	IN void *pContext,
	IN void (*pCallBackRoutine)(
		IN void	*pContext,
		IN HINTERNET hndl,
		IN int	returnCode,
		IN void	*reserved)
	);

extern int HttpWriteRequestData(
	IN HINTERNET hndl,
	IN void *pDataBuffer,
	IN UINT32 *pDataBufferLen,
	IN void *pContext,
	IN void (*pCallBackRoutine)(
		IN void	*pContext,
		IN HINTERNET hndl,
		IN int	returnCode,
		IN void	*reserved)
	);

extern int HttpWriteRequestDataNoCopy(
	IN HINTERNET hndl,
	IN void *pDataBuffer,
	IN UINT32 *pDataBufferLen,
	IN void *pContext,
	IN void (*pCallBackRoutine)(
		IN void	*pContext,
		IN HINTERNET hndl,
		IN int	returnCode,
		IN void	*reserved)
	);

extern int HttpGetReply(
	IN HINTERNET hndl,
	IN OUT int *pHttpStatusCode,
	IN void *pContext,
	IN void (*pCallBackRoutine)(
		IN void	*pContext,
		IN HINTERNET hndl,
		IN int	returnCode,
		IN int	httpStatusCode,
		IN void	*reserved)
	);

extern int HttpReadResponseData(
	IN HINTERNET hndl,
	IN OUT char **pReplyDataBuffer,
	IN OUT UINT32 *pPostDataLen,
	IN void *pContext,
	IN void (*pCallBackRoutine)(
		IN void	*pContext,
		IN HINTERNET hndl,
		IN int	returnCode,
		IN char	**pReplyDataBuffer,
		IN UINT32 *pReplyDataLen,
		IN void	*reserved)
	);

extern int HttpCloseConnection(
	IN HINTERNET hndl);

