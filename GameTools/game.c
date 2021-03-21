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
// void* CreateDataPacket(MessageType msg,void* data,unsigned int* size){
// 	unsigned int packSize = (*size)+sizeof(MessageType)+sizeof(unsigned int);
// 	void* alloced = NULL;
// 	alloced = malloc(packSize);
// 	*(MessageType*)alloced=msg;
// 	*(unsigned int*)((char*)alloced+sizeof(MessageType)) = *size;
// 	char* msgPlace = ((char*)alloced+sizeof(MessageType)+sizeof(unsigned int));
// 	memcpy(msgPlace,data,*size);
// 	*size = packSize;
// 	return alloced;
// }

void* CreateDataPacket(MessageType msg,unsigned int* allocSize,void* data,unsigned int dataSize){
	unsigned int packSize = (*allocSize)+HEADERSIZE;
	void* alloced = NULL;
	alloced = malloc(packSize);
	*(MessageType*)alloced=msg;
	*(unsigned int*)((char*)alloced+sizeof(MessageType)) = *allocSize;
	char* msgPlace = ((char*)alloced+HEADERSIZE);
	memcpy(msgPlace,data,dataSize);
	*allocSize = HEADERSIZE+dataSize;
	return alloced;
}

int SendToSocket(TCPsocket sock,void* data,unsigned int dataSize){
	int sentN = SDLNet_TCP_Send(sock,data,dataSize);
	if(sentN != dataSize){
		return 0;
	}
	return 1;
}