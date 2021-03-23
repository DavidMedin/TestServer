#pragma once
#include <list.h>
#include <game.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <signal.h>

#include <GL/gl3w.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#include <luaManager.h>

#include "luaClientLib.h"

TCPsocket sock;