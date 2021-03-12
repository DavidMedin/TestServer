#pragma once
#include "server.h"
extern TCPsocket clientSock;//defined in server.c

static int ServerQuit(lua_State* L){
	printf("sending kill messege\n");
	int msg = Quit;
	int sentN = SDLNet_TCP_Send(clientSock,&msg,sizeof(msg));
	if(sentN != sizeof(msg)){
		printf("failed to send all data: %s\n",SDLNet_GetError());
	}
	return 0;
}

static const struct luaL_Reg serverLib[]={
	{"ServerQuit",ServerQuit},
	{NULL,NULL}
};

void OpenLuaServerLib(lua_State* L){
	// luaL_setfuncs(L,serverLib,0);
	lua_register(L,"ServerQuit",ServerQuit);
}