/***************************************************************************
 * tnef2txt
 *   A program to decode application/ms-tnef MIME attachments into text
 *   for those fortunate enough not to be running either a Microsoft 
 *   operating system or mailer.
 *
 * Brandon Long (blong@uiuc.edu), April 1997
 * 1.0 Version
 *   Supports most types, but doesn't decode properties.  Maybe some other 
 *   time.
 *
 * 1.1 Version (7/1/97)
 *   Supports saving of attAttachData to a file given by attAttachTitle
 *   start of property decoding support
 *
 * 1.2 Version (7/19/97)
 *   Some architectures don't like reading 16/32 bit data on unaligned 
 *   boundaries.  Fixed, losing efficiency, but this doesn't really
 *   need efficiency anyways.  (Still...)
 *   Also, the #pragma pack from the MSVC include file wasn't liked 
 *   by most Unix compilers, replaced with a GCCism.  This should work
 *   with GCC, but other compilers I don't know.
 *
 * 1.3 Version (7/22/97)
 *   Ok, take out the DTR over the stream, now uses read_16.
 *
 * NOTE: THIS SOFTWARE IS FOR YOUR PERSONAL GRATIFICATION ONLY.  I DON'T 
 * IMPLY IN ANY LEGAL SENSE THAT THIS SOFTWARE DOES ANYTHING OR THAT IT WILL
 * BE USEFULL IN ANY WAY.  But, you can send me fixes to it, I don't mind.
 ***************************************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include "config.h"
#include "tnef.h"
#include "mapidefs.h"
#include "mapitags.h"

#define VERSION "tnef2txt/1.3"

int Verbose = FALSE;
int SaveData = FALSE;

/* Some systems don't like to read unaligned data */
uint32 read_32(uint8 *tsp)
{
  uint8 a,b,c,d;
  uint32 ret;

  a = *tsp;
  b = *(tsp+1);
  c = *(tsp+2);
  d = *(tsp+3);

  ret = long_little_endian(a<<24 | b<<16 | c<<8 | d);

  return ret;
}

uint16 read_16(uint8 *tsp)
{
  uint8 a,b;
  uint16 ret;

  a = *tsp;
  b = *(tsp + 1);

  ret = little_endian(a<<8 | b);

  return ret;
}


char *attribute_string(uint32 attribute)
{
  switch (attribute) {
    case attNull: 
      return ("attNull");
      break;
    case attFrom: 
      return ("attFrom");
      break;
    case attSubject: 
      return ("attSubject");
      break;
    case attDateSent:
      return ("attDateSent");
      break;
    case attDateRecd: 
      return ("attDateRecd");
      break;
    case attMessageStatus: 
      return ("attMessageStatus");
      break;
    case attMessageClass: 
      return ("attMessageClass");
      break;
    case attMessageID: 
      return ("attMessageID");
      break;
    case attParentID: 
      return ("attParentID");
      break;
    case attConversationID: 
      return ("attConversationID");
      break;
    case attBody: 
      return ("attBody");
      break;
    case attPriority: 
      return ("attPriority");
      break;
    case attAttachData: 
      return ("attAttachData");
      break;
    case attAttachTitle: 
      return ("attAttachTitle");
      break;
    case attAttachMetaFile: 
      return ("attAttachMetaFile");
      break;
    case attAttachCreateDate: 
      return ("attAttachCreateDate");
      break;
    case attAttachModifyDate: 
      return ("attAttachModifyDate");
      break;
    case attDateModified: 
      return ("attDateModified");
      break;
    case attAttachTransportFilename: 
      return ("attAttachTransportFilename");
      break;
    case attAttachRenddata: 
      return ("attAttachRenddata");
      break;
    case attMAPIProps: 
      return ("attMAPIProps");
      break;
    case attRecipTable: 
      return ("attRecipTable");
      break;
    case attAttachment: 
      return ("attAttachment");
      break;
    case attTnefVersion: 
      return ("attTnefVersion");
      break;
    case attOemCodepage: 
      return ("attOemCodepage");
      break;
    case attOriginalMessageClass: 
      return ("attOriginalMessageClass");
      break;
    case attOwner: 
      return ("attOwner");
      break;
    case attSentFor: 
      return ("attSentFor");
      break;
    case attDelegate: 
      return ("attDelegate");
      break;
    case attDateStart: 
      return ("attDateStart");
      break;
    case attDateEnd: 
      return ("attDateEnd");
      break;
    case attAidOwner: 
      return ("attAidOwner");
      break;
    case attRequestRes: 
      return ("attRequestRes");
      break;
    default:
      return ("Unknown");
      break;
  }
  return ("Unknown");
}

