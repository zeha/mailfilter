

#include "stdafx.h"

#include <cstddef>
#include "../MailFilter/Config/MFConfig-Filter.h++"
#include "../MailFilter/Config/MFConfig-defines.h"
#include "../MailFilter/Main/mftools.h"


std::vector<MailFilter_Configuration::Filter> MFC_FilterList;

#define MAX_BUF 5000
using namespace std;

class FilterPrinter_Base
{
protected:
	FILE* fFile;

public:	
	FilterPrinter_Base(FILE* file) { fFile = file; }
	virtual void startfile() { };
	virtual void endfile() { };
	virtual void operator() (MailFilter_Configuration::Filter& flt) = 0;
	
};


class FilterPrinter_CSV : public FilterPrinter_Base
{
public:
	
	FilterPrinter_CSV(FILE* file) : FilterPrinter_Base(file) {}

	void operator() (MailFilter_Configuration::Filter& flt) {
			fprintf(this->fFile,"0x%02x,0x%02x,0x%02x,0x%02x,%d,%d,%d,\"%s\",\"%s\"\r\n",
				flt.matchfield,
				flt.notify,
				flt.type,
				flt.action,
				flt.enabled,
				flt.enabledIncoming,
				flt.enabledOutgoing,
				flt.expression.c_str(),
				flt.name.c_str()
				);
	}
};

class FilterPrinter_TAB : public FilterPrinter_Base
{
public:
	
	FilterPrinter_TAB(FILE* file) : FilterPrinter_Base(file) {}

	void operator() (MailFilter_Configuration::Filter& flt) {
			fprintf(this->fFile,"0x%02x\t0x%02x\t0x%02x\t0x%02x\t%d\t%d\t%d\t%s\t%s\r\n",
				flt.matchfield,
				flt.notify,
				flt.type,
				flt.action,
				flt.enabled,
				flt.enabledIncoming,
				flt.enabledOutgoing,
				flt.expression.c_str(),
				flt.name.c_str()
				);
	}
};

class FilterPrinter_BIN : public FilterPrinter_Base
{
public:
	
	FilterPrinter_BIN(FILE* file) : FilterPrinter_Base(file) {}

	void operator() (MailFilter_Configuration::Filter& flt) {
			fputc(flt.matchfield,fFile);
			fputc(flt.notify,fFile);
			fputc(flt.type,fFile);
			fputc(flt.action,fFile);
			fputc(flt.enabled,fFile);
			fputc(flt.enabledIncoming,fFile);
			fputc(flt.enabledOutgoing,fFile);

			// v11+
				// stock filter
				fputc(0,fFile);
				// 4 bytes for counter
				fputc(0,fFile);
				fputc(0,fFile);
				fputc(0,fFile);
				fputc(0,fFile);
			// ////

			fprintf(fFile,"%s",flt.expression.c_str());
			fputc(0,fFile);
			fprintf(fFile,"%s",flt.name.c_str());
			fputc(0,fFile);
	}
	void startfile()
	{
		fprintf(fFile,MFFLTSIG);
		fputc(0,fFile);
		fprintf(fFile,MFCSIG);
		fputc(0,fFile);
		fputc(0,fFile);
		fputc(0,fFile);
		fputc(0,fFile);
	}
	void endfile()
	{
	}
};

class FilterReader_Base
{
protected:
	FILE* fFile;

public:	
	FilterReader_Base(FILE* file) { fFile = file; }
	virtual int read(std::vector<MailFilter_Configuration::Filter> *MFC_FilterList) = 0;
	
};

class FilterReader_BIN : public FilterReader_Base
{
public:
	FilterReader_BIN(FILE* file) : FilterReader_Base(file) {}

