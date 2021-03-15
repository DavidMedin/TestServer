#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <signal.h>
#include <luaManager.h>
#include <game.h>
IPaddress serverAddress;
TCPsocket sock;

int stop=0;
void interuptHandler(int nothing){
	stop=1;
}

int main(int argv,char** argc){
	signal(SIGINT,interuptHandler);
	#ifdef _WIN64
	if(SDL_Init(SDL_INIT_VIDEO)<0){
		printf("Failed to initialize SDL!\n");
		return 0;
	}
	#endif
	if(SDLNet_Init()==-1){
		printf("Failed to initialize SDL!\n");
		return 0;
	}

	//init lua
	InitLua(NULL);

	printf("I'm here too you know...\n");
	//resolve IP address
	if(SDLNet_ResolveHost(&serverAddress,"localhost",1234)==-1){
		printf("ResolveHost Error: %s\n",SDLNet_GetError());
	}
	while(!sock){
		sock=SDLNet_TCP_Open(&serverAddress);
		if(sock==NULL){
			printf("TCP Open Error: %s\n",SDLNet_GetError());
		}
	}	
	printf("Connected!\n");
	fd_set inFile;
	FD_ZERO(&inFile);
	FD_SET(0,&inFile);
	fd_set savedFile=inFile;//because I guess select resets inFile
	struct timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=0;
	while(!stop){
		//get message type
		int msgType;
		int gotN=SDLNet_TCP_Recv(sock,&msgType,sizeof(MessageType));
		if(gotN==sizeof(MessageType) && msgType==Quit){
			printf("killing\n");
			break;
		}else if(gotN != sizeof(MessageType)){
			printf("error! %s\n",SDLNet_GetError());
			break;
		}
		//get message size
		unsigned int msgSize;
		gotN = SDLNet_TCP_Recv(sock,&msgSize,sizeof(unsigned int));
		if(gotN!=sizeof(unsigned int)){
			printf("error on get size: %s\n",SDLNet_GetError());
			break;
		}
		void* msg = malloc(msgSize);
		//get data from packet!
		gotN=SDLNet_TCP_Recv(sock,msg,msgSize);
		if(gotN!=msgSize){
			printf("error: %s\n",SDLNet_GetError());
			break;
		}else
		switch(msgType){
			case DisplayText:{
				printf("%s\n",(char*)msg); break;
			}
			default: printf("message type %d is not handled by the client yet! Get to work!\n",*(int*)msg);
		}

		//use cmd line if there is stuff waiting in stdin
		if(select(1,&inFile,NULL,NULL,&timeout)){
			ParseCmdLine();
		}
		inFile=savedFile;
	}

	//disconnect
	SDLNet_TCP_Close(sock);
	printf("Disconnected!\n");

	//cleanup
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}