/*
 *
 *   MFLic.cpp
 *
 *   MailFilter Licensing (C++). For NLM Use.
 *
 */
	  

// Enable this one for debugging
//#define _TRACE

//#define N_PLAT_NLM

//#include <stat.h>

//#define WIN32

#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#ifndef WIN32
/* NetWare */
#include <unistd.h>
#ifndef __NOVELL_LIBC__
/* CLib workarounds */
#define N_PLAT_NLM
extern "C" { void  delay( unsigned milliseconds ); }
#endif
#else
/* Win32 */
#include <io.h>
#endif

#ifdef __NOVELL_LIBC__
extern "C" { void MF_DisplayCriticalError(const char* format, ...); }
#define ConsolePrintf MF_DisplayCriticalError
#endif

	#undef _MAX_DRIVE
	#undef _MAX_DIR
	#undef _MAX_FNAME
	#undef _MAX_EXT
	#undef _MAX_NAME
	#undef MAX_PATH	  
	#define _MAX_PATH    255 /* maximum length of full pathname */
	#define _MAX_SERVER  48  /* maximum length of server name */
	#define _MAX_VOLUME  16  /* maximum length of volume component */
	#define _MAX_DRIVE   3   /* maximum length of drive component */
	#define _MAX_DIR     255 /* maximum length of path component */
	#define _MAX_FNAME   9   /* maximum length of file name component */
	#define _MAX_EXT     5   /* maximum length of extension component */
	#define _MAX_NAME    13  /* maximum length of file name */
	#define MAX_PATH _MAX_SERVER+_MAX_VOLUME+_MAX_PATH+10

extern char* MFT_Local_ServerName;
char MFL_LicenseKey[MAX_PATH];
int MFL_Certified = -1;

static unsigned char keyA[25];
static unsigned char keyB[25];
static unsigned int  fFlags	= 0;
static unsigned char key[100];
static char kec[100];
static int fOffset = 0;

#include "MFLic.h"

//
//   Adler32 Checksuming ... see adler32.c for information, copyright and license.
// 
#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552	/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);
unsigned long adler32 (unsigned long adler, const unsigned char *buf, unsigned int len)
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (len == 0) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
	    buf += 16;
            k -= 16;
        }
        if (k != 0) do {
            s1 += *buf++;
	    s2 += s1;
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}



int MFL_Stage0(int prevCert);
int MFL_Stage1(int prevCert);
int MFL_Stage2(int prevCert);
int MFL_Stage3(int prevCert);
int MFL_Stage4(int prevCert);
int MFL_Stage5(int prevCert);
int MFL_Stage6(int prevCert);
int MFL_Stage7(int prevCert);
int MFL_Stage8(int prevCert);
int MFL_Stage9(int prevCert);

//
//   MailFilter Licensing: Init()
//
int MFL_Init(int runLevel)
{
	int run = 255;
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("\nMFD: Lic RunLevel: %d\n",runLevel);
#endif
	switch (runLevel)
	{
	case 0:
		run = MFL_Stage0(-1);
		break;
	case 1:
		run = -1;
//		run = MFL_Stage1(MFL_Certified);
		break;
	case 2:
		run = MFL_Stage2(MFL_Certified);
		break;
	case 3:
		run = -1;
//		run = MFL_Stage3(MFL_Certified);
		break;
	case 4:
		run = MFL_Stage4(MFL_Certified);
		break;
	case 5:
		run = MFL_Stage5(MFL_Certified);
		break;
	case 6:
		run = -1;
//		run = MFL_Stage6(MFL_Certified);
		break;
	case 7:
		run = -1;
		run = MFL_Stage7(MFL_Certified);
		break;
	case 8:
		run = -1;
		run = MFL_Stage8(MFL_Certified);
		break;
	case 9:
		run = MFL_Stage9(MFL_Certified);
		break;
	default:
		run = -1;
		break;
	}

#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("\nMFD: ***Stage %d reported %d ***\n",runLevel,run);
#endif

	if (run == -1)
		return MFL_Certified;

	if (run == 255)
		return 0;

	return run;
}