	int read(std::vector<MailFilter_Configuration::Filter> *MFC_FilterList)
	{
		char szTemp[MAX_BUF+10];
		int rc;
		
		//
#define MFFLTSIG "MAILFILTER_FILTER_EXCHANGEFILE"		
		// Read In Configuration Version/Build
		fread(szTemp,sizeof(char),strlen(MFFLTSIG)+1,fFile);
		if (strcmp(szTemp,MFFLTSIG) != 0)
		{
			fclose(fFile);
			return -3;
		}
		fread(szTemp,sizeof(char),strlen(MFCSIG)+1,fFile);
		if (strcmp(szTemp,MFCSIG) != 0)
		{
			fclose(fFile);
			return -4;
		}

		fseek(fFile, 54+1, SEEK_SET);
		while (!feof(fFile))
		{
			fseek(fFile,-1,SEEK_CUR);
			MailFilter_Configuration::Filter* flt = new MailFilter_Configuration::Filter();

			flt->matchfield = (MailFilter_Configuration::FilterField)(fgetc(fFile));
			flt->notify = (MailFilter_Configuration::Notification)(fgetc(fFile));
			flt->type = (MailFilter_Configuration::FilterType)(fgetc(fFile));
			flt->action = (MailFilter_Configuration::FilterAction)(fgetc(fFile));
			flt->enabled = (bool)(fgetc(fFile));
			flt->enabledIncoming = (bool)(fgetc(fFile));
			flt->enabledOutgoing = (bool)(fgetc(fFile));

			// v11+
				// stock filter
				fgetc(fFile);
				// 4 bytes for counter
				fgetc(fFile);
				fgetc(fFile);
				fgetc(fFile);
				fgetc(fFile);
			// ---

			rc = (long)fread(szTemp,sizeof(char),1000,fFile);
			fseek(fFile,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);
			
			if (strlen(szTemp) > 0) flt->expression = szTemp;

			rc = (long)fread(szTemp,sizeof(char),1000,fFile);
			fseek(fFile,((int)(strlen(szTemp)))-rc+1,SEEK_CUR);

			if (strlen(szTemp) > 0) flt->name = szTemp;
			
			if (flt->expression == "")
			{
				delete flt;
				break;
			}
					
			MFC_FilterList->push_back(*flt);
			
			fgetc(fFile);
		}

		return 0;
	}
};


class FilterReader_CSV : public FilterReader_Base
{
private:
	int extractString(const char* inStr, std::string& szOut)
	{
		int cpos = 0;
		int outpos = 0;
		int strStart = -1;
		int tmpCharsRead = 0;
		size_t len = strlen(inStr);
		bool isEnclosed = true;
		int numEnclosures = 0;

		while ( (inStr[cpos++] != '\"') && (inStr[cpos] != 0) ) {}; // bis zum ersten " lesen
		if (inStr[cpos] == 0)
		{
			isEnclosed = false;
			cpos = 0;
		}

		if (inStr[cpos] == 0)
			return 0;
		
	//		return 0;

		szOut = "";
		strStart = cpos;
		outpos = strStart;
		for (cpos = strStart ; (unsigned int)cpos < len ; cpos++)
		{
	//		printf(" %d,%d",outpos,cpos);
			
	//**		szOut[outpos-strStart] = inStr[cpos];
			szOut += (char)inStr[cpos];
			if (inStr[cpos] == '\0')
			{	//szOut[0] = 0;	/* String ungültig..., kein 2tes " gefunden */
				szOut.empty();
				return 0;
			}

			if ( (inStr[cpos] == '\"') && (inStr[cpos-1] == '\"') )		  // "
				numEnclosures++;
				else
				numEnclosures = 0;
			
			if (
			 
				(
				
					(isEnclosed) 
					&& 
					(inStr[cpos] == '\"')  	// "
	/*				&& (
						(numEnclosures > 1)
						&&
						(inStr[cpos-1] == '\"')
						) */
					
				) 
				
				|| 
				(!isEnclosed) 
				
			)
			{ 
				if (
						( (inStr[cpos+1] == '\r') )
						||
						( (inStr[cpos+1] == '\n') )
						||
						( (inStr[cpos+1] == 0) )
						||
						( (inStr[cpos+1] == ',') ) 
						|| 
						( (inStr[cpos+1] == ' ') && (inStr[cpos+2] == ',') )
					)
				{
					if (!isEnclosed) { outpos++; }
					/* TODO ergänzen string weiterlesen/test */
	//**				szOut[outpos-strStart] = 0;
					
	/*				for (cpos=0;cpos<(strlen(szOut));cpos++)
					{	
						if ( (szOut[cpos] == '"') && (szOut[cpos+1] == '"') )
						{
							szOut[cpos] = szOut[cpos+1];
							cpos++;
						}
					}
						
	*/				
					return outpos+1;
					break;
				}
			}

			outpos++;
		}
		
		return 0;
	}

public:
	FilterReader_CSV(FILE* file) : FilterReader_Base(file) {}

