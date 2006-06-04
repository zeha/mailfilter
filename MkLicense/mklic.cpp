
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#ifdef WIN32
#include <conio.h>
#endif

#include "../MailFilter/Licensing/MFLic.h"

//extern "C"
//{					// (uLong adler, const Bytef *buf, uInt len)
unsigned long adler32 (unsigned long adler, const unsigned char *buf, unsigned int len);
//}

#ifdef N_PLAT_NLM
#include <nwbitops.h>
#include <nwconio.h>
#else
#include <math.h>
#endif

bool getYesNo()
{
	printf(" [Y/N]: ");
	char ch;
	bool q = false;
	bool ret = false; 
	do
	{
#ifdef WIN32
		ch = _getch();
#else
		ch = getchar();
#endif
		ch = toupper( ch );
		switch (ch)
		{
		case 'Y':
			ret = true;
			q = true;
			break;
		case 'N':
			ret = false;
			q = true;
			break;
		default:
			break;
		}
	} while( q == false );

	if (ret == true)
		printf("Y");
	else
		printf("N");

	return ret;
}

int main(int argc, char* argv[])
{
	unsigned int p = 0;
	unsigned int cp = 0;
	unsigned int i3 = 0;
	unsigned char keyA[24];
	unsigned char keyB[24];
	unsigned char key[sizeof(keyA)+sizeof(keyB)+5];
	unsigned char kec[sizeof(key)*2];

	time_t ltime;
	unsigned long iCheck3   = 0;

	unsigned char fCountry	= (unsigned char)233;
	unsigned char fOpSys	= (unsigned char)MAILFILTER_OPSYS_NETWARE;
	unsigned char fVersion	= (unsigned char)1;
	unsigned int  fFlags	= 0xFFFFFFFF;
	unsigned int  fTemp		= 0;
	unsigned char cCheck1	= 0;
	unsigned char cCheck2	= 0;
	
	unsigned char fOffset	= 0;
//	unsigned char fDealer	= 0;

	bool useParameters = false;
	int parametersStartAt = 1;

	char tmpc[50];
	char in[81];
	char fs[16];
	bool enable_M_VScan = false;
	bool enable_M_URedir = false;
	bool enable_M_MCopy = false;
	bool enable_M_Reserved4 = false;

	keyA[23]=0;
	keyA[22]=0;
	keyA[21]=0;
	keyB[23]=0;
	keyB[22]=0;
	keyB[21]=0;

	if (argc > 1)
	{
		if (strcasecmp("-nocon",argv[1]) == 0)
		{
			useParameters = true;
			parametersStartAt++;
		}
	}

	if (argc > 5)
	{
		useParameters = true;
	} else {
		if (useParameters)
			return 1;
	}

	if (!useParameters)
		printf("\n MAILFILTER License Generation\n (c) Copyright 2001 Christian Hofstaedtler\n\n >>Assuming AUSTRIA as Country Code.\n >>Using MAJOR Version 1.\n\n");

	// Get Current time and init server name key
	time(&ltime);
	sprintf((char*)keyA,"BMNCPDAT%08d",ltime);
	// Fill in Offset
	fOffset = ltime & 0xF;

	if (useParameters)
		strncpy(in,argv[parametersStartAt],15);
	else
	{
		printf(" ->Enter Server Name: ");
		fgets(in, 80, stdin);
	}
	in[15]=0;

	{
		char* p = in;
		while(true)
		{
			if ((*p == '\0') || (*p == '\r') || (*p == '\n'))
			{
				*p = '\0';
				break;
			}
			
			if (isalpha(*p))
			{
				*p = toupper(*p);
			}
			++p;
		}
	}

	
	strncpy(fs,in,15);
	in[15]=0;
	strcat(fs,"@");
	in[16]=0;

	memcpy(keyA,fs,strlen(fs));
	//_strupr((char*)keyA);

	fOffset = fOffset & 0x0F;

	if (!useParameters)
		printf(" ->Modules:");

	
	
	if (!useParameters)
		printf("\n  ->Virus Scan:");

	enable_M_VScan		 = useParameters ? (toupper(argv[parametersStartAt+1][0]) == 'Y') : getYesNo();
	if (enable_M_VScan)
		fFlags = fFlags & MAILFILTER_MC_M_VIRUSSCAN;

	
	
	if (!useParameters)
		printf("\n  ->User Redirection:");

	enable_M_URedir		 = useParameters ? (toupper(argv[parametersStartAt+2][0]) == 'Y') : getYesNo();
	if (enable_M_URedir)
		fFlags = fFlags & MAILFILTER_MC_M_USERREDIR;

	
	
	if (!useParameters)
		printf("\n  ->Mail Copy:");

	enable_M_MCopy		 = useParameters ? (toupper(argv[parametersStartAt+3][0]) == 'Y') : getYesNo();
	if (enable_M_MCopy)
		fFlags = fFlags & MAILFILTER_MC_M_MAILCOPY;

	
	
	if (!useParameters)
		printf("\n  ->Reserved4:");

	enable_M_Reserved4	 = useParameters ? (toupper(argv[parametersStartAt+4][0]) == 'Y') : getYesNo();
	if (enable_M_Reserved4)
		fFlags = fFlags & MAILFILTER_MC_M_BWTHCNTRL;

	
	if (!useParameters)
		printf("\n");

	// Build Key A (part1)

	fTemp = fFlags;

	cCheck1 = 0;
	for (p = 0;p<(15);p++)
	{
		cCheck1 = (keyA[p]-0x20) + (0x3F & cCheck1);
		keyA[p] = keyA[p] - 0x20;
		keyA[p] = keyA[p]<<2;
		keyA[p] = keyA[p] | (fTemp & 0x0003);
		keyA[p] = keyA[p] ^ (fOffset + p + 0x0F);
		
		fTemp = fTemp>>2;
	}
	keyA[15] = (0x40-cCheck1)<<2;
	keyA[15] = keyA[15] | (fTemp & 0x0003);
	keyA[15] = keyA[15] ^ (fOffset + 15 + 0x0F) ;	//	30 = 15+15  = 0x1E
	//keyA[16]=0;

	cCheck2=4;
	for (p = 0;p<(sizeof(keyA));p++)
		cCheck2 = (cCheck2) + keyA[p];
	

	
	// Build Key B
	
	time(&ltime);
	
	sprintf(tmpc,"%09d",ltime);

	strcpy((char*)keyB,tmpc+(strlen(tmpc)-9));
	tmpc[9]=0;
	
	sprintf((char*)keyB,"%s%c%c%c%x%c",tmpc,fCountry,fOpSys,fVersion,fFlags,cCheck2);
	

	for (p = 0;p<(sizeof(keyB));p++)
		keyB[p] = keyB[p] ^ (fOffset + 0xAA - p);
	
	
	i3 = 24;
//	printf("i3: %i\n",i3);

	iCheck3 = adler32(1L,NULL,0);
	iCheck3 = adler32(iCheck3,(unsigned char*)keyB,i3);
	keyA_chkSumB.sum = iCheck3;
	for (p = 0;p<4;p++)
		keyA[16+p] = keyA_chkSumB.str[p];

	keyA[20]=fOffset;
	keyA[21]=0;

	// Build End Key


	i3 = 0;
	cp = 0;
	bool key_A = false;
	unsigned int xp;
#define key_Max (48)
	for (p = 0;p<(key_Max);p+=2)
	{
		xp = p/2;
		if (key_A)
		{
			key[p] = keyA[xp];
			key[p+1] = keyB[xp];
		} else
		{
			key[p] = keyB[xp];
			key[p+1] = keyA[xp];
		}


		key_A = (!key_A);
	

	}

	cp = 0;
	i3 = 0;
	for (p = 0;p<48;p++)
	{
		kec[cp] = (key[p] & 0x3F) + 0x30; //| 0x40;

		i3++;
		cp++;
		if (i3 == 3)
		{
			fTemp = key[p] & 0xC0;
			fTemp = (fTemp>>2) | (key[p-1] & 0xC0);
			fTemp = (fTemp>>2) | (key[p-2] & 0xC0);
			fTemp = fTemp>>2;

			kec[cp] = fTemp  + 0x30; //| (0x40);
			i3 = 0;
			cp++;
		}
	}
	kec[cp]=0;

	for (p = 0;p<(strlen((char*)kec));p++)
	{
		switch (kec[p])
		{
		case ':':
			kec[p]='x';	break;
		case ';':
			kec[p]='r';	break;

		case '<':
			kec[p]='q';	break;
		case '=':
			kec[p]='t';	break;
		case '>':
			kec[p]='z';	break;
		case '?':
			kec[p]='v';	break;
		case '@':
			kec[p]='p';	break;
		case '\\':
			kec[p]='w';	break;

		case 0x5E:		// dach
			kec[p]='s';	break;
		case 0x5F:		// unterstrich
			kec[p]='y';	break;
		case 0x60:		// hochkomma
			kec[p]='u';	break;

		}
	}

	printf("\n");
	printf("    -----------------------MAILFILTER  CLEARINGHOUSE-----------------------\n");
	printf("    SERVER: %s - VSCAN: %d - ALIAS: %d - MCOPY: %d\n",in,enable_M_VScan,enable_M_URedir,enable_M_MCopy);
	printf("    KEY: %s\n",kec);
	printf("    -----------------------ELECTRONIC  KEY  DELIVERY-----------------------\n");
	printf("\n");

/*	cCheck1 = 0;
	for (p = 0;p<(sizeof(key)-1);p++)
	{
		cCheck1 = cCheck1+key[p];
		printf(" %2d %c=%02x \n",p,key[p],key[p]);
	}
	printf("%c %02d\n",cCheck1,cCheck1);

	x = (((int)keyA[1]) + ((int)keyA[3]))>>1;
	checkSum[0] = (char)(x);	// ((int)x<0) ? (int)x*(-1) : (int)x );
	
	cp = 0;
	for (p = 0;p<24;p++)
	{
		cp++;
		
		checkSum[cp] = (char)((keyA[p]) ^ (checkSum[0]));
		if (checkSum[cp] < 0x61)
			checkSum[cp] = '1' + ((0x61 - checkSum[cp])/10);
		if (checkSum[cp] > 0x7A)
			checkSum[cp] = '4' + abs((checkSum[cp] - 0x7A)/20);

		cp++;
		checkSum[cp] = (keyA[23-p])^(p|(p/3));
printf("%c[%d]",checkSum[cp],(p|(p/3)));
	}
	checkSum[cp+1]=0;
printf("\n");

	x=0;
	for (p = 0;p<(strlen(keyA)+1);p++)
		x = ( x + keyA[p] );
	checkSum[cp+1]=x;
	checkSum[cp+2]=0;
		if (checkSum[cp+1] < 0x61)
			checkSum[cp+1] = '1' + ((0x61 - checkSum[cp+1])/10);
		if (checkSum[cp+1] > 0x7A)
			checkSum[cp+1] = '4' + abs((checkSum[cp+1] - 0x7A)/20);

	cp++;
	x=0;
	for (p = 0;p<(cp+1);p++)
		x = ( x + checkSum[p] );
	checkSum[cp+1]=x;
	checkSum[cp+2]=0;
		if (checkSum[cp+1] < 0x61)
			checkSum[cp+1] = '1' + ((0x61 - checkSum[cp+1])/10);
		if (checkSum[cp+1] > 0x7A)
			checkSum[cp+1] = '4' + abs((checkSum[cp+1] - 0x7A)/20);

*/	/* // Small Decoder 
	out[0]=0;
	dx[1]=0;
	for (p = cp+1;p>1;p--)
	{
		p--;

		dx[0] = checkSum[p]^3;
		strcat(out,dx);
	} */
/*	printf("\n\n");

	unsigned long keyOut = 253;
	keyOut = adler32 ( keyOut , NULL , 0 );

	keyOut = adler32 ( keyOut , (unsigned char*)keyA , strlen(keyA)+1 );
//	printf(">%d<\n",keyOut);



	printf("    ---------------------MAILFILTER  CLEARINGHOUSE---------------------\n");
	printf("    KEY: %d-%s\n",keyOut,checkSum);
	printf("    VSCAN: %d - ALIAS: %d - MCOPY: %d - SERVER: %s\n",enable_M_VScan,false,false,in);
	printf("    ---------------------ELECTRONIC  KEY  DELIVERY---------------------\n");
	

*/
//    uLong adler;
//    const Bytef *buf;
//    uInt len;

	return 0;
}


