/*
 * tnef-lib.c
 * 
 * based on tnef2txt, by Brandon Long.
 *
 * tnef2txt had this license header attached, and the same license applies
 * to tnef-lib:
 *
 * NOTE: THIS SOFTWARE IS FOR YOUR PERSONAL GRATIFICATION ONLY.  I DON'T 
 * IMPLY IN ANY LEGAL SENSE THAT THIS SOFTWARE DOES ANYTHING OR THAT IT WILL
 * BE USEFULL IN ANY WAY.  But, you can send me fixes to it, I don't mind.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include "config.h"
#include "tnef.h"
#include "mapidefs.h"
#include "tnef-lib.h"
#include "mapitags.h"

//#define VERSION "tnef-lib 0.1 (based on tnef2txt/1.3)"

/* Some systems don't like to read unaligned data */
static uint32 read_32(uint8 *tsp, uint8* streamsize)
{
  uint8 a,b,c,d;
  uint32 ret;

  if ( (tsp+3) > streamsize ) { return 0; }

  a = *tsp; 
  b = *(tsp+1);
  c = *(tsp+2);
  d = *(tsp+3);

  ret = long_little_endian(a<<24 | b<<16 | c<<8 | d);

  return ret;
}

static uint16 read_16(uint8 *tsp, uint8* streamsize)
{
  uint8 a,b;
  uint16 ret;

  if ( (tsp+1) > streamsize ) { return 0; }

  a = *tsp;
  b = *(tsp + 1);

  ret = little_endian(a<<8 | b);

  return ret;
}

static char *make_string(uint8 *tsp, int size)
{
  static char s[256] = "";
  int len = (size>sizeof(s)-1) ? sizeof(s)-1 : size;

  strncpy(s,tsp, len);
  s[len] = '\0';
  return s;
}

static int handle_props(uint8 *tsp, uint8* streamsize) 
{
  int bytes = 0;
  uint32 num_props = 0;
  uint32 x = 0;


  num_props = read_32(tsp,streamsize);
  bytes += sizeof(num_props);

  while (x < num_props) 
  {
    uint32 prop_tag;
    uint32 num;
//    char filename[256];
    static int file_num = 0;

    prop_tag = read_32(tsp+bytes,streamsize);
    bytes += sizeof(prop_tag);
    switch (prop_tag & PROP_TYPE_MASK) 
    {
      case PT_BINARY: 
	num = read_32(tsp+bytes,streamsize);
	bytes += sizeof(num);
	num = read_32(tsp+bytes,streamsize);
	bytes += sizeof(num);
	if (prop_tag == PR_RTF_COMPRESSED)
	{
/*	  sprintf (filename, "tnef_%d_%0lx.rtf", file_num, num);
	  file_num++;
	  if (SaveData) {
	    printf ("\tSaving Compressed RTF to %s\n", filename);
	    save_attach_data(filename, tsp+bytes, num);
	  } else {
	    printf ("\tContains Compressed RTF MAPI property\n");
	  }
*/	}
	/* num + PAD */
	bytes += num + ((num % 4) ? (4 - num%4) : 0);
	break;
      case PT_STRING8:
/*	switch (prop_tag)
	{
	  case PR_CONVERSATION_TOPIC:
	    printf ("\tConversation Topic: ");
	    break;
	  case PR_SENDER_ADDRTYPE:
	    printf ("\tSender Address Type: ");
	    break;
	  case PR_SENDER_EMAIL_ADDRESS:
	    printf ("\tSender Email Address: ");
	    break;
	  case PR_RTF_SYNC_BODY_TAG:
	    printf ("\tRTF Sync body tag: ");
	    break;
	  case PR_SUBJECT_PREFIX:
	    printf ("\tSubject Prefix: ");
	    break;
	  default: 
	    printf ("\tUnknown String: ");
	    break;
	}*/
	num = read_32(tsp+bytes,streamsize);
	bytes += sizeof(num);
	num = read_32(tsp+bytes,streamsize);
	bytes += sizeof(num);
//	printf ("%s\n", make_string(tsp+bytes,num));
	bytes += num + ((num % 4) ? (4 - num%4) : 0);
	break;
      case PT_UNICODE:
      case PT_OBJECT:
	break;
      case PT_I2:
	bytes += 2;
	break;
      case PT_LONG:
	bytes += 4;
	break;
      case PT_R4:
	bytes += 4;
	break;
      case PT_DOUBLE:
	bytes += 8;
	break;
      case PT_CURRENCY:
      case PT_APPTIME:
      case PT_ERROR:
	bytes += 4;
	break;
      case PT_BOOLEAN:
	bytes += 4;
	break;
      case PT_I8:
	bytes += 8;
      case PT_SYSTIME:
	bytes += 8;
	break;
    }
    x++;
  }

  return 0;
}
//__declspec(export) 
LIB_EXPORT int LibTNEF_SaveAttachmentData(struct LibTNEF_ClientData* data, char* szDestinationFilename)
{
  FILE *out;

  out = fopen( ( szDestinationFilename == NULL ? data->szAttachmentName : szDestinationFilename ), "w");
  if (out == NULL) {
//    fprintf(stderr, "Error openning file %s for writing\n", filename);
    return -1;
  }
  fwrite(data->lAttachmentStart, sizeof(uint8), data->lAttachmentSize, out);
  fclose(out);
  return 0;
}
/*
int default_handler(uint32 attribute, uint8 *tsp, uint32 size)
{

   uint16 type = ATT_TYPE(attribute);

  switch (type) {
    case atpTriples:
      break;
    case atpString:
    case atpText:
      printf("Attribute %s: ", attribute_string(attribute));
      printf("%s\n",make_string(tsp,size));
      break;
    case atpDate:
      printf("Attribute %s: ", attribute_string(attribute));
      printf("%s\n",date_string(tsp));
      break;
    case atpShort:
      break;
    case atpLong:
      break;
    case atpByte:
      break;
    case atpWord:
      break;
    case atpDword:
      break;
    default:
      printf("Attribute %s: ", attribute_string(attribute));
      printf("Unknown type\n");
      break;
  }
  return 0;
}
*/
//__declspec(export) 
LIB_EXPORT int LibTNEF_Decode(const char* szFilename, struct LibTNEF_ClientData* data /* out */)
{
  FILE *fp;
  struct stat sb;
  int dump = 0;

  if (data->STRUCTSIZE != sizeof(struct LibTNEF_ClientData))
  	return -1;	/* invalid struct */

  if (stat(szFilename,&sb) == -1) {
    return -10;
  }

  data->size = sb.st_size;

  data->tnef_stream = (uint8 *)malloc(data->size);

  if (data->tnef_stream == NULL) {
    return -12;
  }

  if ((fp = fopen(szFilename,"rb")) == NULL) {
    return -13;
  }
  
  fseek(fp,SEEK_SET,0);
  
  data->nread = fread(data->tnef_stream, sizeof(uint8), data->size, fp);
  if (data->nread < (data->size-1)) {
    return -14;
  }
  data->size = data->nread;
  fclose(fp);

  data->tsp = data->tnef_stream;

  if (TNEF_SIGNATURE == read_32(data->tsp,data->tnef_stream+data->size))
  {
//    printf("Good TNEF Signature\n");
  }
   else 
  {
//    printf("Bad TNEF Signature, Expecting: %lx  Got: %lx\n",TNEF_SIGNATURE, read_32(data->tsp,data->tnef_stream+data->size));
	return -20;
  }
  data->tsp += sizeof(TNEF_SIGNATURE);

  read_16(data->tsp,data->tnef_stream+data->size);
//  printf("TNEF Attach Key: %x\n",);
  data->tsp += sizeof(uint16); 

  return 0;
}

