/*+
 +		MFCleanup.cpp
 +		
 +		Cleaning code
 +		
 +		Copyright 2001-2004 Christian Hofstädtler.
 +		
 +		
+*/

#define _MFD_MODULE			"MFCleanup.cpp"
#include "MailFilter.h"
#include "MFVersion.h"
#include <string>

class ListItem {
	public:
	 std::string filename;
	 time_t mtime;
	 long long size;
};
class CompareItemsByTimestamp {
public:
	bool operator() 
			(const ListItem& f1, 
			 const ListItem& f2)
	{			// oldest are first
		if (f1.mtime < f2.mtime)
			return true;
		return false;
	}
};

//
// * Checks the Problem Directory for its Size and the Age of the contained files
//
void MF_CheckProblemDirAgeSize()
{
	unsigned long	cntTotalNumber = 0;
	unsigned long	cntKilledNumber = 0;

#ifdef __NOVELL_LIBC__
	long long		cntTotalSize = 0;
	long long		cntKilledSize = 0;
#else
	unsigned long	cntTotalSize = 0;
	unsigned long	cntKilledSize = 0;
#endif

	unsigned long	MaxFileAge = 0;
	long long		MaxDirSize = 0;

	std::string thisFile;
	unsigned int curItem = 0;

	unsigned long	killAge = 0;
	char			messageText[8000];

	if (MF_GlobalConfiguration->MFLTRoot == "")
		return;

	std::string baseDir = MF_GlobalConfiguration->MFLTRoot + IX_DIRECTORY_SEPARATOR_STR"MFPROB"IX_DIRECTORY_SEPARATOR_STR"DROP"IX_DIRECTORY_SEPARATOR_STR;

	MFD_Out(MFD_SOURCE_GENERIC,"Checking MFPROB size...");

	if ((MF_GlobalConfiguration->ProblemDirMaxSize == 0) && (MF_GlobalConfiguration->ProblemDirMaxAge == 0))
		return;
	
	// set up cleaner
	
	if (MF_GlobalConfiguration->ProblemDirMaxAge != 0)
		MaxFileAge = (unsigned long)(time(NULL) - (MF_GlobalConfiguration->ProblemDirMaxAge*86400));
	MaxDirSize = MF_GlobalConfiguration->ProblemDirMaxSize*1024;


	std::vector<ListItem> list;
	list.clear();

	iXDir dir(baseDir.c_str());
	while ( dir.ReadNextEntry() )
	{
		if (MFT_NLM_Exiting > 0)	break;
		
		ListItem item;

		item.filename = dir.GetCurrentEntryName();
		item.mtime = dir.GetCurrentEntryModificationTime();
		item.size = dir.GetCurrentEntrySize();
		
		list.push_back(item);

		ThreadSwitch();
	}
	sort(list.begin(),list.end(),CompareItemsByTimestamp());

	for (curItem = 0; curItem < list.size(); curItem++)
	{
		cntTotalNumber++;
		cntTotalSize = cntTotalSize + list[curItem].size;

		if (MaxFileAge > list[curItem].mtime)
		{
			cntKilledNumber++;
			cntKilledSize = cntKilledSize + list[curItem].size;

			thisFile = baseDir + list[curItem].filename;
			MFD_Out(MFD_SOURCE_VSCAN,"killage: %s\n",list[curItem].filename.c_str());
			
			list[curItem].size = 0;	// mark as deleted
			unlink(thisFile.c_str());
		}	
	}
	
	MFD_Out(MFD_SOURCE_GENERIC,"deleted %d OLD files (%lld kB)\n",cntKilledNumber,cntKilledSize/1024);
	MFD_Out(MFD_SOURCE_GENERIC,"size compare says: %lld vs %d\n", ((cntTotalSize - cntKilledSize)/1024), MaxDirSize/1024);
	
	for (curItem = 0; curItem < list.size(); curItem++)
	{
		if ((MaxFileAge > list[curItem].mtime) && (list[curItem].size != 0))
		{
			cntKilledNumber++;
			cntKilledSize = cntKilledSize + list[curItem].size;

			thisFile = baseDir + list[curItem].filename;
			unlink(thisFile.c_str());
		}	
	}
	
	// convert to kBytes	
	cntTotalSize = cntTotalSize / 1024;
	cntKilledSize = cntKilledSize / 1024;

	MFD_Out(MFD_SOURCE_GENERIC,	
#ifdef __NOVELL_LIBC__ // 64bit
			" > Total Files: %d, Size: %lld kBytes\n > Total Killed: %d, Size: %lld kBytes\n > New Totals: %d, Size: %lld kBytes\n",
#else
			" > Total Files: %d, Size: %d kBytes\n > Total Killed: %d, Size: %d kBytes\n > New Totals: %d, Size: %d kBytes\n",
#endif
			cntTotalNumber,cntTotalSize,
			cntKilledNumber,cntKilledSize,
			cntTotalNumber-cntKilledNumber,cntTotalSize - cntKilledSize);

	if (MF_GlobalConfiguration->NotificationAdminMailsKilled && cntKilledNumber)
	{

		sprintf(messageText,
			"The Problem Directory has been cleaned up.\r\n\r\nStatistics:\r\n"
#ifdef __NOVELL_LIBC__ // 64bit
			" > Total Files: %d, Size: %lld kB\r\n > Total Killed: %d, Size: %lld kB\r\n > New Totals: %d, Size: %lld kB\r\n",
#else
			" > Total Files: %d, Size: %d kB\r\n > Total Killed: %d, Size: %d kB\r\n > New Totals: %d, Size: %d kB\r\n",
#endif
			cntTotalNumber,cntTotalSize,
			cntKilledNumber,cntKilledSize,
			cntTotalNumber-cntKilledNumber,cntTotalSize - cntKilledSize);
		MF_EMailPostmasterGeneric("Problem Directory Cleanup",messageText,"","");
	}
}
