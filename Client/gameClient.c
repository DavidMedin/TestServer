#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <signal.h>
#include <luaManager.h>
#include "luaClientLib.h"

#include <list.h>
#include <game.h>
IPaddress serverAddress;
TCPsocket sock;
SDLNet_SocketSet sockSet;

SDL_Thread* recieveThred;

int cmdQuit=0;
void interuptHandler(int nothing){
	cmdQuit=1;
}

void Exit(){
	// SDLNet_TCP_Close(sock);
	printf("Disconnected!\n");

	//cleanup
	lua_close(state);
	SDLNet_Quit();
	SDL_Quit();
}

int SDLCALL Recieve(void* param){
	while(!cmdQuit){
		//check if sock has stuff
		int active;
		if((active=SDLNet_CheckSockets(sockSet,0))==-1){
			printf("failed to check sockets: %s\n",SDLNet_GetError());
		}else if(active>0){
			//get message type
			int msgType;
			int gotN=SDLNet_TCP_Recv(sock,&msgType,sizeof(MessageType));
			if(gotN==sizeof(MessageType) && msgType==Quit){
				printf("killing\n");
				cmdQuit=1;
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
				free(msg);
				break;
			}else
			switch(msgType){
				case DisplayText:{
					printf("%s\n",(char*)msg); break;
				}
				default: printf("message type %d is not handled by the client yet! Get to work!\n",*(int*)msg);
			}
			free(msg);
		}
	}
	cmdQuit=1;
	return 0;
}
void SendData(void* data,unsigned int size){
	int sent = SDLNet_TCP_Send(sock,data,size);
	if(sent!=size){
		printf("failed to send all data: %s\n",SDLNet_GetError());
		cmdQuit=1;
	}
}
int main(int argv,char** argc){
	// List data={0};6	
	// int values[]={1234,234,346,61,3};
	// for(int i = 0;i < 5;i++){
	// 	PushBack(&data,&values[i],sizeof(int));
	// }
	// ForEach(data){
	// 	printf("value is %d\n",*(int*)iter.this->data);
	// }
	// ForEach(data){
	// 	printf("removing element!\n");
	// 	RemoveElementNF(&iter);
	// }
	// ForEach(data){
	// 	printf("value is %d\n",*(int*)iter.this->data);
	// }


	{signal(SIGINT,interuptHandler);
	#ifdef _WIN64
	if(SDL_Init(SDL_INIT_VIDEO)<0){
		printf("Failed to initialize SDL!\n");
		return 1;
	}
	#endif
	if(SDLNet_Init()==-1){
		printf("Failed to initialize SDL!\n");
		return 1;
	}
	//init lua
	InitLua(LoadLuaLib);
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
	sockSet=SDLNet_AllocSocketSet(1);
	if(!sockSet){
		printf("Couldn't allocate socket set: %s\n",SDLNet_GetError());
		Exit();
		return 0;
	}
	if(SDLNet_TCP_AddSocket(sockSet,sock)==-1){
		printf("Couldn't add socket: %s\n",SDLNet_GetError());
		SDLNet_FreeSocketSet(sockSet);
		Exit();
		return 0;
	}
	printf("Connected!\n");}


	recieveThred = SDL_CreateThread(Recieve,"Recieve",NULL);
	while(!cmdQuit) ParseCmdLine();
	int status;
	SDL_WaitThread(recieveThred,&status);
	SDLNet_TCP_DelSocket(sockSet,sock);
	SDLNet_TCP_Close(sock);
	SDLNet_FreeSocketSet(sockSet);
	//disconnect
	Exit();
	return 0;
}