	int read(std::vector<MailFilter_Configuration::Filter> *MFC_FilterList)
	{
		char szTemp[MAX_BUF];
		while (!feof(fFile))
		{
			int matchfield;
			int notify;
			int type;
			int action;
			int enabled;
			int enabledIn;
			int enabledOut;

			// and the new style
			MailFilter_Configuration::Filter* flt = new MailFilter_Configuration::Filter();

			int rc = fscanf(fFile," 0x%hhx %*[,] 0x%hhx %*[,] 0x%hhx %*[,] 0x%hhx %*[,] %hhd %*[,] %hhd %*[,] %hhd %*[,]",// %*[\"]%[abcdefghijklmnopqrstuvwxyzABCDEFQRSTUVWXYZ -_.,;:@!"§$%&/()=?0123456789]%*[\"] %*[,] %*[\"]%s%*[\"] \n",
				&matchfield,
				&notify,
				&type,
				&action,
				&enabled,
				&enabledIn,
				&enabledOut);

			// this is to prevent the possible page faults from fscanf
			// TODO: replace fscanf with a C++ solution
			flt->matchfield = (MailFilter_Configuration::FilterField)(matchfield);
			flt->notify = (MailFilter_Configuration::Notification)(notify);
			flt->type = (MailFilter_Configuration::FilterType)(type);
			flt->action = (MailFilter_Configuration::FilterAction)(action);
			flt->enabled = (bool)(enabled);
			flt->enabledIncoming = (bool)(enabledIn);
			flt->enabledOutgoing = (bool)(enabledOut);

			szTemp[0]=0;
			fgets(szTemp,1000,fFile);
			
			printf("0x%02x 0x%02x 0x%02x 0x%02x %d %d %d ",flt->matchfield,flt->notify,flt->type,flt->action,flt->enabled,flt->enabledIncoming,flt->enabledOutgoing);

			rc = this->extractString(szTemp,flt->name);
			printf("%s - ",flt->name.c_str());
			rc = this->extractString(szTemp+rc,flt->expression);
			printf("%s\n",flt->expression.c_str());

			if (flt->expression == "")
			{
				delete flt;
				break;
			}
			MFC_FilterList->push_back(*flt);

		}
		return 0;
	}
};

class FilterReader_TAB: public FilterReader_Base
{
private:
	size_t extractString(const char* inStr, std::string& szOut)
	{
		int cpos = 1;
		int outpos = 0;
		int strStart = -1;
		int tmpCharsRead = 0;
		size_t len = strlen(inStr);
		char szTmp[2];

		szOut.empty();

		while ( 
					(inStr[cpos] != '\t') && 
					(inStr[cpos] != '\r') && 
					(inStr[cpos] != '\n') && 
					(inStr[cpos] != 0)
			)
		{
			szTmp[0] = inStr[cpos];
			szTmp[1] = 0;
			szOut+=szTmp;
			
			cpos++;
		}

		return szOut.length();
	}
public:
	FilterReader_TAB(FILE* file) : FilterReader_Base(file) {}

