#include <screen.h>
#include <stdio.h>

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
	
	f = fopen(destFile,"w");
	fprintf(f,"%s%s",foo,bar);
	fclose(f);

	printf("  --> Done!\n");
}
