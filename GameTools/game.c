#include "game.h"
#include <stdlib.h>
void* CreateStringPacket(MessageType msg,char* str,unsigned int* leng){
	unsigned int packSize = (*leng)+sizeof(MessageType)+sizeof(unsigned int);
	void* data = NULL;
	data = malloc(packSize);
	*(MessageType*)data=msg;
	*(unsigned int*)((char*)data+sizeof(MessageType)) = *leng;
	char* msgPlace = ((char*)data+sizeof(MessageType)+sizeof(unsigned int));
	strcpy_s(msgPlace,*leng,str);
	*leng = packSize;
	return data;
}