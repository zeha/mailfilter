//
//  Get's the servername
//
#define _IX_LIBRARY
#include "ix.h"

void ix_getservername(char* serverName)
{
#ifdef N_PLAT_NLM
	char *volume;
	char *dir;
	volume = (char*)malloc(_MAX_VOLUME);
	dir = (char*)malloc(_MAX_DIR);

	if (serverName == NULL) return;
	
	if (sourcePath == NULL)
		ParsePath ( "SYS:" , serverName, volume, dir );		// returns local name
	else
		ParsePath ( sourcePath , serverName, volume, dir );	// returns from orig. path

	free(dir);
	free(volume);
#endif
#ifdef WIN32
	unsigned long bufSize = (MAX_COMPUTERNAME_LENGTH + 1);
	GetComputerName(serverName, &bufSize); 
#endif
}