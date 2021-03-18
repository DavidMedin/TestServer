#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include "game.h"
#include "list.h"
#include "luaManager.h"
#include "luaServerLib.h"
// #define strtok_safe
#ifdef _WIN64
#define strtok_safe strtok_s
#else
#define strtok_safe strtok_r
#endif

void KillConnection();
void SendToAllClients(void* data,unsigned int dataSize);
void ReplyToClient(void* data,unsigned int dataSize);