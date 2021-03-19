#include "server.h"


IPaddress ip;
TCPsocket sock;SDLNet_SocketSet servSockSet;

// TCPsocket clientSock;
int cmdQuit=0;
SDL_mutex* cmdSignal;

// List clientSocks={0};
// SDLNet_SocketSet sockSet;
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
			For_Each(drawers,iter){
				if(((Drawer*)Iter_Data)->count<DRAWERSIZE){
					//this drawer isn't full
					DepositSock((Drawer*)Iter_Data,(Sock){tmpSock,"hi"});
				}
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
	if(clientSocks.count==0) return;
	int used = SDLNet_CheckSockets(sockSet,1000);
	if(used==-1){
		printf("failed to check sockets for recieving: %s\n",SDLNet_GetError());
		return;
	}else if(used>0){
		//loop for ready
		For_Each(clientSocks,iter){
			if(SDLNet_SocketReady(Iter_DataVal(TCPsocket))>0){
				//get message type
				TCPsocket clientSock = Iter_DataVal(TCPsocket);
				int msgType;
				int gotN=SDLNet_TCP_Recv(clientSock,&msgType,sizeof(MessageType));
				if(gotN != sizeof(MessageType)){
					//remove from sock set
					int used=SDLNet_TCP_DelSocket(sockSet,Iter_DataVal(TCPsocket)); 
					if(used==-1){
						printf("failed to delete sock from drawer!: %s\n",SDLNet_GetError());
						SDLNet_TCP_DelSocket(sockSet,*(TCPsocket*)Iter_Data);
					}else{
						printf("Disconnecting: %s\n",SDLNet_GetError());
						SDLNet_TCP_Close(Iter_DataVal(TCPsocket));
						RemoveElement(&iter);
					}
					break;
				}
				lastRecieved=Iter_Data;
				if(gotN==sizeof(MessageType) && msgType==Quit){
					printf("ha ha, very funny\n");
					unsigned int leng = 17;
					void* packet = CreateStringPacket(DisplayText,"Ha ha very funny",&leng);
					ReplyToClient(packet,leng);
					free(packet);
				}
				//get message size
				unsigned int msgSize;
				gotN = SDLNet_TCP_Recv(clientSock,&msgSize,sizeof(unsigned int));
				if(gotN!=sizeof(unsigned int)){
					printf("error on get size: %s\n",SDLNet_GetError());
					break;
				}
				void* msg = malloc(msgSize);
				//get data from packet!
				gotN=SDLNet_TCP_Recv(clientSock,msg,msgSize);
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
				free(msg);
			}			
		}
	}
}

void SendToAllClients(void* data,unsigned int dataSize){
	SDL_LockMutex(sockMutex);
	For_Each(clientSocks,iter){
		if(!SendToSocket(Iter_DataVal(TCPsocket),data,dataSize)){
			//remove from sock set
			int used=SDLNet_TCP_DelSocket(sockSet,*(TCPsocket*)Iter_Data); 
			if(used==-1){
				printf("failed to delete sock from drawer!: %s\n",SDLNet_GetError());
				SDLNet_TCP_DelSocket(sockSet,*(TCPsocket*)Iter_Data);
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
	For_Each(clientSocks,iter){
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
	For_Each(clientSocks,iter){
		int used=SDLNet_TCP_DelSocket(sockSet,*(TCPsocket*)Iter_Data); 
		if(used==-1){
			printf("failed to delete sock from drawer!: %s\n",SDLNet_GetError());
			SDLNet_TCP_DelSocket(sockSet,*(TCPsocket*)Iter_Data);
		}else{
			SDLNet_TCP_Close(*(TCPsocket*)iter.this->data);
			RemoveElement(&iter);
			printf("Disconnected!\n");
		}
	}
	SDLNet_TCP_Close(sock);
	//cleanup
	SDLNet_FreeSocketSet(sockSet);
	SDLNet_FreeSocketSet(servSockSet);
	CleanupLua();
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}