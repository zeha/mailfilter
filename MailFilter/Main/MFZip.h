/* 
	MFZip.h

	Author:			Christian Hofstaedtler
	Description:	Can do ZIP compression.
	Sources:		parts out of zip.h from zlib/contrib/minizip
*/

#ifndef MFZip_H
#define MFZip_H

#define MFZip_DEFLATED   8
#define MFZip_WRITEBUFFERSIZE (16384)
typedef void* zipF;

#define MFZip_OK                                  (0)
#define MFZip_ERRNO               (Z_ERRNO)
#define MFZip_PARAMERROR                  (-102)
#define MFZip_INTERNALERROR               (-104)
#define MFZip_NOZIPFILE					(-200)
#define MFZip_ERR_INNERFILEOPEN			(-210)
#define MFZip_ERR_INNERFILEREAD			(-211)
#define MFZip_ERR_INNERFILEWRITE		(-211)
#define MFZip_ERR_INNERFILECLOSE		(-213)
#define MFZip_ERR_LOCALFILEOPEN			(-220)
#define MFZip_ERR_LOCALFILEREAD			(-221)
#define MFZip_ERR_LOCALFILEWRITE		(-222)
#define MFZip_ERR_LOCALFILECLOSE		(-223)


/* tm_zip contain date/time info */
typedef struct tm_zip_s 
{
	unsigned int tm_sec;            /* seconds after the minute - [0,59] */
	unsigned int tm_min;            /* minutes after the hour - [0,59] */
	unsigned int tm_hour;           /* hours since midnight - [0,23] */
	unsigned int tm_mday;           /* day of the month - [1,31] */
	unsigned int tm_mon;            /* months since January - [0,11] */
	unsigned int tm_year;           /* years - [1980..2044] */
} tm_zip;

typedef struct
{
	tm_zip      tmz_date;       /* date in understandable format           */
    unsigned long       dosDate;       /* if dos_date == 0, tmu_date is used      */
/*    uLong       flag;        */   /* general purpose bit flag        2 bytes */

    unsigned long       internal_fa;    /* internal file attributes        2 bytes */
    unsigned long       external_fa;    /* external file attributes        4 bytes */
} MFZip_Fileinfo;


extern "C" {
extern zipF zipOpen (const char *pathname, int append);
extern int zipOpenNewFileInZip (zipF file,
					   const char* filename,
					   const MFZip_Fileinfo* zipfi,
					   const void* extrafield_local,
					   unsigned int size_extrafield_local,
					   const void* extrafield_global,
					   unsigned int size_extrafield_global,
					   const char* comment,
					   int method,
					   int level);
extern int zipWriteInFileInZip (zipF file,
					   const void* buf,
					   unsigned len);
extern int zipCloseFileInZip (zipF file);
extern int zipClose (zipF file,
				const char* global_comment);
}

class MFZip
{
public:
	/* flags: currently only >0 results in unlink() first */
	MFZip(char* zipFilename, int compressLevel, unsigned int flags);
	/* innerfilename: how the file gets called, local: filename on local system */
	int AddFile(char* innerFilename, char* localFilename);
	/* don't forget to call the destructor; it closes the zip file */
	virtual ~MFZip();

protected:
	zipF zipFile;
	int compressionLevel;
};

#endif	// MFZip_H