	int read(std::vector<MailFilter_Configuration::Filter> *MFC_FilterList)
	{	int rc = -1;
		char szTemp[MAX_BUF];

		int matchfield;
		int notify;
		int type;
		int action;
		int enabled;
		int enabledIncoming;
		int enabledOutgoing;

		while (!feof(fFile))
		{
			// and the new style
			MailFilter_Configuration::Filter* flt = new MailFilter_Configuration::Filter();

			rc = fscanf(fFile,"0x%hhx\t0x%hhx\t0x%hhx\t0x%hhx\t%hhd\t%hhd\t%hhd",// %*[\"]%[abcdefghijklmnopqrstuvwxyzABCDEFQRSTUVWXYZ -_.,;:@!"§$%&/()=?0123456789]%*[\"] %*[,] %*[\"]%s%*[\"] \n",
				&matchfield,
				&notify,
				&type,
				&action,
				&enabled,
				&enabledIncoming,
				&enabledOutgoing
				);

			if (rc == -1)
			{
				delete flt;
				break;
			}

			printf("%d ",rc);

			// TODO: replace fscanf with a C++ solution
			flt->matchfield = (MailFilter_Configuration::FilterField)(matchfield);
			flt->notify = (MailFilter_Configuration::Notification)(notify);
			flt->type = (MailFilter_Configuration::FilterType)(type);
			flt->action = (MailFilter_Configuration::FilterAction)(action);
			flt->enabled = (bool)(enabled);
			flt->enabledIncoming = (bool)(enabledIncoming);
			flt->enabledOutgoing = (bool)(enabledOutgoing);

			szTemp[0]=0;
			fgets(szTemp,1000,fFile);
			
			printf("%4d: 0x%02x 0x%02x 0x%02x 0x%02x %d %d %d ",MFC_FilterList->size(),flt->matchfield,flt->notify,flt->type,flt->action,flt->enabled,flt->enabledIncoming,flt->enabledOutgoing);

			rc = (int)extractString(szTemp,flt->expression);
			printf("%s - ",flt->expression.c_str());
			rc++;
			rc = (int)extractString(szTemp+rc,flt->name);
			printf("%s \n",flt->name.c_str());

			if (flt->expression == "")
			{
				delete flt;
				break;
			}
			MFC_FilterList->push_back(*flt);
		}
		return 0;
	}
};


int fucking_DoIt_BIN(std::vector<MailFilter_Configuration::Filter> *MFC_FilterList, FILE* fFile)
{
	FilterPrinter_BIN printer(fFile);
	printer.startfile();
	for_each(MFC_FilterList->begin(),MFC_FilterList->end(),printer);
	printer.endfile();
	return 0;
}

int fucking_DoIt_TAB(std::vector<MailFilter_Configuration::Filter> *MFC_FilterList, FILE* fFile)
{
	FilterPrinter_TAB printer(fFile);
	printer.startfile();
	std::for_each(MFC_FilterList->begin(),MFC_FilterList->end(),printer);
	printer.endfile();
	return 0;
}

int fucking_DoIt_CSV(std::vector<MailFilter_Configuration::Filter> *MFC_FilterList, FILE* fFile)
{
	FilterPrinter_CSV printer(fFile);
	printer.startfile();
	std::for_each(MFC_FilterList->begin(),MFC_FilterList->end(),printer);
	printer.endfile();
	return 0;
}

int ExecuteRead(const char* szInFile, int inType)
{
	FILE* fFile;
	int rc = 0;
	
	//printf("Check: %s->%s (%d->%d)\n",szInFile,szOutFile,inType,outType);

	/* read */
	FilterReader_Base *reader;
	
	if ((fFile = fopen(szInFile,"rt")) == NULL)
	{
		return -1;
	}

	switch (inType)
	{
	case 0:
		reader = (FilterReader_Base*)new FilterReader_BIN(fFile);
		break;
	case 1:
		reader = (FilterReader_Base*)new FilterReader_TAB(fFile);
		break;
	case 2:
		return -1;
	default:
		fclose(fFile);
		return -2;
	}
	rc = reader->read(&MFC_FilterList);
	
	delete reader;
	fclose(fFile);
	return rc;
}

int ExecuteWrite(const char* szOutFile, int outType)
{
	FILE* fFile;
	int rc = 0;
	/* write */

	if ((fFile = fopen(szOutFile,"wb")) == NULL)
	{
		return -1;
	}

	switch (outType)
	{
	case 0:
		rc = fucking_DoIt_BIN(&MFC_FilterList,fFile);
		break;
	case 1:
		rc = fucking_DoIt_TAB(&MFC_FilterList,fFile);
		break;
	case 2:
		rc = fucking_DoIt_CSV(&MFC_FilterList,fFile);
		break;
	default:
		fclose(fFile);
		return -2;
	}
	
	fclose(fFile);
	return rc;
}
