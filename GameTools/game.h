#pragma once
#include <SDL2/SDL_net.h>
#define _CRT_SECURE_NO_WARNINGS 1//linux doesn't use it, I won't

#define HEADERSIZE sizeof(MessageType)+sizeof(unsigned int)
typedef enum{
	Quit,
	DisplayText,
	Login,
	TextToUser
}MessageType;
//PACKET HEADER FORMAT
/*
{
	MessageType (4 bytes), //nothing after this is needed if MessageType is Quit
	unsigned int sizeOfMessage,
	bla bla your data here
}
*/

//WARNING: allocates new data, you must clean yourself
void* CreateStringPacket(MessageType msg,char* str,unsigned int* leng);
//will write offset to next available data slot to allocSize
void* CreateDataPacket(MessageType msg,unsigned int* allocSize,void* data,unsigned int dataSize);

int SendToSocket(TCPsocket sock,void* data,unsigned int dataSize);