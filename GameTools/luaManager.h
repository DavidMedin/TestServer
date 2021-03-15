#pragma once
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <dirent.h>
#include "list.h"

lua_State* state;
List luaFiles;

void InitLua();
void RefreshLuaFiles();
void RefreshCmdFile();
void ExecuteLuaFile(char* fileName);
void CleanupLua();
void ParseCmdLine();