int MFL_Stage0(int prevCert)
{
	if (prevCert != -1)
	{
#ifndef WIN32
	delay(4);
#endif
		return 0;
	}

	int rc = -1;
	unsigned int p = 0;
	
	unsigned int nb = 65;	//, br;
	kec[0]=0;
	
	strncpy(kec,MFL_LicenseKey,65);

	kec[65]=0;
	if (kec[0] == 0)
		rc = 2;

	if (rc > -1)
	{	MFL_Certified = rc;
		return rc;
	}

	// Replace characters
	for (p = 0;p<(65);p++)
	{
		switch (kec[p])
		{
		case 'x':
			kec[p]=':';	break;
		case 'r':
			kec[p]=';';	break;

		case 'q':
			kec[p]='<';	break;
		case 't':
			kec[p]='=';	break;
		case 'z':
			kec[p]='>';	break;
		case 'v':
			kec[p]='?';	break;
		case 'p':
			kec[p]='@';	break;
		case 'w':
			kec[p]='\\';	break;

		case 's':		// dach
			kec[p]=0x5E;	break;
		case 'y':		// unterstrich
			kec[p]=0x5F;	break;
		case 'u':		// hochkomma
			kec[p]=0x60;	break;

		}
	}

	MFL_Certified = prevCert;
	return MFL_Certified;
}

int MFL_Stage2(int prevCert)
{
	if (prevCert != -1)
	{
#ifndef WIN32
	delay(4);
#endif
		return prevCert;
	}

	unsigned int p = 0;
	unsigned int cp = 0;
	unsigned int i3 = 0;

	unsigned int  fTemp		= 0;

	// 6bit Decode
	cp = 0;
	i3 = 0;
	for (p = 0;p<65;p++)
	{
		key[cp] = (unsigned char)( (kec[p]-0x30)/* & 0x3F*/);

		cp++;
		i3++;
		if (i3 == 3)
		{
			p++;
			fTemp = (unsigned int)(kec[p] - 0x30);

			fTemp = fTemp<<2;
			key[cp-3] = (unsigned char)(key[cp-3] | (fTemp & 0xC0));
			fTemp = fTemp<<2;
			key[cp-2] = (unsigned char)(key[cp-2] | (fTemp & 0xC0));
			fTemp = fTemp<<2;
			key[cp-1] = (unsigned char)(key[cp-1] | (fTemp & 0xC0));

			i3 = 0;
		}
	}
	key[cp]=0;
	fTemp = 0;
	memset(kec,0,99);
	
	MFL_Certified = prevCert;
	return MFL_Certified;
}


int MFL_Stage4(int prevCert)
{
	if (prevCert != -1)
	{
#ifndef WIN32
	delay(4);
#endif
		return prevCert;
	}

	unsigned int p;
	bool key_A = false;
	for (p = 0;p<(50);p+=2)
	{
		if (key_A)
		{
			keyA[p/2] = key[p];
			keyB[p/2] = key[p+1];
		} else
		{
			keyB[p/2] = key[p];
			keyA[p/2] = key[p+1];
		}
		key_A = (!key_A);
	}
	keyA[(p/2)-1]=0;
	keyB[(p/2)-1]=0;


	MFL_Certified = prevCert;
	return MFL_Certified;
}

int MFL_Stage5(int prevCert)
{	
	if (prevCert != -1)
	{
#ifndef WIN32
	delay(4);
#endif
		return prevCert;
	}

	unsigned long iCheck3   = 0;

	unsigned int p;
	unsigned int i3;

	fOffset = keyA[20] & 0x0F;
	
	
	// Check Version OK
	if ((keyB[11]^(fOffset + 0xAA - 11)) != 1)
	{	MFL_Certified = 0;
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("MailFilter Release - Stage 5 (KEY B): Found in key: (%d)%d(%d)\n",(keyB[10]^(fOffset + 0xAA - 10)),(keyB[11]^(fOffset + 0xAA - 11)),(keyB[12]^(fOffset + 0xAA - 12)));
#endif
		return MFL_Certified;
	}


	// Yes, Calculate Checksum for Key B
	i3 = 24;
	for (p = 0;p<4;p++)
		keyA_chkSumB.str[p] = (char)(keyA[16+p]);

	iCheck3 = adler32(1L,NULL,0);
	iCheck3 = adler32(iCheck3,(unsigned char*)keyB,i3);


	// Check Cheksum Match?
	if ( iCheck3 != keyA_chkSumB.sum )
	{
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("CheckSum - Stage 5 (KEY B)\n-> %d -> %d\n",iCheck3,keyA_chkSumB.sum);
#endif
		MFL_Certified = 0;
		return MFL_Certified;
	}
	
	// Check OS OK
	if ((keyB[10]^(fOffset + 0xAA - 10)) != MAILFILTER_OPSYS)
	{	MFL_Certified = 0;
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("OpSys Code - Stage 5 (KEY B)\n");
#endif
		return MFL_Certified;
	}

	// Yep.
	return MFL_Certified;

}

