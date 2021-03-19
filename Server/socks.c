#include "socks.h"


Drawer* MakeDrawer(){
	Drawer* drawer = calloc(1,sizeof(Drawer));
	drawer->set = SDLNet_AllocSocketSet(DRAWERSIZE);
	if(!drawer->set){
		printf("failed to allocate socket set!: %s\n",SDLNet_GetError());
	}
	return drawer;
}
void DepositSock(Drawer* drawer,Sock sock){
	//iterate through the bit fields
	//print the bitsegs
	for(int i = 0;i < DRAWERSIZE/sizeof(char);i++){
		printf("%b\n",drawer->bits[i]);
	}
	for(int bitSeg = 0;bitSeg < DRAWERSIZE/sizeof(char);bitSeg++){
		char testSeg = drawer->bits[bitSeg];
		//iterate through the bit field to find the first 0
		for(int i = 0;i < 8;i++){
			if(~testSeg&1){
				//do the things
				if(SDLNet_TCP_AddSocket(drawer->set,sock.socket)==-1){
					printf("failed to add sock to drawer: %s\n",SDLNet_GetError());
				}
				drawer->socks[bitSeg*8+i]=sock;
				drawer->count++;
				printf("before: %b\n",drawer->bits[bitSeg]);
				drawer->bits[bitSeg]|=1<<i;
				printf("after: %b\n",drawer->bits[bitSeg]);
				for(int q = 0;q < DRAWERSIZE/sizeof(char);q++){
					printf("%b\n",drawer->bits[q]);
				}
				return;
			}
			testSeg>>=1;
		}
	}
	printf("Failed to deposit sock in drawer. Is it full?\n");
}