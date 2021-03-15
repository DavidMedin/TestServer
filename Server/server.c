#include "server.h"


IPaddress ip;
TCPsocket sock;

// TCPsocket clientSock;
int cmdQuit=0;
SDL_mutex* cmdSignal;

List clientSocks;
SDL_mutex* sockMutex;


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
		TCPsocket* allocSock = malloc(sizeof(TCPsocket));
		*allocSock = tmpSock;
		SDL_LockMutex(sockMutex);
		PushBack(&clientSocks,allocSock,sizeof(TCPsocket));
		SDL_UnlockMutex(sockMutex);
		printf("Connected!\n");
	 }
	}while(cmdQuit==0);
	return 1;
}

void SendToAllClients(void* data,unsigned int dataSize){
	SDL_LockMutex(sockMutex);
	List iter = clientSocks;
	while(iter){
		int sentN = SDLNet_TCP_Send(*(TCPsocket*)iter->data,data,dataSize);
		if(sentN != dataSize){
			// printf("failed to send all data (%d of %u sent): %s message %s\n",sentN,dataSize,SDLNet_GetError(),(char*)data);
			printf("Disconnecting\n");
			SDLNet_TCP_Close(*(TCPsocket*)iter->data);
			RemoveNode(&iter);
		}else
		iter=iter->next;
	}
	SDL_UnlockMutex(sockMutex);
}
void KillConnection(){
	printf("sending kill messege\n");
	int msg = Quit;
	List iter = clientSocks;
	while(iter){
		int sentN = SDLNet_TCP_Send(*((TCPsocket*)iter->data), &msg, sizeof(msg));
		if(sentN != sizeof(msg)){
			printf("failed to send all data (%d of %d sent): %s\n",sentN,(int)sizeof(msg),SDLNet_GetError());
		}
		iter=iter->next;
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
	InitLua(OpenLuaServerLib);
	printf("hello, my guy\n");

	//create the socket list mutex
	sockMutex = SDL_CreateMutex();
	if(!sockMutex)
		printf("Couldn't create socket list mutex: %s\n",SDL_GetError());
	SDL_Thread* connectingThd = SDL_CreateThread(Connect,"Connect",NULL);
	if(!connectingThd)
		printf("Couldn't create connecting thread!: %s\n",SDL_GetError());
		//resolve IP address
	// SDLNet_ResolveHost(&ip, NULL, 1234);
	// sock = SDLNet_TCP_Open(&ip);
	// if (sock == NULL) {
	// 	printf("TCP Open Error: %s\n", SDLNet_GetError());
	// 	return 0;
	// }
	// while(1){
	// 	TCPsocket tmpSock = SDLNet_TCP_Accept(sock);
	// 	if (tmpSock) {
	// 		TCPsocket* allocSock = malloc(sizeof(TCPsocket));
	// 		*allocSock = tmpSock;
	// 		//SDL_LockMutex(sockMutex);
	// 		PushBack(&clientSocks, allocSock, sizeof(TCPsocket));
	// 		//SDL_UnlockMutex(sockMutex);
	// 		printf("Connected!\n");
	// 		break;
	// 	}
	// }
	while(!cmdQuit) ParseCmdLine();
	int stat;
	SDL_WaitThread(connectingThd,&stat);
	SDL_DestroyMutex(sockMutex);//no longer needed, in single threaded 'mode'
	KillConnection();

	//disconnect
	while(clientSocks){
		SDLNet_TCP_Close(*(TCPsocket*)clientSocks->data);
		RemoveNode(&clientSocks);
		printf("Disconnected!\n");
	}

	//cleanup
	CleanupLua();
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}