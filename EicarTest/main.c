#include <screen.h>
#include <stdio.h>
#include <nks/dirio.h>
#include <client.h>
#include <errno.h>

static NXPathCtx_t	MFT_PathCtxt_ServerConnection = 0;
static NXPathCtx_t	MFT_PathCtxt_UserConnection = 0;
static int			MFT_ServerIdentity = 0;


static bool MailFilterApp_Server_LoginToServer()
{	
	int            err; 
	 
	err = create_identity ("ICC", "cn=MailFilter.ou=System.o=ICC", "mf", NULL, XPORT_WILD|USERNAME_ASCII, &MFT_ServerIdentity);
	if (err)
	{
		printf(" * Details: create_identity failed with rc=%d, errno: %d\n",err,errno);
		return false;
	}

	err = NXCreatePathContext(0, "SYS:", NX_PNF_DEFAULT, NULL, &MFT_PathCtxt_ServerConnection);
	if (err)	return false;

	err = NXCreatePathContext(0, "SYS:", NX_PNF_DEFAULT, (void *) MFT_ServerIdentity, &MFT_PathCtxt_UserConnection);
	if (err)	return false;

	return true;
}

bool MailFilterApp_Server_SelectServerConnection()
{
	int err = setcwd(MFT_PathCtxt_ServerConnection);
	if (err)	return false;
	
	return true;
}

bool MailFilterApp_Server_SelectUserConnection()
{
	int err = setcwd(MFT_PathCtxt_UserConnection);
	if (err)	return false;
	
	return true;
}

static bool MailFilterApp_Server_LogoutFromServer()
{
	NXFreePathContext(MFT_PathCtxt_ServerConnection);
	MFT_PathCtxt_ServerConnection = 0;
	
	NXFreePathContext(MFT_PathCtxt_UserConnection);
	MFT_PathCtxt_UserConnection = 0;
	
	delete_identity(MFT_ServerIdentity);
	return true;
}


void main(int argc, char* argv[])
{
	FILE* f;
	const char* foo = "X5O!P%@AP[4\\PZX54(P^)7CC)7}$";
	const char* bar = "EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
	const char* destFile = "SYS:\\EICAR.COM";

	printf("  MailFilter EICAR.COM Test Utility\n");
	printf("  Usage: EICARTST.NLM [Filename]\n");
	printf("\n");
	printf("  EICAR Test File will be written to:\n");
	if (argc>1)
		destFile = argv[1];
	
	printf("       %s\n", destFile);


	if (!MailFilterApp_Server_LoginToServer())
		printf("  -!> ERROR logging in to server!\n");

	if (!MailFilterApp_Server_SelectUserConnection())
		printf("  -!> ERROR selecting new server connection!\n");

	
	f = fopen(destFile,"w");
	fprintf(f,"%s%s",foo,bar);
	fclose(f);

	printf("  --> Done!\n");
	
	pressenter();

	MailFilterApp_Server_LogoutFromServer();	
}
