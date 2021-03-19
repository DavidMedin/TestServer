#include "luaServerLib.h"
extern int cmdQuit;


static int ServerQuit(lua_State* L){
	// KillConnection();
	cmdQuit=1;
	return 0;
}
static int ServerSendText(lua_State* L){
	//one argument, the string message
	if(!lua_isstring(L,1)){
		printf("ServerSendText expected a string!\n");
		return 0;
	}
	char* msg = (char*)lua_tostring(L,1);
	unsigned int msgLen = (unsigned int)strlen((const char*)msg)+1;
	void* packet = CreateStringPacket(DisplayText,msg,&msgLen);
	SendToAllClients(packet,msgLen);
	free(packet);
	return 0;
}
static int ServerReplyText(lua_State* L){
	if(!lua_isstring(L,1)){
		printf("ServerReplyText Expected a string\n");
		return 0;
	}
	char* msg = (char*)lua_tostring(L,1);
	unsigned int msgLen = (unsigned int)strlen((const char*)msg)+1;
	void* packet = CreateStringPacket(DisplayText,msg,&msgLen);
	ReplyToClient(packet,msgLen);
	free(packet);
	return 0;
}

void OpenLuaServerLib(lua_State* L){
	lua_register(L,"ServerQuit",ServerQuit);
	lua_register(L,"ServerSendText",ServerSendText);
	lua_register(L,"ServerReplyText",ServerReplyText);
}