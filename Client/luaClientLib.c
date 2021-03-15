#include "luaClientLib.h"
extern int cmdQuit;
static int ClientQuit(lua_State* L){
	cmdQuit=1;
}

void LoadLuaLib(lua_State* L){
	lua_register(L,"ClientQuit",ClientQuit);
}