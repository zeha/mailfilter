#pragma once
#include "iX.h"

#ifndef _IXDIR_H_
#define _IXDIR_H_


class iXDir
{
public:
	iXDir(const char* DirectoryName);
	virtual ~iXDir();
	
	bool ReadNextEntry();
	const char* GetCurrentEntryName();
	long long GetCurrentEntrySize();
	time_t GetCurrentEntryModificationTime();
	
	bool SkipDotFiles;
	
	bool UnlinkCurrentEntry();
	
private:
	char* m_DirectoryName;

#if defined(IXPLAT_NETWARE_LIBC) || defined(IXPLAT_WIN32)
	DIR* m_Directory;
	struct dirent* m_Entry;
#endif
#ifdef IXPLAT_NETWARE_CLIB
	DIR* m_Directory;
	DIR* m_Entry;
#endif

};

#endif // _IXDIR_H_