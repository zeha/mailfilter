/*****************************************************************************
 *
 * (C) 1999 Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 *
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked or uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 ****************************************************************************/

//
//	This file contains api's that are exported for NLM's that need access to portal routines.
//


// Common header and footer functions.
extern UINT BuildAndSendHeader(HINTERNET	handle,
							   char 		*windowTitle,
							   char 		*pageIdentifier,
							   char 		Refresh,
							   UINT32		refreshDelay,
							   UINT			flags,
							   void			(*AddHeaderText)(HINTERNET	handle),
							   char			*bodyTagText,
							   char			*helpURL);

extern UINT BuildAndSendHelpHeader(HINTERNET	handle,
				   char 		*windowTitle,
				   UINT			flags,
				   void			(*AddHeaderText)(HINTERNET	handle),
				   char			*bodyTagText);

extern UINT BuildAndSendUpdateHeader(HINTERNET	handle,
									 void		*info,
									 UINT32		infoLen,
									 char 		*windowTitle,
									 char 		*pageIdentifier,
									 char 		Refresh,
									 UINT32		refreshDelay,
									 UINT		flags,
									 void		(*AddHeaderText)(HINTERNET	handle),
									 char		*bodyTagText,
									 char		*pageToRefresh,
			  						 char 		*helpURL);

extern UINT32 BuildAndSendFooter(HINTERNET hndl);



/* Defines for server health flags */
#define NEW_ENTRY 			0x01
#define REPLACE_ENTRY 		0x02
#define FULL_PAGE_DISPLAY	0x04

/*  Server health registration information...*/
/* Categories */
#define KERNEL_SERVICES   		0
//	 	Cpu Utilization
//	 	Allocated Server Processes
//	 	Available Server Processes
//	 	DS Thread Usage
//	 	Work To Do Response Time
#define DEBUG_SERVICES   		1
//	 	Abended Threads ect......
#define CONNECTION_SERVICES  	2
//	 	Connection Availability
#define FILESYSTEM_SERVICES  	3
//	 	Directory Cache Buffers
//	 	Cache Performance
//	 	Available Disk Space
//	 	Available Directory Entries
#define MEMORY_SERVICES   		4
//	 	Available Memory
//	 	Virtual Memory Performance
//	 	Swap File Information
#define DIRECTORY_SERVICES   	5
//	 	DS Status
#define LAN_SERVICES   			6
//	 	Packet Receive Buffers
//	 	Available ECB's
//	 	Lan Traffic
#define STORAGE_SERVICES   		7
//	 	Disk Traffic
#define PRINTING_SERVICES   	8
//	 	NDPS Printing Services
#define APPLICATION_SERVICES 	9
//	 	GroupWise Status
#define HARDWARE_SERVICES		10
//		Used for hardware vendors to report
//		the state of the hardware.
#define MISC_SERVICES			11
//
//		This category will hold all
//		items that don't have a category

//		Should always be 1 more then the
//		last category number.
#define MAX_CATEGORIES		12

/* ErrorCodes */
#define SUCCESS   						0
#define INVALID_MODULE_HANDLE   		1
#define INVALID_HEALTH_MONITOR_HANDLE   2
#define INVALID_CATEGORY   				3
#define INVALID_DESCRIPTION_STRING   	4
#define INVALID_URL_STRING   			5
#define INVALID_HELP_URL_STRING			6
#define INSUFFICIENT_MEMORY				7
#define INVALID_REGISTER_FLAGS			8
#define HEALTH_MONITOR_REPLACED			9
#define MP_LOCK_ERROR					10
#define THRESHOLD_ALREADY_SET			11
#define INVALID_CALLBACK_ADDRESS		12
#define VALUE_NOT_AVAILABLE				13
#define UNABLE_TO_CREATE_KEY			14

// currentState values
#define REDSTATUS 2
#define YELLOWSTATUS 1
#define GREENSTATUS 0
#define INACTIVESTATUS 10
#define OFFLINE 20

#define MAX_DESCRIPTION_SIZE			128
#define MAX_VALUE_STRING_SIZE			64
#define MAX_URL_SIZE					128
#define MAX_HELP_URL_SIZE				128
#define MIN_TABLE_SIZE 					3

/* Prototypes for registering, unregistering and posting information */
/* to portal for server health and setting a threshold callback routine. */
extern UINT32 RegisterHealthMonitor(UINT32 moduleHandle,
									UINT32 *handle,
									UINT8 category,
									UINT8 flags,
									char *descriptionString,
									char *URL,
									char *helpURL, 
									char *OldDescriptionString);

extern UINT32 UnregisterHealthMonitor(UINT32 moduleHandle,
									  UINT32 handle,
							    	  UINT8 category);

extern UINT32 PostHealthState(UINT32 moduleHandle,
							  UINT32 handle,
							  UINT32 category,
							  char *maxValue,
							  char *peakValue,
							  char *currentValue,
							  UINT8 currentState,
							  UINT8 *resUINT8,
							  UINT32 resUINT32);


extern UINT32 InstallPortalMSHealth(void);

extern UINT32 RemovePortalMSHealth(void);

extern UINT32 RegisterServerHealthThreshold(UINT32 handle,
											char *thresholdURL,
									 		UINT32 category,
											UINT32 (*ReturnCurrentValues)
													(char *suspectValue, char *criticalValue,
													UINT32 *susDuration, UINT32 *critDuration));



extern int nrmCreateContext(HINTERNET hrequest, int currentNDSContext, int *newNDSContext, char *context);
extern int nrmFindSpecificObjectAttribute(int NDSContext, char *attributeTag, char *value);
extern int nrmFindObjectAttributes(int NDSContext, int attributeListCount, char **attributeNames, char **attributeData);

extern int register_fm_entry(
					void *moduleHandle,
					char *pURL,
					char *pTitle,
					char *pAltText,	// optional
					char *pGif);		// optional

extern int remove_fm_entry(
					void *moduleHandle,
					char *pURL);


extern UINT32 NRMCreateVirtualInterfaceFile(char	*filePath,
				char	*readFunctionName,
				void	*readFunctionAddress,
				char	*writeFunctionName,
				void	*writeFunctionAddress,
				char	*helpInformationString);


extern UINT32 NRMRemoveVirtualInterfaceFile(char *filePath);
