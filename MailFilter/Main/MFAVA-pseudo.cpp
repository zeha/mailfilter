/*
 +
 +   MFAVA-pseudo.cpp
 +
 +   MFAVA Library Abstraction: pseudo only.
 +
 */

#include <stdio.h>
#include <errno.h>

int MailFilter_AV_Check()
{
	return ENOENT;
}

int MailFilter_AV_Init()
{
	return ENOTSUP;
}

int MailFilter_AV_ScanFile(const char* szFileName, char* szVirusName, size_t iVirusNameLength, int &iVirusType)
{
#pragma unused(szFileName,szVirusName,iVirusNameLength,iVirusType)
	return ENOTSUP;
}

int MailFilter_AV_DeInit()
{	
	return ENOTSUP;
}

// -eof-
