#include "server.h"


IPaddress ip;
TCPsocket sock;SDLNet_SocketSet servSockSet;

int cmdQuit=0;
SDL_mutex* cmdSignal;

List drawers={0};
SDL_mutex* sockMutex;


int SDLCALL ThdParseCmd(void* param){
	while(!cmdQuit) ParseCmdLine();
	return 0;
}
void Connect(){
	TCPsocket tmpSock;
	int ready = SDLNet_CheckSockets(servSockSet,1000);
	if(ready==-1){
		printf("no activity, or error: %s\n",SDLNet_GetError());
		perror("SDLNet_CheckSockets");
	}else if((ready=SDLNet_SocketReady(sock))>0){
		tmpSock = SDLNet_TCP_Accept(sock);
		if(tmpSock){
			//add to socketSet
			// int used=SDLNet_TCP_AddSocket(sockSet,tmpSock);
			SDL_LockMutex(sockMutex);
			int found=0;
			For_Each(drawers,iter){
				if(Iter_Val(iter,Drawer)->count<DRAWERSIZE){
					//this drawer isn't full
					DepositSock(Iter_Val(iter,Drawer),(Sock){tmpSock,"hi"});
					found=1;
				}
			}
			if(!found){
				//create drawer
				Drawer* draw = MakeDrawer();
				PushBack(&drawers,draw,sizeof(Drawer));
				DepositSock(draw,(Sock){tmpSock,"hi"});
			}
			// PushBack(&clientSocks,allocSock,sizeof(TCPsocket));
			SDL_UnlockMutex(sockMutex);
			printf("Connected!\n");
		}
	}else if(ready==-1){
		printf("failed to check ready-ness of socket: %s\n",SDLNet_GetError());
	}
}

TCPsocket* lastRecieved=NULL;
void ReplyToClient(void* data,unsigned int dataSize){
	if(!SendToSocket(*lastRecieved,data,dataSize))
		printf("failed to reply: %s\n",SDLNet_GetError());
	lastRecieved=NULL;
}
void Recieve(){
	For_Each(drawers,drawer){
		if(Iter_Val(drawer,Drawer)->count==0) continue;
		int used = SDLNet_CheckSockets(Iter_Val(drawer,Drawer)->set,0);
		if(used==-1){
			printf("failed to check sockets for recieving: %s\n",SDLNet_GetError());
			return;
		}else if(used>0){
			//loop for ready
			Drawer* draw = Iter_Val(drawer,Drawer);
			for(int i = 0;i < DRAWSOCKSIZE;i++){
				Sock* sock = &draw->socks[i];
				if(SDLNet_SocketReady(sock->socket)>0){
					//get message type
					// TCPsocket sock->socket = ;
					int msgType;
					int gotN=SDLNet_TCP_Recv(sock->socket,&msgType,sizeof(MessageType));
					if(gotN != sizeof(MessageType)){
						//remove from sock set
						if(RemoveSock(draw,i) && drawers.count!=0){
							//drawer is now empty
							DestroyDrawer(draw);
							RemoveElement(&drawer);
							break;
						}
						continue;
					}
					lastRecieved=sock;
					if(gotN==sizeof(MessageType) && msgType==Quit){
						printf("ha ha, very funny\n");
						unsigned int leng = 17;
						void* packet = CreateStringPacket(DisplayText,"Ha ha very funny",&leng);
						ReplyToClient(packet,leng);
						free(packet);
					}
					//get message size
					unsigned int msgSize;
					gotN = SDLNet_TCP_Recv(sock->socket,&msgSize,sizeof(unsigned int));
					if(gotN!=sizeof(unsigned int)){
						printf("error on get size: %s\n",SDLNet_GetError());
						continue;
					}
					void* msg = malloc(msgSize);
					//get data from packet!
					gotN=SDLNet_TCP_Recv(sock->socket,msg,msgSize);
					if(gotN!=msgSize){
						printf("error: %s\n",SDLNet_GetError());
						free(msg);
						continue;
					}else
					switch(msgType){
						case DisplayText:{
							printf("%s\n",(char*)msg); break;
						}
						default: printf("message type %d is not handled by the client yet! Get to work!\n",msgType);
					}
					free(msg);
				}			
			}
		}

	}
}

void SendToAllClients(void* data,unsigned int dataSize){
	SDL_LockMutex(sockMutex);
	For_Each(drawers,drawIter){
		Drawer* drawer = Iter_Val(drawIter,Drawer);
		if(drawer->count==0)continue;
		for(int i = 0;i < DRAWERSIZE;i++){
			if(!IsThereSock(drawer,i)) continue;
			if(!SendToSocket(drawer->socks[i].socket,data,dataSize))
				printf("failed to reply: %s\n",SDLNet_GetError());
		}
	}
	SDL_UnlockMutex(sockMutex);
}

void KillConnection(){
	printf("sending kill messege\n");
	int msg = Quit;
	SendToAllClients(&msg,sizeof(msg));
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
	
	// sockSet=SDLNet_AllocSocketSet(16);
	// if(!sockSet){
	// 	printf("Couldn't allocate socket set!: %s\n",SDLNet_GetError());
	// }
	//allocate socket set
	servSockSet=SDLNet_AllocSocketSet(1);
	if(!servSockSet){
		printf("Couldn't allocate server socket set!: %s\n",SDLNet_GetError());
	}
	//resolve IP address for server sock
	SDLNet_ResolveHost(&ip,NULL,1234);
	sock=SDLNet_TCP_Open(&ip);
	if(sock==NULL){
		printf("TCP Open Error: %s\n", SDLNet_GetError());
		return 0;
	}
	SDLNet_TCP_AddSocket(servSockSet,sock);

	//start drawer list
	PushBack(&drawers,MakeDrawer(),sizeof(Drawer));

	//create the socket list mutex
	sockMutex = SDL_CreateMutex();
	if(!sockMutex)
		printf("Couldn't create socket list mutex: %s\n",SDL_GetError());
	SDL_Thread* parseCmd = SDL_CreateThread(ThdParseCmd,"ThreadParseCmdLine",NULL);
	if(!parseCmd)
		printf("Couldn't create connecting thread!: %s\n",SDL_GetError());
	//resolve IP address
	while(!cmdQuit) {
		Connect();
		Recieve();
	}

	int stat;
	SDL_WaitThread(parseCmd,&stat);
	SDL_DestroyMutex(sockMutex);//no longer needed, in single threaded 'mode'
	KillConnection();
	
	//disconnect
	For_Each(drawers,drawIter){
		Drawer* drawer = Iter_Val(drawIter,Drawer);
		for(int i = 0;i < DRAWERSIZE;i++){
			if(IsThereSock(drawer,i)) RemoveSock(drawer,i);
		}
		DestroyDrawer(drawer);
		RemoveElement(&drawIter);
	}
	if(drawers.count)printf("WHAT!? drawer.count is not zero, it's, it's... %d!!!\n",drawers.count);
	//cleanup
	SDLNet_TCP_Close(sock);
	SDLNet_FreeSocketSet(servSockSet);
	CleanupLua();
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}