char *type_string(uint16 type)
{
  switch (type) {
    case atpTriples:
      return ("atpTriples");
      break;
    case atpString:
      return ("atpString");
      break;
    case atpText:
      return ("atpText");
      break;
    case atpDate:
      return ("atpDate");
      break;
    case atpShort:
      return ("atpShort");
      break;
    case atpLong:
      return ("atpLong");
      break;
    case atpByte:
      return ("atpByte");
      break;
    case atpWord:
      return ("atpWord");
      break;
    case atpDword:
      return ("atpDword");
      break;
    default:
      return ("unknown");
      break;
  }
  return ("unknown");
}

char *prop_type_string(uint32 prop_tag)
{
  switch(prop_tag)
  {
    case PT_BINARY: 
      return ("PT_BINARY");
      break;
    case PT_STRING8:
      return ("PT_STRING8");
      break;
    case PT_UNICODE:
      return ("PT_UNICODE");
      break;
    case PT_OBJECT:
      return ("PT_OBJECT");
      break;
    case PT_I2:
      return ("PT_I2");
      break;
    case PT_LONG:
      return ("PT_LONG");
      break;
    case PT_R4:
      return ("PT_R4");
      break;
    case PT_DOUBLE:
      return ("PT_DOUBLE");
      break;
    case PT_CURRENCY:
      return ("PT_CURRENCY");
      break;
    case PT_APPTIME:
      return ("PT_APPTIME");
      break;
    case PT_ERROR:
      return ("PT_ERROR");
      break;
    case PT_BOOLEAN:
      return ("PT_BOOLEAN");
      break;
    case PT_I8:
      return ("PT_I8");
      break;
    case PT_SYSTIME:
      return ("PT_SYSTIME");
      break;
  }
  return ("unknown");
}

char *weekday[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", 
                    "Friday", "Saturday" };

char *date_string(uint8 *tsp)
{
  static char date_string[256] = "";
  DTR date;
  int bytes = 0;

  date.wYear = read_16(tsp);
  bytes += 2;
  date.wMonth = read_16(tsp+bytes);
  bytes += 2;
  date.wDay = read_16(tsp+bytes);
  bytes += 2;
  date.wHour = read_16(tsp+bytes);
  bytes += 2;
  date.wMinute = read_16(tsp+bytes);
  bytes += 2;
  date.wSecond = read_16(tsp+bytes);
  bytes += 2;
  date.wDayOfWeek = read_16(tsp+bytes);
  bytes += 2;

  sprintf(date_string, "%s, %d/%d/%d ", weekday[date.wDayOfWeek], 
      date.wMonth, date.wDay, date.wYear);
  return date_string;
}

char *make_string(uint8 *tsp, int size)
{
  static char s[256] = "";
  int len = (size>sizeof(s)-1) ? sizeof(s)-1 : size;

  strncpy(s,tsp, len);
  s[len] = '\0';
  return s;
}

char *priority_string(uint16 pri)
{
  switch(pri) {
    case prioLow: 
      return "(Low)";
    case prioNorm:
      return "(Normal)";
    case prioHigh:
      return "(High)";
    default:
      return "";
  }
  return  "";
}

char *status_string(uint8 status)
{
  static char s[256] = "";

  s[0] = '\0';
  if (status & fmsModified)
    sprintf(s,"%s%s ",s,"Modified");
  if (status & fmsLocal)
    sprintf(s,"%s%s ",s,"Local");
  if (status & fmsSubmitted)
    sprintf(s,"%s%s ",s,"Submitted");
  if (status & fmsRead)
    sprintf(s,"%s%s ",s,"Read");
  if (status & fmsHasAttach)
    sprintf(s,"%s%s ",s,"HasAttach");
  return s;
}

