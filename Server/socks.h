#pragma once
#include <SDL2/sdl_net.h>
#define DRAWERSIZE 16

typedef struct{
    TCPsocket socket;
    char* name;
}Sock;

typedef struct{
    Sock socks[DRAWERSIZE];
    SDLNet_SocketSet set;
    int count;
    
}Drawer;