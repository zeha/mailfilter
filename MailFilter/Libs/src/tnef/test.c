/*
** test.c
 */
 
#include "tnef-lib.h"


int main()
{
	struct LibTNEF_ClientData data;
	int type = 0;
	
	data.STRUCTSIZE = sizeof(struct LibTNEF_ClientData);
	if (LibTNEF_Decode("ms-tnef",&data) < 0)
		return -1;

	type = LibTNEF_ReadNextAttribute(&data);
	while (type > 0)
	{
		switch (type)
		{
			case LIBTNEF_TYPE_ATTACHMENTNAME:
				printf("(%d) att name: %s\n",type,data.szAttachmentName);
				break;
			case LIBTNEF_TYPE_ATTACHMENTDATA:
				printf("(%d) att data\n",type);
				break;
			default:
				printf("(%d) unknown part\n",type);
				break;
		}
		printf("calling rna... ");
		type = LibTNEF_ReadNextAttribute(&data);
	}

	LibTNEF_Free(&data);
	
	return 0;
}