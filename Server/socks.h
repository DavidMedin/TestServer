#pragma once
#include <SDL2/sdl_net.h>
#define DRAWERSIZE 16
#define DRAWSOCKSIZE DRAWERSIZE/sizeof(char)
typedef struct{
    TCPsocket socket;
    char* name;
}Sock;

/*
Might be faster.
an alternate method of keeping track of socks in this array is to have
another array of pointer that point into the same array.
If you need to delete a sock,index into the array, and have the element
that points to it to point over it. Now set to zero
For adding, just find the first element that is zero, and cause the element
to the left to point to it,and it should point to the element the left was
pointing to. 
    struct Drawer_Link{struct Drawer_Link* left;struct Drawer_Link* right;} map[DRAWERSIZE+1];
*/
typedef struct{
    Sock socks[DRAWERSIZE];
    char bits[DRAWSOCKSIZE];
    SDLNet_SocketSet set;
    int count;
}Drawer;

Drawer* MakeDrawer();
void DestroyDrawer(Drawer* drawer);
void DepositSock(Drawer* drawer,Sock sock);
int RemoveSock(Drawer* drawer,int index);

int IsThereSock(Drawer* drawer,int index);