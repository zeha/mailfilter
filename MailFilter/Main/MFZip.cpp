/* 
	MFZip.cpp

	Author:			Christian Hofstaedtler
	Description:	Can do ZIP compression.
	Sources:		parts out of zip.h from zlib/contrib/minizip
*/

#define _MFD_MODULE "MFZip.cpp"
#include "MailFilter.h"
#include "MFZip.h"

MFZip::MFZip(const char* zipFilename, int compressLevel, unsigned int flags)
{
	this->compressionLevel = compressLevel;
	
	if (flags)
		unlink(zipFilename);
	
	this->zipFile = zipOpen(zipFilename,0);
	if (!this->zipFile)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Opening ZIP '%s' failed!\n",zipFilename);
		return;
	} else {
		MFD_Out(MFD_SOURCE_ZIP,"Opening ZIP '%s': success.\n",zipFilename);
	}
}


MFZip::~MFZip()
{
	if (this->zipFile)
	{
		MFD_Out(MFD_SOURCE_ZIP,"Closing zip.\n");
		zipClose(this->zipFile,NULL);
	} else {
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: NOT Closing zip, was not open!\n");
	}
}

int MFZip::AddFile(const char* innerFilename, const char* localFilename)
{
	int err;
	MFZip_Fileinfo zi;
	zi.dosDate = 0;
	zi.internal_fa = 0; zi.external_fa = 0;
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
	zi.tmz_date.tm_mday = zi.tmz_date.tm_min = zi.tmz_date.tm_year = 0;
	
	MFD_Out(MFD_SOURCE_ZIP,"Adding '%s'.\n",localFilename);

	if (!this->zipFile)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: No Zip File open!\n");
		return MFZip_NOZIPFILE;
	}

	void* buf=NULL;
	unsigned int buf_size;

    buf_size = MFZip_WRITEBUFFERSIZE;
    buf = (void*)_mfd_malloc(buf_size,"MFZip::AddFile");
    if (buf == NULL)
    {
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not get memory.\n");
    	return MFZip_INTERNALERROR;
    }

	FILE* fin = fopen(localFilename,"rb");

	if (fin == NULL)
	{
		zipCloseFileInZip(this->zipFile);
		_mfd_free(buf,"MFZip::AddFile");
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not open local file.\n");
		return MFZip_ERR_LOCALFILEOPEN;
	}
	
	
	err = zipOpenNewFileInZip(this->zipFile,innerFilename,&zi,
                                 NULL,0,NULL,0,NULL /* comment*/,
                                 (this->compressionLevel != 0) ? MFZip_DEFLATED : 0,
                                 this->compressionLevel);

	if (err != MFZip_OK)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not open inner zipfile.\n");
		fclose(fin);
		
		return MFZip_ERR_INNERFILEOPEN;
		
	} else {
		unsigned int size_read;

		do {
			err = MFZip_OK;
			size_read = fread(buf,1,buf_size,fin);
			if (size_read < buf_size)
				if (feof(fin)==0)
			{
				MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not read local file.\n");
				err = MFZip_ERR_LOCALFILEREAD;
			}
			
			if (size_read>0)
			{
				err = zipWriteInFileInZip ( this->zipFile, buf, size_read );
				if (err<0)
				{
					MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not write inner zipfile.\n");
					err = MFZip_ERR_INNERFILEWRITE;
				}
			}
		} while ((err == MFZip_OK) && (size_read>0));
		
		_mfd_free(buf,"MFZip::AddFile");
		fclose(fin);
		if (err>=0)
		{
			err = zipCloseFileInZip(this->zipFile);
			if (err != MFZip_OK)
			{
				MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not close inner zipfile.\n");
				return MFZip_ERR_INNERFILECLOSE;
			}
		}
	}

	MFD_Out(MFD_SOURCE_ZIP,"Success adding file.\n",localFilename);
	
	return MFZip_OK;	
}

