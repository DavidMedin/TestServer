#pragma once
#include <list.h>
#include <game.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <signal.h>

//cimgui
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#include <cimgui_extras.h>
#ifdef _MSC_VER
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

//my garbo
#include <luaManager.h>
#include "luaClientLib.h"

TCPsocket sock;