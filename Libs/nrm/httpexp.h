/*****************************************************************************
 *
 * (C) 2003 Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 *
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked or uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 ****************************************************************************/

#ifndef __HTTPEXP_H__
#define __HTTPEXP_H__

#ifdef __cplusplus
extern "C" {
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
#define	UINT32_PTR	long *

#ifndef __BOOL__
#define	__BOOL__
#define	BOOL unsigned int
#endif

#define	NetWareHttpStackResourceSignature		'HBNW'
#define	NetWareHttpClientResourceSignature		'HCLT'

//
//
//	HTTP Request Method Type Definitions
//
//
#define	HTTP_REQUEST_METHOD_GET				0
#define	HTTP_REQUEST_METHOD_HEAD			1
#define	HTTP_REQUEST_METHOD_POST			2
#define	HTTP_REQUEST_METHOD_PUT				3
#define	HTTP_REQUEST_METHOD_DELETE			4
#define	HTTP_REQUEST_METHOD_OPTIONS		5
#define	HTTP_REQUEST_METHOD_COPY			6			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_MOVE			7			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_MKCOL			8			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_PROPFIND		9			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_PROPPATCH		10			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_LOCK			11			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_UNLOCK			12			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_MKREF			13			// WEBDAV - not supported
#define	HTTP_REQUEST_METHOD_ORDERPATCH	14			// WEBDAV - not supported


//
//
//
#define	NETWARE_TRY_ALTERNATE_FILE_SYSTEM	0x80085555
//
//
// HTTP Response Status Codes:
//

#define HTTP_STATUS_CONTINUE            100 // OK to continue with request
#define HTTP_STATUS_SWITCH_PROTOCOLS    101 // server has switched protocols in upgrade header

#define HTTP_STATUS_OK                  200 // request completed
#define HTTP_STATUS_CREATED             201 // object created, reason = new URI
#define HTTP_STATUS_ACCEPTED            202 // async completion (TBS)
#define HTTP_STATUS_PARTIAL             203 // partial completion
#define HTTP_STATUS_NO_CONTENT          204 // no info to return
#define HTTP_STATUS_RESET_CONTENT       205 // request completed, but clear form
#define HTTP_STATUS_PARTIAL_CONTENT		 206 // partial GET fufilled
#define HTTP_STATUS_MULTI_STATUS			 207 // multi status

#define HTTP_STATUS_AMBIGUOUS           300 // server couldn't decide what to return
#define HTTP_STATUS_MOVED               301 // object permanently moved
#define HTTP_STATUS_REDIRECT            302 // object temporarily moved
#define HTTP_STATUS_REDIRECT_METHOD     303 // redirection w/ new access method
#define HTTP_STATUS_NOT_MODIFIED        304 // if-modified-since was not modified
#define HTTP_STATUS_USE_PROXY           305 // redirection to proxy, location header specifies proxy to use
#define HTTP_STATUS_REDIRECT_KEEP_VERB  307 // HTTP/1.1: keep same verb

#define HTTP_STATUS_BAD_REQUEST         400 // invalid syntax
#define HTTP_STATUS_DENIED              401 // access denied
#define HTTP_STATUS_PAYMENT_REQ         402 // payment required
#define HTTP_STATUS_FORBIDDEN           403 // request forbidden
#define HTTP_STATUS_NOT_FOUND           404 // object not found
#define HTTP_STATUS_BAD_METHOD          405 // method is not allowed
#define HTTP_STATUS_NONE_ACCEPTABLE     406 // no response acceptable to client found
#define HTTP_STATUS_PROXY_AUTH_REQ      407 // proxy authentication required
#define HTTP_STATUS_REQUEST_TIMEOUT     408 // server timed out waiting for request
#define HTTP_STATUS_CONFLICT            409 // user should resubmit with more info
#define HTTP_STATUS_GONE                410 // the resource is no longer available
#define HTTP_STATUS_LENGTH_REQUIRED     411 // the server refused to accept request w/o a length
#define HTTP_STATUS_PRECOND_FAILED      412 // precondition given in request failed
#define HTTP_STATUS_REQUEST_TOO_LARGE   413 // request entity was too large
#define HTTP_STATUS_URI_TOO_LONG        414 // request URI too long
#define HTTP_STATUS_UNSUPPORTED_MEDIA   415 // unsupported media type

#define HTTP_STATUS_SERVER_ERROR        500 // internal server error
#define HTTP_STATUS_NOT_SUPPORTED       501 // required not supported
#define HTTP_STATUS_BAD_GATEWAY         502 // error response received from gateway
#define HTTP_STATUS_SERVICE_UNAVAIL     503 // temporarily overloaded
#define HTTP_STATUS_GATEWAY_TIMEOUT     504 // timed out waiting for gateway
#define HTTP_STATUS_VERSION_NOT_SUP     505 // HTTP version not supported

#define HTTP_STATUS_FIRST               HTTP_STATUS_CONTINUE
#define HTTP_STATUS_LAST                HTTP_STATUS_VERSION_NOT_SUP

/* HTTP Status Codes */
#define	HTTP_SUCCESSFUL				HTTP_STATUS_OK
#define	HTTP_NOT_MODIFIED				HTTP_STATUS_NOT_MODIFIED
#define	HTTP_BAD_REQUEST				HTTP_STATUS_BAD_REQUEST
#define	HTTP_UNAUTHORIZED				HTTP_STATUS_DENIED
#define	HTTP_FORBIDDEN_ACCESS		HTTP_STATUS_FORBIDDEN
#define	HTTP_NOT_FOUND					HTTP_STATUS_NOT_FOUND
#define	HTTP_INTERNAL_SERVER_ERROR	HTTP_STATUS_SERVER_ERROR
#define	HTTP_NOT_IMPLEMENTED			HTTP_STATUS_NOT_SUPPORTED

//
// HttpQueryInfo info levels. Generally, there is one info level
// for each potential RFC822/HTTP/MIME header that an HTTP server
// may send as part of a request response.
//
// The HTTP_QUERY_RAW_HEADERS info level is provided for clients
// that choose to perform their own header parsing.
//

#define HTTP_QUERY_MIME_VERSION                 0
#define HTTP_QUERY_CONTENT_TYPE                 1
#define HTTP_QUERY_CONTENT_TRANSFER_ENCODING    2				//NOT SUPPORTED
#define HTTP_QUERY_CONTENT_ID                   3				//NOT SUPPORTED
#define HTTP_QUERY_CONTENT_DESCRIPTION          4				//NOT SUPPORTED
#define HTTP_QUERY_CONTENT_LENGTH               5
#define HTTP_QUERY_CONTENT_LANGUAGE             6
#define HTTP_QUERY_ALLOW                        7
#define HTTP_QUERY_PUBLIC                       8
#define HTTP_QUERY_DATE                         9
#define HTTP_QUERY_EXPIRES                      10
#define HTTP_QUERY_LAST_MODIFIED                11
#define HTTP_QUERY_MESSAGE_ID                   12				// NOT SUPPORTED
#define HTTP_QUERY_URI                          13
#define HTTP_QUERY_DERIVED_FROM                 14
#define HTTP_QUERY_COST                         15				// NOT SUPPORTED
#define HTTP_QUERY_LINK                         16
#define HTTP_QUERY_PRAGMA                       17
#define HTTP_QUERY_VERSION                      18
#define HTTP_QUERY_STATUS_CODE                  19              // NOT SUPPORTED
#define HTTP_QUERY_STATUS_TEXT                  20              // NOT SUPPORTED
#define HTTP_QUERY_RAW_HEADERS                  21				// NOT SUPPORTED
#define HTTP_QUERY_RAW_HEADERS_CRLF             22
#define HTTP_QUERY_CONNECTION                   23
#define HTTP_QUERY_ACCEPT                       24
#define HTTP_QUERY_ACCEPT_CHARSET               25
#define HTTP_QUERY_ACCEPT_ENCODING              26
#define HTTP_QUERY_ACCEPT_LANGUAGE              27
#define HTTP_QUERY_AUTHORIZATION                28
#define HTTP_QUERY_CONTENT_ENCODING             29
#define HTTP_QUERY_FORWARDED                    30				// NOT SUPPORTED
#define HTTP_QUERY_FROM                         31
#define HTTP_QUERY_IF_MODIFIED_SINCE            32
#define HTTP_QUERY_LOCATION                     33
#define HTTP_QUERY_ORIG_URI                     34				// NOT SUPPORTED
#define HTTP_QUERY_REFERER                      35
#define HTTP_QUERY_RETRY_AFTER                  36
#define HTTP_QUERY_SERVER                       37
#define HTTP_QUERY_TITLE                        38				// NOT SUPPORTED
#define HTTP_QUERY_USER_AGENT                   39
#define HTTP_QUERY_WWW_AUTHENTICATE             40
#define HTTP_QUERY_PROXY_AUTHENTICATE           41
#define HTTP_QUERY_ACCEPT_RANGES                42
#define HTTP_QUERY_SET_COOKIE                   43
#define HTTP_QUERY_COOKIE                       44
#define HTTP_QUERY_REQUEST_METHOD               45		  // special: GET/POST etc.
#define HTTP_QUERY_REFRESH                      46				// NOT SUPPORTED
#define HTTP_QUERY_CONTENT_DISPOSITION          47

//
// HTTP 1.1 defined headers
//

#define HTTP_QUERY_AGE                          48
#define HTTP_QUERY_CACHE_CONTROL                49
#define HTTP_QUERY_CONTENT_BASE                 50				// NOT SUPPORTED
#define HTTP_QUERY_CONTENT_LOCATION             51
#define HTTP_QUERY_CONTENT_MD5                  52
#define HTTP_QUERY_CONTENT_RANGE                53
#define HTTP_QUERY_ETAG                         54
#define HTTP_QUERY_HOST                         55
#define HTTP_QUERY_IF_MATCH                     56
#define HTTP_QUERY_IF_NONE_MATCH                57
#define HTTP_QUERY_IF_RANGE                     58
#define HTTP_QUERY_IF_UNMODIFIED_SINCE          59
#define HTTP_QUERY_MAX_FORWARDS                 60
#define HTTP_QUERY_PROXY_AUTHORIZATION          61
#define HTTP_QUERY_RANGE                        62
#define HTTP_QUERY_TRANSFER_ENCODING            63
#define HTTP_QUERY_UPGRADE                      64
#define HTTP_QUERY_VARY                         65
#define HTTP_QUERY_VIA                          66
#define HTTP_QUERY_WARNING                      67


//
//	Content Type Strings
//
#define	HTTP_CONTENT_TYPE_OCTET_STREAM			0
#define	HTTP_CONTENT_TYPE_JPEG						1
#define	HTTP_CONTENT_TYPE_GIF						2
#define	HTTP_CONTENT_TYPE_PNG						3
#define	HTTP_CONTENT_TYPE_ZIP						4
#define	HTTP_CONTENT_TYPE_HTML						5
#define	HTTP_CONTENT_TYPE_WAV						6
#define	HTTP_CONTENT_TYPE_AU							7
#define	HTTP_CONTENT_TYPE_MIDI						8
#define	HTTP_CONTENT_TYPE_MP3						9
#define	HTTP_CONTENT_TYPE_WML						10
#define	HTTP_CONTENT_TYPE_HTML_UTF8				11
#define	HTTP_CONTENT_TYPE_END_OF_LIST				12
//
//
//	Request Header Helper Routines (extraction function)
//
//
extern int HttpQueryInfo(
			    IN HINTERNET hRequest,
			    IN UINT32 dwInfoLevel,	// HttpQueryInfoLevel
			    IN void *pBuffer,
			    IN OUT UINT32_PTR lpBufferLength);

extern char *HttpReturnHttpString(IN HINTERNET hndl);
extern char *HttpReturnString(UINT32 stringType);
extern int HttpReturnPathBuffers(IN HINTERNET hrequest, UINT32_PTR lpszPathBufs, char **path, char **cnvpath);
extern void *HttpReturnDataTxBuffer(IN HINTERNET hrequest, UINT32_PTR lpszBuffer);
extern int HttpSendDataTxBuffer(HINTERNET hndl, UINT32 bytesToSend);

extern int HttpSendDataSprintf(
				IN HINTERNET hrequest,
				IN const char *controlString,
				...);

extern BOOL HttpReturnHeaderVersion(
				IN HINTERNET hndl,
				IN OUT UINT32_PTR lpHttpMajorVersion,
				IN OUT UINT32_PTR lpHttpMinorVersion);

extern BOOL HttpReturnRequestMethod(IN HINTERNET hndl, IN OUT UINT32_PTR httpRequestMethodType);

extern int HttpReturnRawRequest(IN HINTERNET hndl, UINT32 szRawRequestBuffer, char *pzRawRequestBuffer);


//
//
//	Response Header Helper Routines
//
//
extern int HttpResetReply(IN HINTERNET hndl);

extern int HttpOpenResponseHeaderTag(IN HINTERNET hrequest, UINT32 HTTP_ERROR_CODE);
extern int HttpAddResponseHeaderTags(
				IN HINTERNET hRequest,
				IN void *pHeaders,
			    IN UINT32 dwHeadersLength);

extern int HttpAddResponseHeaderDateTag(IN HINTERNET hrequest);

extern int HttpAddResponseHeaderContentTypeTag(IN HINTERNET hrequest, IN void *pzExtName, IN int szExtName, OUT UINT32_PTR pContentLengthFlag);

extern int HttpAddResponseHeaderLastModifiedTag(IN HINTERNET hrequest, IN UINT32 gmtTimeInSeconds);

extern int HttpSendResponseHeader(IN HINTERNET hRequest);

extern int HttpAddResponseHeaderStringContentType(IN HINTERNET hrequest, IN void *pzContentType);
extern int HttpAddResponseHeaderContentLengthTag(IN HINTERNET hrequest, IN UINT32 FileSizeHigh, IN UINT32 FileSizeLow);
extern int HttpAddResponseHeaderLocationTag(IN HINTERNET hndl, char *pzURL);

extern int HttpAddResponseHeaderSetCookieTag(IN HINTERNET hndl, IN void *pCookie, IN UINT32 szCookie);

extern int HttpQueryPort(char *pzHostInfoString, UINT32 port, UINT32 *pHost);

extern int HttpSendSuccessfulResponse(IN HINTERNET hrequest, void *pzContentType);
extern int HttpSendErrorResponse(IN HINTERNET hRequest, UINT32 HTTP_ERROR_CODE);

extern int HttpSendPackage(
				IN HINTERNET hndl,
				void *pData,
				UINT32 szData);

extern int HttpSendErrorPackageResponse(
				IN HINTERNET hndl,
				UINT32 HTTP_ERROR_CODE,
				void *pData,
				UINT32 szData);

extern int HttpSendWMLDocumentPrologue(
				IN HINTERNET hndl,
				int version);

//
//
//	Send Data Helper Routines
//
//

extern int HttpSendDataResponse(
				IN HINTERNET hRequest,
				IN void *pBuffer,
				IN UINT32 szBuffer);

extern int HttpSendDataResponseNoCopy(
				IN HINTERNET hRequest,
				IN void *pBuffer,
				IN UINT32 szBuffer);

extern int HttpSendDataFlush(IN HINTERNET hRequest);

extern int HttpEndDataResponse(IN HINTERNET hRequest);


//
//
//	Service Registration Routines
//
//



extern BOOL RegisterServiceMethod(
			IN const char *pzServiceName,
			IN const char *pServiceTag,
			IN int serviceTagLength,
			IN UINT32 (*pServiceMethodCheck)(
						IN HINTERNET,
						IN void *pExtraInfo,
						IN UINT32 szExtraInfo,
						IN UINT32 InformationBits),
			IN void *pRTag,
			OUT UINT32 *pReturnCode);

extern BOOL RegisterServiceMethodEx(
			IN const char *pzServiceName,
			IN const char *pServiceTag,
			IN int serviceTagLength,
			IN void *tableOfContentsStruc,	// NULL or ptr to struct TOCregistration
			IN UINT32 requestedRights,
			IN void *pReserved,
			IN UINT32 (*pServiceMethod)(
						IN HINTERNET,
						IN void *pExtraInfo,
						IN UINT32 szExtraInfo,
						IN UINT32 InformationBits),
			IN void *pRTag,
			OUT UINT32 *pReturnCode);

struct TOCregistration
{
	UINT32 TOCHeadingNumber;	// zero or suggested level number to be in TOC
	char reserved[4];		// must be set to zero
	char TOCHeadingName[64];	//Larger to handle expanded names.
	// more data can follow
};


extern BOOL DeRegisterServiceMethod(
			IN const char *pzServiceName,
			IN const char *pServiceTag,
			IN int serviceTagLength,
			IN UINT32 (*pServiceMethodCheck)(
						IN HINTERNET,
						IN void *pExtraInfo,
						IN UINT32 szExtraInfo,
						IN UINT32 InformationBits),
			IN void *pRTag,
			OUT UINT32 *pReturnCode);


extern BOOL RegisterFileSystemServiceMethodEx(
			IN UINT32 (*pServiceMethodCheck)(
						IN HINTERNET,
						IN void *pExtraInfo,
						IN UINT32 szExtraInfo,
						IN UINT32 InformationBits),
			IN void *pRTag,
			OUT UINT32 *pReturnCode,
			IN UINT32 FileSystemID,
			IN UINT32 optionFlags);

extern BOOL DeRegisterFileSystemServiceMethod(IN void *pRTag);

extern UINT32 TryAlternateFileSystemMethod(
						IN HINTERNET hndl, 
						IN void *pExtraInfo,
						IN UINT32 szExtraInfo,
						IN UINT32 InformationBits);

extern int SetFileSystemServiceRole(int primaryFileSystemID);


//
//	Information/Control Bits Follow
//
#define	INFO_SUPERVISOR_PRIVILEGES_BIT						0x00000001
#define	INFO_CONSOLE_OPERATOR_PRIVILEGES_BIT				0x00000002
#define	INFO_LOGGED_IN_BIT									0x00000004
#define	INFO_SSL_CONNECTION_BIT								0x00000008
#define	INFO_NO_LICENSE_BIT									0x00000010
#define	INFO_DO_DCLIENT_LOGOUT_BIT							0x00000020
#define	INFO_DO_LOGIN_WITHOUT_REDIRECT_BIT					0x00000040
#define	INFO_METHOD_SUPPORTS_USER_TOOLS_BIT					0x00000080

#ifdef	WML
// CONTROL_WML_REQUEST_BIT Set if WML accepted and HTML NOT accepted by client.
// The of 'Accept: */*' option is ignored
#define	INFO_WML_REQUEST_BIT								0x00000100
#endif
#define	INFO_READ_BODY_ON_DEMAND_BIT						0x00000200
#define	INFO_SDEBUG_LIMIT_PRIVILEGES_BIT					0x00000400

#define	DISPLAY_FULL_PAGE_BIT								0x00010000
#define	DISPLAY_USER_TOOLS_BIT								0x00020000
#define METHOD_DESCRIPTION_UTF8								0x00040000
#define	CONTROL_RETURN_METHOD_RIGHTS_BIT					0x08000000
#define	CONTROL_NO_AUTHENTICATION_OPTION_ENABLED_BIT		0x10000000
#define	CONTROL_NO_LOGIN_OPTION_ENABLED_BIT					0x20000000
#define	CONTROL_DEINITIALIZATION_BIT						0x40000000
#define	CONTROL_INITIALIZATION_BIT							0x80000000

#define	INFO_RIGHTS_BITS		(INFO_SUPERVISOR_PRIVILEGES_BIT|INFO_CONSOLE_OPERATOR_PRIVILEGES_BIT|INFO_LOGGED_IN_BIT|INFO_SDEBUG_LIMIT_PRIVILEGES_BIT)

///////////////////////////////////////////////////////////////////////////////////////
/////			FILE MODE FLAGS & APIs
///////////////////////////////////////////////////////////////////////////////////////
#define	FILE_MODE_DATE_TIME_STAMP		0x00000001
#define	FILE_MODE_APPEND_TO_EOF			0x00000002
#define	VALID_FILE_MODE_FLAG_BITS		(FILE_MODE_DATE_TIME_STAMP | FILE_MODE_APPEND_TO_EOF)

extern int HttpActivateFileMode(HINTERNET hndl, UINT32 netWareFileHandle, UINT32 fileModeFlags);
extern int HttpDeActivateFileMode(HINTERNET hndl);
///////////////////////////////////////////////////////////////////////////////////////



//
//	Service Helper Routines
//

extern int HttpReturnPostDataBuffer(IN HINTERNET hndl, char **pPostDataBuffer, UINT32_PTR lpPostDataLen);

extern int serviceGetConnectionNumber(
			IN HINTERNET hrequest,
			IN OUT UINT32_PTR lpNetWareConnectionNumber,
			IN OUT UINT32_PTR lpNetWareTaskNumber);

extern int serviceGetLocalNDSContext(
					IN HINTERNET hrequest,
					IN OUT int *lpContext);

extern int serviceGetRemoteNDSContext(
					IN HINTERNET hrequest,
					IN int addressType,
					IN int addressSize,
					IN char *address,
					IN OUT int *lpContext);

extern int serviceGetUserLoginInfo(
			IN HINTERNET hrequest,
			IN OUT char *pUsername,
			IN OUT int *pUsernameLen,
			IN OUT char *pPassword,
			IN OUT int *pPasswordLen,
			IN OUT int *pCodePage);

//
//
//
extern int HttpGetContext(
				IN HINTERNET hndl,
				UINT32_PTR *pContext);

extern int HttpSetContext(
				IN HINTERNET hndl,
				UINT32_PTR pContext);

extern int HttpReturnPutDataBuffer(
				IN HINTERNET hndl,
				UINT32_PTR lpTotalFileSize,
				UINT32_PTR lpCurrentFileOffset,
				char **pPutDataBuffer,
				UINT32_PTR lpPutDataLen);

extern UINT32 HttpInternetTimeToLocalTime(UINT32 UTC, char *pzOptionalStringOutputBuffer);

extern UINT32 HttpExtractInternetTimeFromString(char *string);
extern UINT32 HttpLocalTimeToInternetTime(UINT32 DOSDateAndTime);
extern int HttpInternetTimeToString(UINT32 utcTime, void *ins);
extern int HttpLocalTimeToString(UINT32 DOSDateAndTime, char *ins);
extern int HttpCheckIfModifiedSinceTime(HINTERNET hndl, int callerInternetTime);
extern BOOL HttpStackIpInformation(
			UINT32 infoLevel,
			void *infoBuffer,
			UINT32_PTR numberOfIpAddresses,
			void **addrInfo,
			UINT32_PTR listenPortNumber);

extern void HTMLConvertName(char *src, char *dst, UINT32 srclen);
extern void HTMLUnConvertName(char *src, char *dst, UINT32 srclen);

extern void HttpUnConvertName(char *src, char *dst, UINT32 srclen);
extern void HttpConvertName(char *src, char *dst, UINT32 srclen);
extern void HttpConvertCP1252ToISO88591(char *src, char *dst, UINT32 srclen);
extern void HttpConvertISO88591ToCP1252(char *src, char *dst, UINT32 srclen);
extern void HttpConvertNDSName(char *src, char *dst, UINT32 srclen);
extern BOOL HttpScanServiceMethods(int infoBits, UINT32 index, char *serviceName, char *serviceNameTag, UINT32_PTR nextIndex);
extern UINT32 HttpServiceMethodTOC(int infoBits, UINT32 TOCLevel, UINT32 szBuffer, void *pBuffer, UINT32_PTR szUsed);
UINT32 HttpReturnLoginServiceTagString(HINTERNET hndl, char *buffer, UINT32 size);

extern int HttpExtractSrcAddress(HINTERNET hndl, int *pSrcIPAddr, int *pSrcIPPort, char *pzSrcIPAddrBuf, char *pzSrcPortBuf);
extern int HttpExtractDestAddress(HINTERNET hndl, int *pDestIPAddr, int *pDestIPPort, char *pzDestIPAddrBuf, char *pzDestPortBuf);
extern int HttpConvertStringToIPv4Address(char *pString, UINT32_PTR pAddressValue, UINT32_PTR pPortValue);

// CULL.C
extern int HttpFindNameAndValue(char *pBuffer, char *pzTokenName, char *pzTokenValue, int *pzTokenValueMaxLen);
extern int HttpParseBuffer(
	char	*pBuffer,
	int	index,
	char	*pzTokenName,
	int	*pzTokenNameMaxLen,
	char	*pzTokenValue,
	int	*pzTokenValueMaxLen,
	char	**pzBufferProcessTo);

// CONFIG.C
int HttpOpenPort(
		long	IPAddress,	// not used (yet), must be ZERO
		unsigned short	portNumber,
		long	flags);

int HttpClosePort(
		long	IPAddress,	// not used (yet), must be ZERO
		unsigned short	portNumber);

// return from the HttpServiceMethodTOC api
struct TOCsearch
{
	UINT32 numberOfTOCHeadings;
	char TOCSubHeadingCount;
	char TOCHeadingFlag;
	char TOCHeadingName[1];
	// more data follows
};


//
//	Debugger Screen Control Prototypes
//
extern BOOL HttpDebugScreenStatus(void);
extern int HttpEnableScreen(void), HttpDisableScreen(void);

//
//
//

//
// File Types
//
#define	FILE_TYPE_UNK		0x00000000
#define	FILE_TYPE_JPEG		0x00000001
#define	FILE_TYPE_GIF		0x00000002
#define	FILE_TYPE_PNG		0x00000003
#define	FILE_TYPE_ZIP		0x00000004
#define	FILE_TYPE_HTML		0x00000005
#define	FILE_TYPE_WAV		0x00000006
#define	FILE_TYPE_AU		0x00000007
#define	FILE_TYPE_MIDI		0x00000008
#define	FILE_TYPE_XML		0x00000009
#define	FILE_TYPE_XLS		0x0000000a

#endif /* __HTTPEXP_H__ */

// If you are registering a method that is in one of these categories you need not use the name
// in the TOC structure because we translate these name. If you are registering in one of these
// just make sure that you category number is correct and pass a pointer to a null and the name.
// This will insure that you method shows up under the correct category.

#define	STR_HEALTH_CONTROL		"Health Monitors"			//Old Name, New Name is Diagnose Server
#define	HEALTH_CONTROL_ID		1							//Use the for your TOCHeadingNumber 

#define	STR_SERVER_CONTROL		"Server Management"			//OLD Name, New Name is Manage Server
#define SERVER_CONTROL_ID		2 							//Use the for your TOCHeadingNumber 

#define	STR_SOFTWARE_CONTROL	"Application Management"	//Old Name, New Name is Manage Applications
#define	SOFTWARE_CONTROL_ID		3 							//Use the for your TOCHeadingNumber 

#define	STR_HARDWARE_CONTROL	"Hardware Management"		//Old name, New Name is Manage Hardware
#define	HARDWARE_CONTROL_ID		4							//Use the for your TOCHeadingNumber 

#define	STR_DS_CONTROL			"NDS Management"			//Old Name, New Name is Manage eDirectory
#define	DS_CONTROL_ID			5							//Use the for your TOCHeadingNumber 

#define	STR_GROUP_CONTROL		"Server Groups"				//Old Name, New Name is Use Server Groups
#define GROUP_CONTROL_ID		6							//Use the for your TOCHeadingNumber 

#define	STR_REMOTE_CONTROL		"Remote Server Access"		//Old Name, New Name is Access Other Servers
#define	REMOTE_CONTROL_ID		7 							//Use the for your TOCHeadingNumber 

#define	STR_VOLUME_CONTROL		"Volume Management" 		//Obsolete

/* ****************************************************************************
*																										*
*						Common Logger Definitions & Prototypes								*
*																										*
**************************************************************************** */
//
//		Common Logger Levels
//
#define	LVL_EMERGENCY		0
#define	LVL_ALERT			1
#define	LVL_ERROR			2
#define	LVL_WARNING			3
#define	LVL_AUTH				4
#define	LVL_SECURITY		5
#define	LVL_USAGE			6
#define	LVL_SYSTEM			7
#define	LVL_IMPORTANT		8
#define	LVL_DEBUG			9

#define	LVL_MAX				(LVL_DEBUG + 1)

//
//		Common Logger Options
//
#define	OPTION_LANG			 0						//Requires pointer to ASCIIZ String
#define	OPTION_DUR			 1						//Requires integer value
#define	OPTION_PS			 2						//Requires integer value
#define	OPTION_ID			 3						//Requires pointer to ASCIIZ String
#define	OPTION_SRC_IP		 4						//Requires pointer to ASCIIZ String
#define	OPTION_SRC_FQDN	 5						//Requires pointer to ASCIIZ String
#define	OPTION_SRC_NAME	 6						//Requires pointer to ASCIIZ String
#define	OPTION_SRC_PORT	 7						//Requires integer value
#define	OPTION_SRC_USR		 8						//Requires pointer to ASCIIZ String
#define	OPTION_SRC_MAIL	 9						//Requires pointer to ASCIIZ String
#define	OPTION_DST_IP		 10					//Requires pointer to ASCIIZ String
#define	OPTION_DST_FQDN	 11					//Requires pointer to ASCIIZ String
#define	OPTION_DST_NAME	 12					//Requires pointer to ASCIIZ String
#define	OPTION_DST_PORT	 13					//Requires integer value
#define	OPTION_DST_USR		 14					//Requires pointer to ASCIIZ String
#define	OPTION_DST_MAIL	 15					//Requires pointer to ASCIIZ String
#define	OPTION_REL_IP		 16					//Requires pointer to ASCIIZ String
#define	OPTION_REL_FQDN	 17					//Requires pointer to ASCIIZ String
#define	OPTION_REL_NAME	 18					//Requires pointer to ASCIIZ String
#define	OPTION_REL_PORT	 19					//Requires integer value
#define	OPTION_REL_USR		 20					//Requires pointer to ASCIIZ String
#define	OPTION_REL_MAIL	 21					//Requires pointer to ASCIIZ String
#define	OPTION_VOL			 22					//Requires integer value
#define	OPTION_VOL_SENT	 23					//Requires integer value
#define	OPTION_VOL_RCVD	 24					//Requires integer value
#define	OPTION_CNT			 25					//Requires integer value
#define	OPTION_CNT_SENT	 26					//Requires integer value
#define	OPTION_CNT_RCVD	 27					//Requires integer value
#define	OPTION_PROG_FILE	 28					//Requires pointer to ASCIIZ String
#define	OPTION_PROG_LINE	 29					//Requires integer value
#define	OPTION_STAT			 30					//Requires pointer to ASCIIZ String
#define	OPTION_TTY			 31					//Requires pointer to ASCIIZ String
#define	OPTION_DOC			 32					//Requires pointer to ASCIIZ String
#define	OPTION_PROT			 33					//Requires pointer to ASCIIZ String
#define	OPTION_CMD			 34					//Requires pointer to ASCIIZ String
#define	OPTION_MSG			 35					//Requires pointer to ASCIIZ String

#define	OPTION_MAX			(OPTION_MSG + 1)

//
// 	Common Logger Error Codes
//
#define	LOGGER_ERR_BAD_HANDLE				1
#define	LOGGER_ERR_BAD_LEVEL					2
#define	LOGGER_ERR_BAD_OPTION_NUMBER		3
#define	LOGGER_ERR_OUT_OF_RANGE				4

//
// 	Common Logger Prototypes  (Exported)
//
extern int HttpLogOpen(char *pzFileName, int holdFlag, int *pHandle);
extern int HttpLogEntry(int pHandle, int level, HINTERNET hndl, char *pzProg, int optionCount, int *pOptions, ...);
extern int HttpLogClose(int pHandle);

extern int HttpDisableLogger(void);
extern int HttpEnableLogger(void);
extern BOOL HttpLoggerStatus(void);

extern int HttpResetSystemLogFile(void);
extern int HttpViewSysLogFile(HINTERNET hndl);

//
//	Mail APIS
//
extern int SMTPSendMailgram(
					BOOL waitOnCompletionFlag,
					int rcptListCount,//	MAX RCPT LIST COUNT = 8
					char **pzRcptList,
					char *pzFrom,		// OPTIONAL: NULL		MAX LEN = 127
					char *pzSender,	// OPTIONAL: NULL		MAX LEN = 127
					char *pzSubject,	// OPTIONAL: NULL		MAX LEN = 127
					char *pzBody);		// OPTIONAL: NULL		MAX LEN = 1023

extern BOOL	SMTPSetMailServerName(char *pMailServerName, int primaryOrAlternateFlag);

extern void SetupMailToList(void);

//  Includes null
#define	MaximumSizeOfRcpt				128
#define	MaximumSizeOfFrom				128
#define	MaximumSizeOfSender			128
#define	MaximumSizeOfSubject			128
#define	MaximumSizeOfBody				1024

/* ****************************************************************************
*																										*
*						HttpSendPingRequest structures and defines
*																										*
**************************************************************************** */

#define IPADDR_STR_LEN					16
#define REQUEST_WORKSPACE_SIZE			200

typedef struct
{
	UINT32		resultCode;
	UINT32		errorCode;
	char		IPAddressString[IPADDR_STR_LEN];
	UINT32		replyDataSize;
	UINT32		transportTime;
	UINT32		TTL;
	char		requestWorkSpace[REQUEST_WORKSPACE_SIZE];
} HttpPingResult;

#define	HTTP_PING_SYNCHRONOUS			0x00000000
#define	HTTP_PING_ASYNCHRONOUS			0x00000001

#define	HTTP_PING_RESULT_SUCCESS		0x00000000				// received ICMP echo reply 
#define	HTTP_PING_RESULT_WAITING		0x00000001				// still processing
#define	HTTP_PING_RESULT_NO_SOCKET		0x00000020				// error opening winsock socket
#define	HTTP_PING_RESULT_HOST_UNKNOWN	0x00000040				// error resolving DNS name
#define	HTTP_PING_RESULT_SOCKET_ERR		0x00000080				// winsock error, error code field is set to winsock err value
#define	HTTP_PING_RESULT_TIMEOUT		0x00000100				// ping request timed out

extern HttpPingResult *HttpSendPingRequest(
	char *pzHostInfoString, 
	UINT32 flags, 
	UINT32 timeOutValue, 
	UINT32 pingAttempts,
	UINT32 *lpHost);

extern void HttpReleasePingResult(
	HttpPingResult *pingResult);

#ifdef __cplusplus
}
#endif
