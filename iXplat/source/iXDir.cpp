
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "iX.h"
#include "iXDir.h"
#include <sys/stat.h>
#include <nwconio.h>

#ifdef IXPLAT_NETWARE_LIBC
#include <screen.h>
#endif


iXDir::iXDir(const char* DirectoryName)
{
#ifdef IXPLAT_NETWARE_CLIB
	char* szFull = (char*)malloc(IX_PATH_MAX);
	strcpy(szFull,DirectoryName);
	
	if ((szFull[strlen(szFull)-1] == '\\') || (szFull[strlen(szFull)-1] == '/'))
		strcat(szFull,"*.*");
	else
		strcat(szFull,"\\*.*");
		
	m_DirectoryName = szFull;
	m_Directory = opendir(szFull);
//	free(szFull);
#else
	m_DirectoryName = strdup(DirectoryName);
	m_Directory = opendir(DirectoryName);
#endif

	m_Entry = NULL;
	SkipDotFiles = false;
	
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
			const char* e = this->GetCurrentEntryName();
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

const char* iXDir::GetOSDirectoryName()
{
	if (m_DirectoryName != NULL)
	{
		return m_DirectoryName;
	} else {
		return "";
	}
}

long long iXDir::GetCurrentEntrySize()
{
#ifndef IXPLAT_WIN32
	if (m_Entry != NULL)
		return m_Entry->d_size;
	else
#endif
		return -1;
}

bool iXDir::UnlinkCurrentEntry()
{
	bool rc = false;
	if (m_Entry != NULL)
	{
		const char* e = this->GetCurrentEntryName();
		char* fullE = (char*)malloc(strlen(e) + strlen(m_DirectoryName) + strlen(IX_DIRECTORY_SEPARATOR_STR)*2 + 1);
		
		strcpy(fullE,m_DirectoryName);
		strcat(fullE,IX_DIRECTORY_SEPARATOR_STR);
		strcat(fullE,e);
		
		if (unlink(fullE))
			rc = false;
		else
			rc = true;
		
		free(fullE);
		return rc;
		
	} else
		return false;
}

time_t iXDir::GetCurrentEntryModificationTime()
{
	if (m_Entry != NULL)
	{
		struct stat st;
		const char* e = this->GetCurrentEntryName();
		char* fullE = (char*)malloc(strlen(e) + strlen(m_DirectoryName) + strlen(IX_DIRECTORY_SEPARATOR_STR)*2 + 1);
		
		strcpy(fullE,m_DirectoryName);
		strcat(fullE,IX_DIRECTORY_SEPARATOR_STR);
		strcat(fullE,e);
		
		stat(fullE,&st);
		free(fullE);
		
#ifdef IXPLAT_NETWARE_LIBC
		return st.st_mtime.tv_sec;
#else
		return st.st_mtime;
#endif
		// just in case.
		return -2;
	
	} else
		return 0;
}
