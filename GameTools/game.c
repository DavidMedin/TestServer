#include "game.h"
#include <string.h>

#include <stdlib.h>

void* CreateStringPacket(MessageType msg,char* str,unsigned int* leng){
	unsigned int packSize = (*leng)+sizeof(MessageType)+sizeof(unsigned int);
	void* data = NULL;
	data = malloc(packSize);
	*(MessageType*)data=msg;
	*(unsigned int*)((char*)data+sizeof(MessageType)) = *leng;
	char* msgPlace = ((char*)data+sizeof(MessageType)+sizeof(unsigned int));
	strcpy(msgPlace,str);
	*leng = packSize;
	return data;
}

int SendToSocket(TCPsocket sock,void* data,unsigned int dataSize){
	int sentN = SDLNet_TCP_Send(sock,data,dataSize);
	if(sentN != dataSize){
		return 0;
	}
	return 1;
}