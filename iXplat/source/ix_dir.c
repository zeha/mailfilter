#define _IX_LIBRARY
#include "ix.h"

#ifdef IXPLAT_WIN32
/*
**
** Local implementations of standard Unix RTL functions which are not provided
** by the VC RTL.
**
*/
#define GetFileFromDIR(d)       (d)->d_entry.cFileName
#define FileIsHidden(d)       ((d)->d_entry.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)

void FlipSlashes(char *cp, int len)
{
    while (--len >= 0) {
        if (cp[0] == '/') {
            cp[0] = IX_DIRECTORY_SEPARATOR;
        }
        cp++;
    }
} /* end FlipSlashes() */


IXStatus ix_dirclose(IX__Dir *d)
{
    if ( d ) {
        if (FindClose( d->d_hdl )) {
            d->magic = (int)-1;
            return IX_SUCCESS;
        } else {
//            _PR_MD_MAP_CLOSEDIR_ERROR(GetLastError());
            return IX_FAILURE;
        }
    }
//    PR_SetError(PR_INVALID_ARGUMENT_ERROR, 0);
    return IX_FAILURE;
}


IXStatus ix_diropen(IX__Dir *d, const char *name)
{
    char filename[ MAX_PATH ];
    int len;

    len = strlen(name);
    /* Need 5 bytes for \*.* and the trailing null byte. */
    if (len + 5 > MAX_PATH) {
//        PR_SetError(PR_NAME_TOO_LONG_ERROR, 0);
        return IX_FAILURE;
    }
    strcpy(filename, name);

    /*
     * If 'name' ends in a slash or backslash, do not append
     * another backslash.
     */
    if (filename[len - 1] == '/' || filename[len - 1] == '\\') {
        len--;
    }
    strcpy(&filename[len], "\\*.*");
    FlipSlashes( filename, strlen(filename) );

    d->d_hdl = FindFirstFile( filename, &(d->d_entry) );
    if ( d->d_hdl == INVALID_HANDLE_VALUE ) {
//		_PR_MD_MAP_OPENDIR_ERROR(GetLastError());
        return IX_FAILURE;
    }
    d->firstEntry = IX_TRUE;
    d->magic = IX_MAGIC_DIR;
    return IX_SUCCESS;
}

char * ix_dirread(IX__Dir *d, IX_DirFlags flags)
{
    int err;
    BOOL rv;
    char *fileName;

    if ( d ) {
        while (1) {
            if (d->firstEntry) {
                d->firstEntry = IX_FALSE;
                rv = 1;
            } else {
                rv = FindNextFile(d->d_hdl, &(d->d_entry));
            }
            if (rv == 0) {
                break;
            }
            fileName = GetFileFromDIR(d);
            if ( (flags & IX_SKIP_DOT) &&
                 (fileName[0] == '.') && (fileName[1] == '\0'))
                 continue;
            if ( (flags & IX_SKIP_DOT_DOT) &&
                 (fileName[0] == '.') && (fileName[1] == '.') &&
                 (fileName[2] == '\0'))
                 continue;
            if ( (flags & IX_SKIP_HIDDEN) && FileIsHidden(d))
                 continue;
            return fileName;
        }
	err = GetLastError();
//	PR_ASSERT(NO_ERROR != err);
//			_PR_MD_MAP_READDIR_ERROR(err);
	return NULL;
		}
//    PR_SetError(PR_INVALID_ARGUMENT_ERROR, 0);
    return NULL;
}

IXStatus ix_delete(const char *name)
{
    if (DeleteFile(name)) {
		return IX_SUCCESS;
	} else {
//		_PR_MD_MAP_DELETE_ERROR(GetLastError());
		return IX_FAILURE;
	}
}

#endif // IXPLAT_WIN32


#ifdef IXPLAT_UNIX

IXStatus ix_opendir(IX__Dir *d, const char *name)
{
int err;

    d->d = opendir(name);
    if (!d->d) {
//        err = errno();
//        _PR_MD_MAP_OPENDIR_ERROR(err);
        return IX_FAILURE;
    }
    return IX_SUCCESS;
}

