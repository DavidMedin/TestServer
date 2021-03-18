#include "luaClientLib.h"
extern int cmdQuit;
void SendData(void* data,unsigned int size);
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
	SendData(data,msgLen+sizeof(MessageType)+sizeof(unsigned int));
	free(data);
	return 0;
}

void LoadLuaLib(lua_State* L){
	lua_register(L,"ClientQuit",ClientQuit);
	lua_register(L,"SendText",SendText);
}