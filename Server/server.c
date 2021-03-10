#include "server.h"

#include "luaServerLib.c"
IPaddress ip;
TCPsocket sock;

TCPsocket clientSock;

void Connect(){
	//resolve IP address
	SDLNet_ResolveHost(&ip,NULL,1234);
	sock=SDLNet_TCP_Open(&ip);
	if(sock==NULL){
		printf("TCP Open Error: %s\n", SDLNet_GetError());
		return;
	}
	do{
		clientSock = SDLNet_TCP_Accept(sock);
	}while(clientSock==NULL);
	printf("Connected!\n");
}
void KillConnection(){
	printf("sending kill messege\n");
	int msg = Quit;
	int sentN = SDLNet_TCP_Send(clientSock,&msg,sizeof(msg));
	if(sentN != sizeof(msg)){
		printf("failed to send all data: %s\n",SDLNet_GetError());
	}
}
int ParseCmdLine(){
	//get text input as commands
	List chunks=0;
	unsigned int byteCount=0;
	while(1){
		char* buff = malloc(256);
		putc((int)'>',stdout);
		char* rez = fgets(buff,256,stdin);
		size_t rezLen = strlen(rez);
		if(rez==NULL){
			free(buff);//not needed
			printf("oh no, fgets raised an error!\n");
			break;
		}
		else if(rezLen!=255){
			byteCount += rezLen+1;//include the \0
			List tmp = PushBack(&chunks,buff,rezLen+1);//include the \0
			break;
		}
		else{
			byteCount += 255;//excluding /0
			List tmp = PushBack(&chunks,buff,256);
			printf("not done %lli\n",tmp->dataSize);
		}
	}
	if (byteCount == 0) { printf("fgets got 0 bytes?\n"); return 0; }
	//have gone through the entire stream
	//contiguize chunks into one
	char* bigString = malloc(byteCount);
	int start=0;
	List iter=chunks;
	do{
		memcpy(bigString+start,iter->data,iter->dataSize-1);
		start+=iter->dataSize-1;//remove the \0
		iter=iter->next;
	}while(iter!=NULL);
	*(bigString+byteCount-1)=(char)0;//add the null character to the end
	*(bigString+byteCount-2)=(char)0;//get rid of the \n
	// printf("%s\n",bigString);
	FreeList(&chunks);
	//refresh lua files for hot reloading
	RefreshCmdFile();

	char* context=0;
	char* token=0;
	token = strtok_s(bigString," ",&context);
	//token is function name
	if(lua_getglobal(state,token)==LUA_TNIL){//pushes token to the stack, is to be called as a function
		printf("command %s doesn't exit!\n",token);
		lua_pop(state,1);
		free(bigString);
		return 1;
	}
	int argumentCount=0;
	token = strtok_s(NULL," ",&context);
	while(token){
		argumentCount++;
		printf("%s\n",token);
		//extract string if there is one there
		char* end=0;
		long num = strtol(token,&end,10);
		if(token==end){
			printf("%s is not a number\n",token);
			lua_pushstring(state,token);
		}else
			lua_pushnumber(state,(double)num);
		token = strtok_s(NULL," ",&context);
	}
	if(lua_pcall(state,argumentCount,0,0)!=LUA_OK)
		printf("oh no, lua threw an error! : %s\n",lua_tostring(state,-1));
	free(bigString);
	lua_getglobal(state,"CmdQuit");
	if(lua_tonumber(state,-1)==1){
		return 0;	
	}
	return 1;
}


int main(int argv, char** argc){
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("failed to initialize SDL!\n");
		return 0;
	}
	if(SDLNet_Init()==-1){
		printf("failed to initialize SDL_net!\n");
		return 0;
	}
	InitLua();

	Connect();
	while(ParseCmdLine());


	KillConnection();

	//disconnect
	SDLNet_TCP_Close(sock);
	printf("Disconnected!\n");

	//cleanup
	CleanupLua();
	SDLNet_Quit();
	SDL_Quit();
	return 0;
}