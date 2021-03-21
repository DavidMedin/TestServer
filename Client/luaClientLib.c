#include "luaClientLib.h"
extern int cmdQuit;
static int ClientQuit(lua_State* L){
	cmdQuit=1;
	return 0;
}
static int SendText(lua_State* L){
	//one argument, the string message
	if(!lua_isstring(L,1)){
		printf("ServerSendText expected a string!\n");
		return 0;
	}
	char* msg = (char*)lua_tostring(L,1);
	unsigned int msgLen = (unsigned int)strlen(msg)+1;
	void* data = malloc(msgLen+sizeof(MessageType)+sizeof(unsigned int));
	*((int*)data)=DisplayText;//write at the beginning
	*((int*)data+1)=msgLen;//write after MessageType
	memcpy((char*)data+sizeof(MessageType)+sizeof(unsigned int),msg,msgLen);
	SendToSocket(sock,data,msgLen+sizeof(MessageType)+sizeof(unsigned int));
	free(data);
	return 0;
}
static int ClientLogin(lua_State* L){
	if(!lua_isstring(L,1)){
		printf("Login expected a string!\n");
		return 0;
	}
	char* name = (char*)lua_tostring(L,1);
	unsigned int dataSize=strlen(name)+1;
	void* data = CreateStringPacket(Login,name,&dataSize);
	SendToSocket(sock,data,dataSize);
	free(data);
	return 0;
}
static int ClientSendTo(lua_State* L){
	if(!lua_isstring(L,1)){
		printf("ClientSendTo expected a string for a username!\n");
		return 0;
	}
	if(!lua_isstring(L,2)){
		printf("ClientSendTo expected a string for a message!\n");
		return 0;
	}
	char* name = (char*)lua_tostring(L,1);
	char* text = (char*)lua_tostring(L,2);
	unsigned int nameSize = strlen(name)+1;
	unsigned int textSize = strlen(text)+1;
	unsigned int packetSize = nameSize+textSize;
	char* packet = CreateDataPacket(TextToUser,&packetSize,name,nameSize);
	memcpy(packet+packetSize,text,textSize);
	SendToSocket(sock,packet,HEADERSIZE+nameSize+textSize);
	free(packet);
	return 0;
}
void LoadLuaLib(lua_State* L){
	lua_register(L,"ClientQuit",ClientQuit);
	lua_register(L,"SendText",SendText);
	lua_register(L,"ClientLogin",ClientLogin);
	lua_register(L,"ClientSendTo",ClientSendTo);
}