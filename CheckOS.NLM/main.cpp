#include <screen.h>
#include <stdio.h>
#include <errno.h>
#include <netware.h>
#include <string.h>
#include <library.h>
#include <malloc.h> 
#include <dlfcn.h>
#include <unistd.h>
#include <ndkvers.h>

#define STATE_FAIL	2
#define STATE_PASS	1
#define STATE_UNDEF	0


	typedef int(*InoScanInit_t)(void*);
	InoScanInit_t InoScanInit_sym;
	
	scr_t gScreen;
	bool gOnlyUseLibScreenFuncs = false;
	bool gAutoClose = false;
	
	bool bOS_LibC = false;
	bool bApp_eTrust = false;
	bool bApp_GwiaCfg = false;
	
void printCheck(const char* szCheck)
{
	if (!szCheck)
		return;
	
	printf("    %s:",szCheck);
}

void printState(int state)
{
	const char* stateText = "undef";
	unsigned char color = COLOR_ATTR_YELLOW;
	uint16_t row;
	uint16_t col;

	
	switch (state)
	{
		case STATE_FAIL:
			stateText = "fail";
			color = COLOR_ATTR_RED;
			break;
		case STATE_PASS:
			stateText = "pass";
			color = COLOR_ATTR_LIME;
			break;
	}

	if (gOnlyUseLibScreenFuncs)
	{
		printf(" %s\n",stateText);
	} 
		else
	{
		// pretty print output
		
		GetOutputCursorPosition(gScreen,&row,&col);
		col = 80-8;
		PositionOutputCursor(gScreen,row,col);
		
		OutputToScreenWithAttribute(gScreen,color,"%s\n",stateText);
	}
}

int main(int argc, char* argv[])
{
	gScreen = getscreenhandle();
	
	if (argc>1)
	{
		if (strcmp(argv[1],"--log") == 0)
		{
			gOnlyUseLibScreenFuncs = true;
			gAutoClose = true;
		}
		if (strcmp(argv[1],"--plain") == 0)
			gOnlyUseLibScreenFuncs = true;
	}

	int libthreshold = 0;
	
	printf("\n MailFilter System Diagnostics:\n\n");

	// libc
	printCheck("LibC Version");
	if (libcthreshold( NETWARE_CSP11, &libthreshold ))
		printState(STATE_FAIL);
	else
		{ bOS_LibC = true; printState(STATE_PASS); }

	// etrust av
	printCheck("CA eTrust AntiVirus 7");
	InoScanInit_sym = (InoScanInit_t)ImportPublicObject(getnlmhandle(),"inoscan@InoScanInit");
	if (InoScanInit_sym == NULL)
		printState(STATE_FAIL);
		else
		{ bApp_eTrust = true; printState(STATE_PASS); }
	
	// sys:\system\gwia.cfg
	printCheck("GWIA Configuration");
	if (access("SYS:\\SYSTEM\\GWIA.CFG",F_OK))
		printState(STATE_UNDEF);
		else
		{ bApp_GwiaCfg = true; printState(STATE_PASS); }
	
	printf("\n\n Explanations:\n");
	
	if (!bOS_LibC)
	{
		printf(	"  Your LibC.NLM is too old to support MailFilter properly.\n"
				"  Please upgrade to a newer support pack. Else, you may want to check for\n"
				"  Novell Field Test Files (NWLIBx.EXE) from support.novell.com.\n\n");
	}
	
	if (!bApp_eTrust)
	{
		printf( "  CA eTrust AntiVirus may not be running or is not installed. MailFilter can\n"
				"  NOT use this AntiVirus product for scanning e-mails in this configuration.\n"
				"  Make sure eTrust AV is running properly and all fixes have been applied.\n\n");
	}
	
	if (!bApp_GwiaCfg)
	{
		printf( "  GWIA.CFG is not located at SYS:SYSTEM. You will have to specify the proper\n"
				"  path to the installation program.\n\n");
	}
	
	printf("\n\n");
	
	if (!gAutoClose)
		pressenter();

	return 0;
}

// -eof-
