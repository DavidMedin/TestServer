#pragma once
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <dirent.h>
#include "list.h"

extern void OpenLuaServerLib(lua_State* L);

lua_State* state;
List luaFiles=0;
void InitLua(){
	state = luaL_newstate();
	luaL_openlibs(state);
	OpenLuaServerLib(state);
	lua_pushnumber(state,0);
	lua_setglobal(state,"CmdQuit");//when this is 1, cmd will quit
}
void RefreshLuaFiles(){
	//remove all files
	FreeList(&luaFiles);
	DIR* dir;
	struct dirent* ent;
	if((dir=opendir(".")) != NULL){//interesting
		while((ent=readdir(dir))!=NULL){
			if(strstr(ent->d_name,"lua")){//jank but whatever
				char* data=malloc(ent->d_namlen+1);
				memcpy(data,ent->d_name,ent->d_namlen+1);
				PushBack(&luaFiles,data,ent->d_namlen+1);
				//execute the file
				if(luaL_dofile(state,data)){
					printf("error: %s\n",lua_tostring(state,-1));
					//lua_close(state);//why was this here
				}
			}
		}
		closedir(dir);
	}else
		printf("Couldn't open directory \".\"\n");
}
void RefreshCmdFile() {
	List iter = luaFiles;
	while (iter!=NULL) {
		if (strcmp(iter->data, "commands.lua") == 0) {
			RemoveNode(&iter);
			break;//found the file.
		}
		iter = iter->next;
	}
	if (luaL_dofile(state, "commands.lua"))
		printf("error: %s\n", lua_tostring(state, -1));
	else {
		char* data = malloc(14);
		strcpy(data, "commands.lua");
		PushBack(&luaFiles, data,14);
	}
}
void ExecuteLuaFile(char* fileName){//ExecuteLuaFile("dir");
	List iter=luaFiles;
	do{
		char buff[256]={0};//assuming this is zeroed by default
		memcpy(buff,fileName,strlen(fileName));//strleng doesn't include \0
		strcat(buff,".lua");
		if(strcmp(iter->data,buff)==0){
			if(luaL_dofile(state,buff)){
				printf("error: %s\n",lua_tostring(state,-1));
				lua_close(state);
			}
		}
	}while(iter=iter->next);
}
void CleanupLua(){
	FreeList(&luaFiles);
	lua_close(state);
}