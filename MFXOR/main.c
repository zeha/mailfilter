// A minimal Win32 console app

#include <stdio.h>



int main( int argc, char** argv[] ) {

	int cpos;
	int size;
	char* out;
	char* root;	//[37] = "C:\\System Volume Information\\_hie.sys";

	if (argc == 1)
		return -1;
	else { root = (char*)argv[1]; out=(char*)malloc(4000); size=strlen(root); }

	printf ("/* %s */\n",root);

	printf ("{");

	for (cpos=0;cpos<size;cpos++)
	{
		out[cpos] = (char)((root[cpos]) ^ (cpos+14));
		out[cpos+1] = 0;
/*		if (out[cpos] == '\\')
			printf ( "\\\\" );
		else
		if (out[cpos] == '"')
			printf ( "\\\"");
		else
		if (out[cpos] < 0x20)*/
			printf ( "0x%02x, ",out[cpos] );
/*		else
			printf ( "%c",out[cpos] );*/
	}

	printf ("},\n");

	return 0;
}