#include <stdio.h>
#include <SDL.h>
#include <SDL_net.h>

#include "../game.h"
IPaddress serverAddress;
TCPsocket sock;

int main(int argv,char** argc){
	if(SDL_Init(SDL_INIT_VIDEO)<0){
		printf("Failed to initialize SDL!\n");
		return 0;
	}
	if(SDLNet_Init()==-1){
		printf("Failed to initialize SDL!\n");
		return 0;
	}
	printf("I'm here too you know...\n");
	//resolve IP address
	if(SDLNet_ResolveHost(&serverAddress,"localhost",1234)==-1){
		printf("ResolveHost Error: %s\n",SDLNet_GetError());
	}
	sock=SDLNet_TCP_Open(&serverAddress);
	if(sock==NULL){
		printf("TCP Open Error: %s\n",SDLNet_GetError());
		return 0;
	}
	printf("Connected!\n");

	while(1){
		//get message
		int msg;
		int gotN=SDLNet_TCP_Recv(sock,&msg,sizeof(int));
		if(gotN != sizeof(int))
			printf("Recieved bytes and data size mismatch\n");
		if(msg==Quit){
			printf("killing myself\n");
			break;
		}
	}

	//disconnect
	SDLNet_TCP_Close(sock);
	printf("Disconnected!\n");

	//cleanup
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}