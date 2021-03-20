#include "socks.h"
#define BYTENUM (int)floor(DRAWERSIZE/(sizeof(char)*8))
#define BYTESTO(index) (int)floor(index/(sizeof(char)*8))

static void PrintBinary(char value){
  for(int i = 0 ;i < 8;i++){
    printf("%d",value & 1);
    value >>=1;
  }
  printf(" | ");
}



Drawer* MakeDrawer(){
	Drawer* drawer = calloc(1,sizeof(Drawer));
	drawer->set = SDLNet_AllocSocketSet(DRAWERSIZE);
	if(!drawer->set){
		printf("failed to allocate socket set!: %s\n",SDLNet_GetError());
	}
	return drawer;
}
void DestroyDrawer(Drawer* drawer){
	SDLNet_FreeSocketSet(drawer->set);
	drawer->set=0;
}
void DepositSock(Drawer* drawer,Sock sock){
	//iterate through the bit fields
	//print the bitsegs
	// for(int i = 0;i < BYTENUM;i++){
	// 	// printf("%b\n",drawer->bits[i]);
	// 	PrintBinary(drawer->bits[i]);
	// 	printf("\n");
	// }
	for(int bitSeg = 0;bitSeg < BYTENUM;bitSeg++){
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
				// printf("before: %b\n",drawer->bits[bitSeg]);
				// printf("before: ");
				// PrintBinary(drawer->bits[bitSeg]);
				drawer->bits[bitSeg]|=1<<i;
				// printf("\nafter: ");
				// PrintBinary(drawer->bits[bitSeg]);
				// printf("\n");

				// for(int q = 0;q < BYTENUM;q++){
				// 	PrintBinary(drawer->bits[q]);
				// 	printf("\n");
				// 	// printf("%b\n",drawer->bits[q]);
				// }
				return;
			}
			testSeg>>=1;
		}
	}
	printf("Failed to deposit sock in drawer. Is it full?\n");
}

int RemoveSock(Drawer* drawer,int index){
	if(index < 0 || index > DRAWERSIZE-1){
		printf("Index out of range (i=%d, range is 0->%d\n",index,DRAWERSIZE-1);
		return 0;
	}
	// for(int i = 0;i < DRAWERSIZE;i++){
		char* targetBitF = &drawer->bits[BYTESTO(index)];
		int offset = index%(sizeof(char)*8);
		if(!((*targetBitF)&(1<<offset))){
			//there is a zero there
			printf("Attempted to remove sock from available slot\n");
			return 0;       
		}else{
			//there is a one there, time to remove
			*targetBitF^=1<<offset;//set the bit to 0
			Sock* sock = &drawer->socks[index];
			if(SDLNet_TCP_DelSocket(drawer->set,sock->socket)==-1){
				printf("Failed to Remove socket from drawer!: %s\n",SDLNet_GetError());
				return 0;
			}
			SDLNet_TCP_Close(sock->socket);
			//call any client remover handler here. Like maybe a logoff or logging thing
			printf("disconnecting!\n");
			drawer->count--;
			sock->socket=0;
			//WARNING: You might need to free 'name' now!
			sock->name=0;
			return drawer->count ? 0 : 1;
		}
	// }
	printf("Index out of range (i=%d, range is 0->%d\n",index,DRAWERSIZE-1);
	return 0;
}

int  IsThereSock(Drawer* drawer,int index){
	// printf("is sock?: ");
	// PrintBinary(drawer->bits[BYTESTO(index)]);
	// printf(" >> %d -> ",index%sizeof(char)*8);
	// PrintBinary((drawer->bits[BYTESTO(index)]>>(index%(sizeof(char)*8))));
	// printf(" %d ",(drawer->bits[BYTESTO(index)]>>(index%(sizeof(char)*8)))&(char)1);
	// printf((drawer->bits[BYTESTO(index)]>>(index%(sizeof(char)*8)))&(char)1 ? "yes" : "no");
	// printf("\n");
	return (drawer->bits[BYTESTO(index)]>>(index%(sizeof(char)*8)))&1;
}