/*
**	tnef-lib.h
*/

#ifdef __cplusplus
extern "C" {
#endif

#define LIBTNEF_TYPE_NOMOREPARTS    0
#define LIBTNEF_TYPE_ATTACHMENTNAME 1
#define LIBTNEF_TYPE_ATTACHMENTDATA 2
#define LIBTNEF_TYPE_MAPIPROPERTIES 3
#define LIBTNEF_TYPE_USEMAPITYPE    4

struct LibTNEF_ClientData {
	long STRUCTSIZE;
	unsigned char* tsp;
	unsigned char* tnef_stream;
	int size;
	int nread;
	char szAttachmentName[256];
	unsigned char* lAttachmentStart;
	long lAttachmentSize;
};

extern int LibTNEF_SaveAttachmentData(struct LibTNEF_ClientData* data, char* szDestinationFilename);
extern int LibTNEF_Decode(char* szFilename, struct LibTNEF_ClientData* data /* out */);
extern int LibTNEF_Free(struct LibTNEF_ClientData* data /* in/out */);
extern int LibTNEF_ReadNextAttribute(struct LibTNEF_ClientData* data /* in/out */);

#ifdef __cplusplus
}
#endif

/* eof */