int handle_props(uint8 *tsp) 
{
  int bytes = 0;
  uint32 num_props = 0;
  uint32 x = 0;


  num_props = read_32(tsp);
  bytes += sizeof(num_props);

  printf("\tNumber of Properties: %ld\n", num_props);
  while (x < num_props) 
  {
    uint32 prop_tag;
    uint32 num;
    char filename[256];
    static int file_num = 0;

    prop_tag = read_32(tsp+bytes);
    bytes += sizeof(prop_tag);
    if (Verbose)
      printf("\tProperty: %08lx (%s)\n", prop_tag, 
	  prop_type_string(prop_tag & PROP_TYPE_MASK));
    switch (prop_tag & PROP_TYPE_MASK) 
    {
      case PT_BINARY: 
	num = read_32(tsp+bytes);
	bytes += sizeof(num);
	num = read_32(tsp+bytes);
	bytes += sizeof(num);
	if (prop_tag == PR_RTF_COMPRESSED)
	{
	  sprintf (filename, "tnef_%d_%0lx.rtf", file_num, num);
	  file_num++;
	  if (SaveData) {
	    printf ("\tSaving Compressed RTF to %s\n", filename);
	    save_attach_data(filename, tsp+bytes, num);
	  } else {
	    printf ("\tContains Compressed RTF MAPI property\n");
	  }
	}
	/* num + PAD */
	bytes += num + ((num % 4) ? (4 - num%4) : 0);
	break;
      case PT_STRING8:
	switch (prop_tag)
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
	}
	num = read_32(tsp+bytes);
	bytes += sizeof(num);
	num = read_32(tsp+bytes);
	bytes += sizeof(num);
	printf ("%s\n", make_string(tsp+bytes,num));
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

}

int save_attach_data(char *title, uint8 *tsp, uint32 size)
{
  FILE *out;
  char filename[256];

  strcpy(filename, title);
  out = fopen(filename, "w");
  if (out == NULL) {
    fprintf(stderr, "Error openning file %s for writing\n", filename);
    return -1;
  }
  fwrite(tsp, sizeof(uint8), size, out);
  fclose(out);
  return 0;
}

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

int read_attribute(uint8 *tsp)
{
  int bytes = 0, header = 0;
  uint32 attribute;
  uint8 component = 0;
  uint32 size = 0;
  uint16 checksum = 0;
  static char attach_title[256] = "default.dat";
  uint8 *ptr;

  component = *tsp;
  bytes += sizeof(uint8);

  attribute = read_32(tsp+bytes);
  bytes += sizeof(attribute);
  size = read_32(tsp+bytes);
  bytes += sizeof(size);
  header = bytes;
  bytes += size;
  checksum = read_16(tsp+bytes);
  bytes += sizeof(checksum);

  if (Verbose) {
    printf("Attribute: %s, (%08lx)\n",attribute_string(attribute),attribute);
    if(component == LVL_MESSAGE)
      printf("\tComponent:\tMessage\n");
    else
      printf("\tComponent:\tAttachment\n");
    printf("\tType:\t\t%s\n",type_string(ATT_TYPE(attribute)));
    printf("\tSize:\t\t%d\n",size);
    printf("\tChecksum:\t%04x\n",checksum);
  }

  switch (attribute) {
    case attNull: 
      default_handler(attribute, tsp+header, size);
      break;
    case attFrom: 
      default_handler(attribute, tsp+header, size);
      break;
    case attSubject: 
      printf("Subject: %s\n",make_string(tsp+header,size));
      break;
    case attDateSent:
      printf("Date Sent: %s\n",date_string(tsp+header));
      break;
    case attDateRecd: 
      printf("Date Recieved: %s\n",date_string(tsp+header));
      break;
    case attMessageStatus: 
      printf("Message Status: %s\n",status_string(*(uint8 *)(tsp+header)));
      break;
    case attMessageClass: 
      printf("Message Class: %s\n",make_string(tsp+header,size));
      break;
    case attMessageID: 
      printf("Message Id: %s\n",make_string(tsp+header,size));
      break;
    case attParentID: 
      printf("Parent Id: %s\n",make_string(tsp+header,size));
      break;
    case attConversationID: 
      printf("Conversation Id: %s\n",make_string(tsp+header,size));
      break;
    case attBody: 
      default_handler(attribute, tsp+header, size);
      break;
    case attPriority: 
      printf("Priority: %d %s\n", read_16(tsp+header),
	  priority_string(read_16(tsp+header)));
      break;
    case attAttachData: 
      if (SaveData) {
	if (!save_attach_data(attach_title, tsp+header,size))
	  printf("Attachment saved to file %s\n", attach_title);
	 else
	   printf("Failure saving attachment to file %s\n", attach_title);
      } else {
	printf("TNEF Contains attached file %s\n", attach_title);
      }
      break;
    case attAttachTitle: 
      strcpy(attach_title, make_string(tsp+header,size));
      printf("Attachment Title: %s\n",attach_title);
      break;
    case attAttachMetaFile: 
      default_handler(attribute, tsp+header, size);
      break;
    case attAttachCreateDate: 
      printf("Attachment Date Created: %s\n",date_string(tsp+header));
      break;
    case attAttachModifyDate: 
      printf("Attachment Date Modified: %s\n",date_string(tsp+header));
      break;
    case attDateModified: 
      printf("Date Modified: %s\n",date_string(tsp+header));
      break;
    case attAttachTransportFilename: 
      default_handler(attribute, tsp+header, size);
      break;
    case attAttachRenddata: 
      default_handler(attribute, tsp+header, size);
      break;
    case attMAPIProps: 
      printf("MAPI Properties\n");
      handle_props(tsp+header);
      break;
    case attRecipTable: 
      default_handler(attribute, tsp+header, size);
      break;
    case attAttachment: 
      default_handler(attribute, tsp+header, size);
      break;
    case attTnefVersion:
      {
	uint32 version;
	version = read_32(tsp+header);
	printf("TNEF Version: %d.%d\n",((version & 0xFFFF0000)>>16),
	    (version & 0x0000FFFF));
      }
      break;
    case attOemCodepage: 
      default_handler(attribute, tsp+header, size);
      break;
    case attOriginalMessageClass: 
      printf("Original Message Class: %s\n",make_string(tsp+header,size));
      break;
    case attOwner: 
      default_handler(attribute, tsp+header, size);
      break;
    case attSentFor: 
      default_handler(attribute, tsp+header, size);
      break;
    case attDelegate: 
      default_handler(attribute, tsp+header, size);
      break;
    case attDateStart: 
      printf("Start Date: %s\n",date_string(tsp+header));
      break;
    case attDateEnd: 
      printf("End Date: %s\n",date_string(tsp+header));
      break;
    case attAidOwner: 
      default_handler(attribute, tsp+header, size);
      break;
    case attRequestRes: 
      default_handler(attribute, tsp+header, size);
      break;
    default:
      default_handler(attribute, tsp+header, size);
      break;
  }
  return bytes;
}

int decode_tnef(uint8 *tnef_stream, int size)
{
  uint8 *tsp;

  tsp = tnef_stream;

  if (TNEF_SIGNATURE == read_32(tsp)) {
    printf("Good TNEF Signature\n");
  } else {
    printf("Bad TNEF Signature, Expecting: %lx  Got: %lx\n",TNEF_SIGNATURE,
	read_32(tsp));
  }
  tsp += sizeof(TNEF_SIGNATURE);

  printf("TNEF Attach Key: %x\n",read_16(tsp));
  tsp += sizeof(uint16); 

  while ((tsp - tnef_stream) < size) {
    tsp += read_attribute(tsp);
  }
  return 0;
}

void usage(char *argv0) 
{
  fprintf(stderr, "\n%s by Brandon Long\n", VERSION);
  fprintf(stderr, "Translation Neutral Encapsulation Format (TNEF) decoder\n"); 
  fprintf(stderr, "\nUsage: %s [-v][-d][-h][-s] <filename>\n", argv0);
  fprintf(stderr, "\t-v: Verbose output\n");
  fprintf(stderr, "\t-d: Dump hex contents\n");
  fprintf(stderr, "\t-h: This information\n");
  fprintf(stderr, "\t-s: Save any attached files in the TNEF stream to disk\n");
  fprintf(stderr, "\n");
}

int main(int argc, char *argv[])
{
  FILE *fp;
  struct stat sb;
  int size, nread;
  uint8 *tnef_stream;
  int dump = 0;
  char *filename;
  int x;

  if (argc < 2 || argc > 4) {
    usage(argv[0]);
    exit(-1);
  }

  x = 1;
  while (x < argc && argv[x]) {
    if (!strcmp(argv[x], "-v")) {
      Verbose = TRUE;
    } 
    else if (!strcmp(argv[x], "-s")) {
      SaveData = TRUE;
    }
    else if (!strcmp(argv[x], "-h")) {
      usage(argv[0]);
      exit(0);
    } 
    else if (!strcmp(argv[x], "-d")) {
      dump = TRUE;
    } else {
      filename = argv[x];
    }
    x++;
  }
  
  if (stat(filename,&sb) == -1) {
    fprintf(stderr, "Error stating file %s\n", filename);
    perror("stat");
    usage(argv[0]);
    exit(-1);
  }

  size = sb.st_size;

  tnef_stream = (uint8 *)malloc(size);

  if (tnef_stream == NULL) {
    fprintf(stderr, "Error allocating %d bytes for loading file\n", size);
    usage(argv[0]);
    exit(-1);
  }

  if ((fp = fopen(filename,"r")) == NULL) {
    fprintf(stderr, "Error openning file %s\n", filename);
    perror("fopen");
    usage(argv[0]);
    exit(-1);
  }

  nread = fread(tnef_stream, sizeof(uint8), size, fp);
  if (nread < size) {
    fprintf(stderr, "Error reading stream from file %s\n",filename);
    perror("fread");
    usage(argv[0]);
    exit(-1);
  }
  fclose(fp);

  if (dump) {
    int x;
    for (x=0 ; x<size ; x++) {
      if (x%32 == 0) putchar('\n');
      if (x%4 == 0) putchar(' ');
      printf("%02x",*(tnef_stream+x));
    }
    putchar('\n');
  }
  decode_tnef(tnef_stream,size);

  return 0;
}
