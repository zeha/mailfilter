/*
 *	MFZAP.c
 *  © Copyright 2002 Christian Hofstädtler
 *
 *	<http://www.ionus.at> 
 *	<http://www.hofstaedtler.com> 
 */

#include <stdio.h>
#include <fcntl.h>
#include <stat.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

char* zaproot()
{
	int cpos;
	char out[26];
	
	char root[] = { ']',  'V', 'C', '+', 'N', 'L', 'Z', 'P', 'B', '@',
				    'Y',  'K', '_', 'G', 'Q', '[', ':', 'W', 'i', 'd',
				   0x06, 0x0D, 'm', 'k', '`' };
	
	for (cpos=0;cpos<25;cpos++)
	{
		out[cpos] = (char)((root[cpos]) ^ (cpos+14));
		out[cpos+1] = 0;
	}

	return strdup(out);
}

void show()
{
	char ev[31];
	FILE *fEv = NULL;
	time_t etime; time_t ctime;
	fEv = fopen(zaproot(),"r");

	if (fEv != NULL)
	{
		time(&ctime);

		fread(ev,1,30,fEv);
		ev[0]=' ';ev[1]=' ';ev[2]=' ';
		ev[25]=0;
		etime = (unsigned long)atol(ev);

		printf("MFZap: Current: %d - Installed: %d\n",ctime,etime);

		if ( (ctime-etime) > 2592000 )
		{
			printf("MFZap: Expired.\n");
		} else {
		
			printf("MFZap: %d Days left.\n",30-((ctime-etime)/(60*60*24)));
		}

		fclose(fEv);

		printf("MFZap: Check Done.\n");
	
	} else {
		printf("MFZap: N/A\n");
	}
}

int main (int argc, char* argv[])
{
	if (argc > 1)
	{
		if (memicmp("-show",argv[1],5) == 0)
		{
			show();
		}
		if (memicmp("-zap",argv[1],4) == 0)
		{
			printf("MFZap: Zap\n");
			unlink(zaproot());
		}
	}
	
	return 0;
}
