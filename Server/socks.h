#pragma once
#include <SDL2/sdl_net.h>
#define DRAWERSIZE 16

typedef struct{
    TCPsocket socket;
    char* name;
}Sock;

typedef struct{
    Sock socks[DRAWERSIZE];
    char bits[DRAWERSIZE/sizeof(char)];
    SDLNet_SocketSet set;
    int count;
}Drawer;

Drawer* MakeDrawer();
void DepositSock(Drawer* drawer,Sock sock);