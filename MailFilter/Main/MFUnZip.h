/* 
	MFUnZip.h

	Author:			Christian Hofstaedtler
	Description:	Can do ZIP decompression.
	Sources:		parts out of unzip.h from zlib/contrib/minizip
*/

#ifndef MFUnZip_H
#define MFUnZip_H

#define MFUnZip_DEFLATED   8
#define MFUnZip_WRITEBUFFERSIZE (16384)
typedef void* unzFile;

#define z_off_t  off_t

#define MFUnZip_OK                  (0)
#define MFUnZip_ERRNO               (-1)	// Z_ERRNO
#define MFUnZip_PARAMERROR          (-102)
#define MFUnZip_INTERNALERROR       (-104)
#define MFUnZip_BADZIPFILE          (-103)
#define MFUnZip_CRCERROR            (-105)
#define MFUnZip_END_OF_LIST_OF_FILE (-1000)

#define MFUnZip_NOZIPFILE           (-2001)
#define MFUnZip_FILENOTFOUND        (-2002)
#define MFUnZip_OUTOFMEMORY         (-2003)

/* tm_unz contain date/time info */
typedef struct tm_unz_s 
{
	unsigned int tm_sec;            /* seconds after the minute - [0,59] */
	unsigned int tm_min;            /* minutes after the hour - [0,59] */
	unsigned int tm_hour;           /* hours since midnight - [0,23] */
	unsigned int tm_mday;           /* day of the month - [1,31] */
	unsigned int tm_mon;            /* months since January - [0,11] */
	unsigned int tm_year;           /* years - [1980..2044] */
} tm_unz;

typedef struct
{
    unsigned long version;              /* version made by                 2 bytes */
    unsigned long version_needed;       /* version needed to extract       2 bytes */
    unsigned long flag;                 /* general purpose bit flag        2 bytes */
    unsigned long compression_method;   /* compression method              2 bytes */
    unsigned long dosDate;              /* last mod file date in Dos fmt   4 bytes */
    unsigned long crc;                  /* crc-32                          4 bytes */
    unsigned long compressed_size;      /* compressed size                 4 bytes */ 
    unsigned long uncompressed_size;    /* uncompressed size               4 bytes */ 
    unsigned long size_filename;        /* filename length                 2 bytes */
    unsigned long size_file_extra;      /* extra field length              2 bytes */
    unsigned long size_file_comment;    /* file comment length             2 bytes */

    unsigned long disk_num_start;       /* disk number start               2 bytes */
    unsigned long internal_fa;          /* internal file attributes        2 bytes */
    unsigned long external_fa;          /* external file attributes        4 bytes */

    tm_unz tmu_date;
} MFUnZip_Fileinfo;

typedef struct 
{
	unsigned long number_entry;         /* total number of entries in
				       the central dir on this disk */
	unsigned long size_comment;         /* size of the global comment of the zipfile */
} MFUnZip_GlobalInfo;


/*
   Compare two filename (fileName1,fileName2).
   If iCaseSenisivity = 1, comparision is case sensitivity (like strcmp)
   If iCaseSenisivity = 2, comparision is not case sensitivity (like strcmpi
								or strcasecmp)
   If iCaseSenisivity = 0, case sensitivity is defaut of your operating system
	(like 1 on Unix, 2 on Windows)
*/


extern "C" {
extern unzFile unzOpen (const char *pathname);

extern int unzGetGlobalInfo (unzFile file, MFUnZip_GlobalInfo *pglobal_info);

extern int unzGoToFirstFile (unzFile file);
extern int unzGoToNextFile (unzFile file);
extern int unzLocateFile (unzFile file,
					const char* filename,
					int iCaseSensitivity);

extern int unzClose (unzFile file);

extern int unzOpenCurrentFile (unzFile file);
extern int unzCloseCurrentFile (unzFile file);
extern int unzReadCurrentFile (unzFile file, 
					  void* buf,
					  unsigned len);
extern z_off_t unztell (unzFile file);
extern int unzeof (unzFile file);

extern int unzGetCurrentFileInfo (unzFile file,
					     MFUnZip_Fileinfo *pfile_info,
					     char *szFileName,
					     unsigned long fileNameBufferSize,
					     void *extraField,
					     unsigned long extraFieldBufferSize,
					     char *szComment,
					     unsigned long commentBufferSize);


}

class MFUnZip
{
public:
	MFUnZip(const char* zipFilename);
	/* innerfilename: how the file gets called, local: filename on local system */
	int ExtractFile(const char* innerFilename, const char* localFilename);
	/* read filenames in zip */
	iXList* ReadZipContents();
	/* don't forget to call the destructor; it closes the zip file */
	virtual ~MFUnZip();

protected:
	unzFile zipFile;
	/* extract the currently selected file */
	long ExtractCurrentFile(const char* localFilename);
};

#endif	// MFUnZip_H
