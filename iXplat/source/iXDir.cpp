
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "iX.h"
#include "iXDir.h"
#include <sys/stat.h>

#ifdef IXPLAT_NETWARE_LIBC
#include <screen.h>
#endif


iXDir::iXDir(const char* DirectoryName)
{
#ifdef IXPLAT_NETWARE_CLIB
	char* szFull = (char*)malloc(IX_PATH_MAX);
	strcpy(szFull,DirectoryName);
	strcat(szFull,"\\*.*");
	m_Directory = opendir(szFull);
	free(szFull);
#else
	m_Directory = opendir(DirectoryName);
#endif

	m_Entry = NULL;
	SkipDotFiles = false;
	
	m_DirectoryName = strdup(DirectoryName);
}


iXDir::~iXDir()
{
	if (m_Directory != NULL)
		closedir(m_Directory);
	
	if (m_DirectoryName != NULL)
		free(m_DirectoryName);
}

bool iXDir::ReadNextEntry()
{
	if (m_Directory != NULL)
	{
		m_Entry = readdir(m_Directory);
		
		// valid result?
		if (m_Entry == NULL) 
			return false;
		else
		{
			const char* e = GetCurrentEntryName();
			if (e[0] == '.')
			{
				if (e[1] == '\0')
				{
					return ReadNextEntry();
				} else
				if ( (e[1] == '.') && (e[2] == '\0') )
				{
					return ReadNextEntry();
				} else
				if (SkipDotFiles)
				{
					// user doesn't want .-files, so we skip over it
					return ReadNextEntry();
				} else
				{
					return true;
				}
			} else 
			{
				// no dotfile
				return true;
			}

			
		}
	} else
		return false;
}

const char* iXDir::GetCurrentEntryName()
{
	if (m_Entry != NULL)
	{
#ifdef IXPLAT_NETWARE_CLIB
		return m_Entry->d_nameDOS;
#else
		return m_Entry->d_name;
#endif
 	} else{
 		return "";
 	}
}

long long iXDir::GetCurrentEntrySize()
{
	if (m_Entry != NULL)
		return m_Entry->d_size;
	else
		return -1;
}

bool iXDir::UnlinkCurrentEntry()
{
	if (m_Entry != NULL)
	{
		const char* e = GetCurrentEntryName();
		
		char* fullE = (char*)malloc(strlen(e) + strlen(m_DirectoryName) + strlen(IX_DIRECTORY_SEPARATOR_STR)*2 + 1);
		
		strcpy(fullE,m_DirectoryName);
		strcat(fullE,IX_DIRECTORY_SEPARATOR_STR);
		strcat(fullE,e);
		
		if (unlink(fullE))
			return false;
		else
			return true;
	} else
		return false;
}

time_t iXDir::GetCurrentEntryModificationTime()
{
	if (m_Entry != NULL)
	{
		struct stat st;
		const char* e = GetCurrentEntryName();
		
		stat(e,&st);
		
#ifdef IXPLAT_NETWARE_CLIB
		ConsolePrintf("mt:%d ",st.st_mtime);
		return st.st_mtime;
#else
		consoleprintf("mt:%d ",st.st_mtime);
		return st.st_mtime.tv_sec;
#endif
		// just in case.
		return -2;
	
	} else
		return (-1);
}