IXStatus ix_closedir(IX__Dir *d)
{
int rv = 0, err;

    if (d->d) {
        rv = closedir(d->d);
        if (rv == -1) {
//                err = _MD_ERRNO();
//                _PR_MD_MAP_CLOSEDIR_ERROR(err);
			return IX_FAILURE;
        }
    }
    return IX_SUCCESS;
}

char * ix_readdir(IX__Dir *d, IX_DirFlags flags)
{
struct dirent *de;
int err;

    for (;;) {
        /*
          * XXX: readdir() is not MT-safe. There is an MT-safe version
          * readdir_r() on some systems.
          */
        de = readdir(d->d);
        if (!de) {
//            err = _MD_ERRNO();
//            _PR_MD_MAP_READDIR_ERROR(err);
            return 0;
        }        
        if ((flags & IX_SKIP_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == 0))
            continue;
        if ((flags & IX_SKIP_DOT_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == '.') &&
            (de->d_name[2] == 0))
            continue;
        if ((flags & IX_SKIP_HIDDEN) && (de->d_name[0] == '.'))
            continue;
        break;
    }
    return de->d_name;
}

IXStatus ix_delete(const char *name)
{
	int rv = -1;

    rv = unlink(name);

    if (rv == -1) {
		return IX_FAILURE;
    }
    return IX_SUCCESS;
}


#endif // IXPLAT_UNIX


#ifdef IXPLAT_NETWARE_CLIB

IXStatus ix_diropen(struct IX__Dir *d, const char *name)
{
//int err;

    d->d = opendir(name);
    if (!d->d) {
        return IX_FAILURE;
    }
    return IX_SUCCESS;
}

IXStatus ix_dirclose(struct IX__Dir *d)
{
int rv = 0; //, err;

    if (d->d) {
        rv = closedir(d->d);
        if (rv == -1) {
			return IX_FAILURE;
        }
    }
    return IX_SUCCESS;
}

char * ix_dirread(struct IX__Dir *d, IX_DirFlags flags)
{
struct dirent *de;
//int err;

    for (;;) {
        de = readdir(d->d);
        if (!de) {
            return 0;
        }        
        if ((flags & IX_SKIP_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == 0))
            continue;
        if ((flags & IX_SKIP_DOT_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == '.') &&
            (de->d_name[2] == 0))
            continue;
        if ((flags & IX_SKIP_HIDDEN) && (de->d_name[0] == '.'))
            continue;
        break;
    }
    return de->d_name;
}

char * ix_dirread_83(struct IX__Dir *d, IX_DirFlags flags)
{
struct dirent *de;
//int err;

    for (;;) {
        de = readdir(d->d);
        if (!de) {
            return 0;
        }        
        if ((flags & IX_SKIP_DOT) &&
            (de->d_nameDOS[0] == '.') && (de->d_nameDOS[1] == 0))
            continue;
        if ((flags & IX_SKIP_DOT_DOT) &&
            (de->d_nameDOS[0] == '.') && (de->d_nameDOS[1] == '.') &&
            (de->d_nameDOS[2] == 0))
            continue;
        if ((flags & IX_SKIP_HIDDEN) && (de->d_nameDOS[0] == '.'))
            continue;
        break;
    }
    return de->d_nameDOS;
}

struct dirent * ix_direntry(struct IX__Dir *d, IX_DirFlags flags)
{
struct dirent *de;
//int err;

    for (;;) {
        de = readdir(d->d);
        if (!de) {
            return 0;
        }        
        if ((flags & IX_SKIP_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == 0))
            continue;
        if ((flags & IX_SKIP_DOT_DOT) &&
            (de->d_name[0] == '.') && (de->d_name[1] == '.') &&
            (de->d_name[2] == 0))
            continue;
        if ((flags & IX_SKIP_HIDDEN) && (de->d_name[0] == '.'))
            continue;
        break;
    }
    return de;
}

IXStatus ix_delete(const char *name)
{
	int rv = -1;

    rv = unlink(name);

    if (rv == -1) {
		return IX_FAILURE;
    }
    return IX_SUCCESS;
}


#endif // IXPLAT_NETWARE_CLIB