int MFL_Stage7(int prevCert)
{
	if (prevCert != -1)
	{
#ifndef WIN32
	delay(4);
#endif
		return prevCert;
	}
	unsigned int p;

	keyB[23]=0;

	// If we're already here, decode Key B
	for (p = 0;p<(sizeof(keyB)-1);p++)
	{	keyB[p] = (unsigned char)(keyB[p] ^ (fOffset + 0xAA - p));
//		printf("%02x ",keyB[p]);
	}
//	printf("\n");

	// Decode Key A
	for (p = 0;p<(sizeof(keyA)-1);p++)
	{	keyA[p] = (unsigned char)(keyA[p] ^ (fOffset + p + 0x0F));
//		printf("%02x ",keyA[p]);
	}


	return MFL_Certified;
}

int MFL_Stage8(int prevCert)
{
	if (prevCert != -1)
	{
#ifndef WIN32
	delay(4);
#endif
		return prevCert;
	}
	unsigned int fTemp;
	unsigned int p;

	fTemp = 0;
	for (p = 15;p!=-1;p--)
	{
		fTemp = fTemp<<2;
		fTemp = fTemp | (keyA[p] & 0x0003);
		keyA[p] = (unsigned char)(keyA[p]>>2);
		keyA[p] = (unsigned char)(keyA[p] + 0x20);
	}
	keyA[16] = 0; 

	fFlags = fTemp;
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("MFD: Flags: >%x<\n",fFlags);
ConsolePrintf("MFD: SERVER: >%s<\n",keyA);
#endif
	

	return MFL_Certified;
}

int MFL_Stage9(int prevCert)
{
	if (prevCert != -1)
	{
#ifndef WIN32
	delay(4);
#endif
		return prevCert;
	}

	unsigned int p;
	unsigned char cCheck1	= 0;
//	unsigned char cCheck2	= 0;
	unsigned char serverNameCmp[17];

	cCheck1 = 0;
	for (p = 0;p<16;p++)
		cCheck1 = (unsigned char)((keyA[p]-0x20) + (0x3F & cCheck1));

#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("MFD: check1: %x\n",cCheck1);
ConsolePrintf("MFD: KeyA: %s\n",keyA);
#endif
	cCheck1 = (unsigned char)(cCheck1 & 0x3F);

	if (cCheck1 != 0)
	{
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("CheckSum 1 - Stage 9 (KEY A) (%x)\n",cCheck1);
#endif
		MFL_Certified = 0;
		return MFL_Certified;
	}

	strcpy((char*)serverNameCmp,(char*)MFT_Local_ServerName);
	serverNameCmp[15]=0;
	strcat((char*)serverNameCmp,"@");
	if (	memcmp ( (char*)serverNameCmp , (char*)keyA , strlen((char*)serverNameCmp) )	!= 0	)
	{
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("ServerName - Stage 9 (KEY A)\n");
#endif
		MFL_Certified = 0;
		return MFL_Certified;
	}



//	sprintf((char*)keyB,"%08d%c%c%c%x%c",ltime,fCountry,fOpSys,fVersion,fFlags,cCheck2);
#if (defined( _TRACE ) && (!defined( WIN32 )))
ConsolePrintf("Country Code: %d\nOperating System: %d\nMailFilter Release: %d\n",keyB[9],keyB[10],keyB[11]);
#endif

	MFL_Certified = 1;
	return MFL_Certified;
}


bool MFL_GetFlag(int flag)
{
//	flag = flag;	/* compiler warning unless implemented */

	// Enable all for the eval ... the funx know who to disable theirselves ;) ...
	if (MFL_Certified == 2)
		return true;
	else
	{
		return ((fFlags | flag) == flag);
	}
}

#ifdef WIN32
void main()
{
	int rL;
	for (rL = 0;rL<10;rL++)
		MFL_Init(rL);
	
	//printf("\n*** MFL_Init(%i) returned %d ***\n",rL,MFL_Init(rL));
		
	printf("%x\n",fFlags);
//	printf("%x - %x - %x\n",,fFlags | MAILFILTER_MC_M_USERREDIR,fFlags | MAILFILTER_MC_M_MAILCOPY);
}
#endif