LIB_EXPORT int LibTNEF_Free(struct LibTNEF_ClientData* data /* in/out */)
{
	if (data->tnef_stream != NULL)	{ free(data->tnef_stream); }
	data->STRUCTSIZE = NULL;		/* invalidate */
	data->tsp = NULL;
	data->size = NULL;
	data->nread = NULL;
	return 0;
}
 
LIB_EXPORT int LibTNEF_ReadNextAttribute(struct LibTNEF_ClientData* data /* in/out */)
{
	int bytes = 0, header = 0;
	uint32 attribute;
	uint8 component = 0;
	uint32 size = 0;
	uint16 checksum = 0;
	int rc = 0;
//	uint8 *ptr;
//	static char attach_title[256] = "default.dat";

	if ( 
		( 
			(data->tsp) - (data->tnef_stream)
		 )
		< 
		(data->size) 
	)
	{

		component = *data->tsp;
		bytes += sizeof(uint8);

		attribute = read_32(data->tsp+bytes,data->tnef_stream+data->size);
		bytes += sizeof(attribute);
		size = read_32(data->tsp+bytes,data->tnef_stream+data->size);
		bytes += sizeof(size);
		header = bytes;
		bytes += (int)size;
		checksum = read_16(data->tsp+bytes,data->tnef_stream+data->size);
		bytes += sizeof(checksum);

		if (component != LVL_MESSAGE)
		{
//			printf("\tSize:\t\t%d\n",size);
		}

		switch (attribute)
		{
			case attAttachTitle: 
				strcpy(data->szAttachmentName, make_string((data->tsp)+header,size));
				rc = LIBTNEF_TYPE_ATTACHMENTNAME;
				break;
			case attAttachData: 
/*				if (SaveData) {
				if (!save_attach_data(attach_title, tsp+header,size))
				printf("Attachment saved to file %s\n", attach_title);
				else
				printf("Failure saving attachment to file %s\n", attach_title);
				} else {
				printf("TNEF Contains attached file %s\n", attach_title);
				}*/
				data->lAttachmentStart = data->tsp+header;
				data->lAttachmentSize = (long)size;
				rc = LIBTNEF_TYPE_ATTACHMENTDATA;
				break;
			case attMAPIProps: 
				handle_props(data->tsp+header,data->tnef_stream+data->size);
				rc = LIBTNEF_TYPE_MAPIPROPERTIES;
				break;
			default:
//				default_handler(attribute, tsp+header, size);
				rc = LIBTNEF_TYPE_USEMAPITYPE;
				break;
		}

		data->tsp += bytes;
		return rc;
		
	} else {
		return LIBTNEF_TYPE_NOMOREPARTS;
	}
}
