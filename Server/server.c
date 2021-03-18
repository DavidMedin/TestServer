#include "server.h"


IPaddress ip;
TCPsocket sock;

// TCPsocket clientSock;
int cmdQuit=0;
SDL_mutex* cmdSignal;

List clientSocks={0};
SDL_mutex* sockMutex;

SDLNet_SocketSet sockSet;


int SDLCALL Connect(void* param){
	//resolve IP address
	SDLNet_ResolveHost(&ip,NULL,1234);
	sock=SDLNet_TCP_Open(&ip);
	if(sock==NULL){
		printf("TCP Open Error: %s\n", SDLNet_GetError());
		return 0;
	}
	TCPsocket tmpSock;
	do{
        tmpSock = SDLNet_TCP_Accept(sock);
        if(tmpSock){
			//add to socketSet
			int used=SDLNet_TCP_AddSocket(sockSet,tmpSock);
			printf("used socks: %d\n",used);
			if(used==-1){
				printf("Failed to add socket to socket set!: %s\n",SDLNet_GetError());

			}else{
				TCPsocket* allocSock = malloc(sizeof(TCPsocket));
				*allocSock = tmpSock;
				SDL_LockMutex(sockMutex);
				PushBack(&clientSocks,allocSock,sizeof(TCPsocket));
				SDL_UnlockMutex(sockMutex);
				printf("Connected!\n");
			}
        }
	}while(cmdQuit==0);
	return 1;
}

void SendToAllClients(void* data,unsigned int dataSize){
	SDL_LockMutex(sockMutex);
	int ready = SDLNet_CheckSockets(sockSet,0);
	if(ready==-1){
		printf("No sockets have anything of interest: %s\n",SDLNet_GetError());
		perror("SDLNet_CheckSockets");
	}else
	For_Each(clientSocks){
		int sentN = SDLNet_TCP_Send(*(TCPsocket*)Iter_Data,data,dataSize);
		if(sentN != dataSize){
			//remove from sock set
			int used=SDLNet_TCP_DelSocket(sockSet,*(TCPsocket*)Iter_Data); 
			if(used==-1){
				printf("failed to delete sock from drawer!: %s\n",SDLNet_TCP_DelSocket(sockSet,*(TCPsocket*)Iter_Data));
			}else{
				printf("Disconnecting\n");
				SDLNet_TCP_Close(*(TCPsocket*)iter.this->data);
				RemoveElement(&iter);
			}
		}
	}
	SDL_UnlockMutex(sockMutex);
}
void KillConnection(){
	printf("sending kill messege\n");
	int msg = Quit;
	For_Each(clientSocks){
		int sentN = SDLNet_TCP_Send(*((TCPsocket*)iter.this->data), &msg, sizeof(msg));
		if(sentN != sizeof(msg)){
			printf("failed to send all data (%d of %d sent): %s\n",sentN,(int)sizeof(msg),SDLNet_GetError());
		}
	}
}



int main(int argv, char** argc){
#ifdef _WIN64
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("failed to initialize SDL!\n");
		return 0;
	}
#endif
	if(SDLNet_Init()==-1){
		printf("failed to initialize SDL_net!\n");
		return 0;
	}
	InitLua(OpenLuaServerLib);//wasn't here for some reason
	printf("hello, my guy\n");
    
	//allocate socket set
	sockSet=SDLNet_AllocSocketSet(16);
	if(!sockSet){
		printf("Couldn't allocate socket set!: %d\n",SDLNet_GetError());
	}
	//create the socket list mutex
	sockMutex = SDL_CreateMutex();
	if(!sockMutex)
		printf("Couldn't create socket list mutex: %s\n",SDL_GetError());
	SDL_Thread* connectingThd = SDL_CreateThread(Connect,"Connect",NULL);
	if(!connectingThd)
		printf("Couldn't create connecting thread!: %s\n",SDL_GetError());
    //resolve IP address
    
	while(!cmdQuit) ParseCmdLine();
	int stat;
	SDL_WaitThread(connectingThd,&stat);
	SDL_DestroyMutex(sockMutex);//no longer needed, in single threaded 'mode'
	KillConnection();
    
	//disconnect
	For_Each(clientSocks){
		SDLNet_TCP_Close(*(TCPsocket*)iter.this->data);
		RemoveElement(&iter);
		printf("Disconnected!\n");
	}
	//cleanup
    SDLNet_FreeSocketSet(sockSet);
	CleanupLua();
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}