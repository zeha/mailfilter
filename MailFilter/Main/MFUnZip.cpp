/* 
	MFUnZip.cpp

	Author:			Christian Hofstaedtler
	Description:	Can do ZIP decompression.
	Sources:		parts out of zip.h from zlib/contrib/minizip
*/

#define _MFD_MODULE "MFUnZip.cpp"
#include "MailFilter.h"
#include "MFUnZip.h"

MFUnZip::MFUnZip(const char* zipFilename)
{
	this->zipFile = unzOpen(zipFilename);
	if (!this->zipFile)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Opening UnZIP '%s' failed!\n",zipFilename);
		return;
	} else {
		MFD_Out(MFD_SOURCE_ZIP,"Opening UnZIP '%s': success.\n",zipFilename);
	}
}


MFUnZip::~MFUnZip()
{
	if (this->zipFile)
	{
		MFD_Out(MFD_SOURCE_ZIP,"Closing zip.\n");
		unzClose(this->zipFile);
	} else {
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: NOT Closing zip, was not open!\n");
	}
}

int MFUnZip::ExtractCurrentFile(const char* localFilename)
{
	int err = MFUnZip_OK;
	MFUnZip_Fileinfo zi;
	char filename_inzip[256];

	if (!this->zipFile)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: No Zip File open!\n");
		return MFUnZip_NOZIPFILE;
	}

	err = unzGetCurrentFileInfo(this->zipFile,&zi,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
	if (err != MFUnZip_OK)
	{
		MFD_Out(MFD_SOURCE_ZIP,"Error %d returned from GetCurrentFileInfo\n",err);
		return err;
	}

	unsigned int sizeBuf;
	void* Buf;

	sizeBuf = MFUnZip_WRITEBUFFERSIZE; // 8192
	Buf = (void*)_mfd_malloc(sizeBuf,"MFUnZip::ExtractCurrentFile");
	if (Buf == NULL)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not get enough memory.\n");
		return MFUnZip_OUTOFMEMORY;
	}

	err = unzOpenCurrentFile(this->zipFile);
	if (err != MFUnZip_OK)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not open inner zipfile.\n");
		return MFUnZip_BADZIPFILE;
	}

	FILE* fout = fopen(localFilename,"wb");
	if (fout == NULL)
	{
		unzCloseCurrentFile(this->zipFile);
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not open local file\n");
	}

	if (fout != NULL)
	{
		do {

			err = unzReadCurrentFile(this->zipFile,Buf,sizeBuf);
			if (err < 0)
			{
				MFD_Out(MFD_SOURCE_ZIP,"ERROR: %d from ReadCurrentFile.\n",err);
				break;
			}
			if (err > 0)
			{
				if (fwrite(Buf,(size_t)err,1,fout) != 1)
				{
					MFD_Out(MFD_SOURCE_ZIP,"ERROR: cant write to local file\n");
					err = MFUnZip_ERRNO;
					break;
				}
			}
		} while (err>0);

		fclose(fout);		// close local file
	}
	
	if (err == MFUnZip_OK)
	{
		err = unzCloseCurrentFile(this->zipFile);
		if (err != MFUnZip_OK)
			MFD_Out(MFD_SOURCE_ZIP,"ERROR: %d from unzCloseCurrentFile\n",err);
	} else
		unzCloseCurrentFile(this->zipFile);

	_mfd_free(Buf,"MFUnZip::ExtractCurrentFile");
	return err;
		
}

int MFUnZip::ExtractFile(const char* innerFilename, const char* localFilename)
{
	if (!this->zipFile)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: No Zip File open!\n");
		return MFUnZip_NOZIPFILE;
	}

	if (unzLocateFile(this->zipFile,innerFilename,2) != MFUnZip_OK)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: Could not find '%s' in zipfile.\n",innerFilename);
		return MFUnZip_FILENOTFOUND;
	}
	return this->ExtractCurrentFile(localFilename);
}

iXList* MFUnZip::ReadZipContents()
{
	iXList* lst = new iXList();
	
	unsigned long i;
	MFUnZip_GlobalInfo gi;
	int err;

	err = unzGetGlobalInfo (this->zipFile,&gi);
	if (err!=MFUnZip_OK)
	{
		MFD_Out(MFD_SOURCE_ZIP,"ERROR: zipfile prob (%d) in unzGetGlobalInfo\n",err);
	}
	for (i=0;i<gi.number_entry;i++)
	{
		char filename_inzip[256];
		MFUnZip_Fileinfo file_info;
		unsigned long ratio=0;
		err = unzGetCurrentFileInfo(this->zipFile,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
		if (err!=MFUnZip_OK)
		{
//			printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			break;
		}

		lst->AddValueBool(filename_inzip,false);

		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(this->zipFile);
			if (err!=MFUnZip_OK)
			{
				MFD_Out(MFD_SOURCE_ZIP,"ERROR: zipfile prob (%d) in unzGoToNextFile\n",err);
				break;
			}
		}
	}
	
	return lst;
}

