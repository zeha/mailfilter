/*+
 +		MFConfig.cpp
 +		
 +		MailFilter Configuration
 +		
 +		Copyright 2001 Christian Hofstädtler
 +		
 +		
 +		- Aug/2001 ; ch   ; Initial Code
 +		
 +		
+*/

#define _MFD_MODULE			"MFCONFIG.CPP"
#include "MailFilter.h"

#define MAILFILTER_CONFIGURATIONFILE_STRING (MAILFILTER_CONFIGURATION_LENGTH+1)

//
//  Show Configuration
//


int MF_ConfigReadFile(char* FileName, char* szDestination, long iBufLen)
{
	FILE* cfgFile;
	size_t dRead;
	
	szDestination[0] = 0;
	
	cfgFile = fopen(FileName,"rb");
	if (cfgFile == NULL)
		return -2;
	
	dRead = fread(szDestination,sizeof(char),(unsigned int)(iBufLen-1),cfgFile);
	fclose(cfgFile);
	
	szDestination[dRead]=0;
	szDestination[iBufLen-1]=0;
	
	return (int)(dRead);
}

int MF_ConfigReadString(char* ConfigFile, int Entry, char Value[])
{
	FILE* cfgFile;
	size_t dRead;

	Value[0]=0;

	cfgFile = fopen(ConfigFile,"rb");

	if (cfgFile == NULL)
		return -2;

	fseek(cfgFile,Entry,SEEK_SET);
	
	dRead = fread(Value,sizeof(char),MAILFILTER_CONFIGURATION_LENGTH-2,cfgFile);
	
	fclose(cfgFile);
	Value[dRead+1]=0;

	Value[MAX_PATH-2]=0;

	return (int)(strlen(Value));
}

int MF_ConfigReadInt(char ConfigFile[MAX_PATH], int Entry)
{
	int retVal = 0;

	char readBuf[16];

	FILE* cfgFile;

	readBuf[0]='\0';

	cfgFile = fopen(ConfigFile,"rb");

	if (cfgFile == NULL)
		return -2;

	fseek(cfgFile,Entry,SEEK_SET);
	fread(readBuf,sizeof(char),15,cfgFile);
	fclose(cfgFile);

	readBuf[14]=0;

	retVal = atoi(readBuf);

	return retVal;
}


/*
 *
 *
 *  --- eof ---
 *
 *
 */

