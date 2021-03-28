
#include "gameClient.h"
IPaddress serverAddress;

SDLNet_SocketSet sockSet;

SDL_Thread* recieveThred;


//each line is max 255 characters
int nextLine=0;
char* lines[20] = {0};
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

void PutConsole(char* string,...){
	
	if(lines[nextLine]!=NULL){
		free(lines[nextLine]);
	}
	char* buffer = malloc(255);
	int len = strlen(string);
	va_list valist;
	va_start(valist,string);
	int written=vsprintf(buffer,string,valist);
	va_end(valist);
	if(written < 0){
		printf("PutConsole failed to write!\n");
	}if(written != len){
		PutConsole("Failed to write all data! (%d of %d)\n",written,len);
	}
	lines[nextLine++]=buffer;
	if(nextLine>19) nextLine=0;
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
				printf("error, gotN & msgSize mismatch {%d -> %d}: %s\n",SDLNet_GetError(),gotN,msgSize);
				free(msg);
				break;
			}else
			switch(msgType){
				case DisplayText:{
					//add to lines
					//if(lines[nextLine]!=NULL){
					//	free(lines[nextLine]);
					//}
					//lines[nextLine++]=msg;
					//if(nextLine>19) nextLine=0;
					PutConsole(msg);
					free(msg);
					break;
					//printf("%s\n",(char*)msg); break;
				}
				default:{
					printf("message type %d is not handled by the client yet! Get to work!\n",msgType);
					free(msg);
				}
			}
			//kfree(msg);
		}
		SDL_Delay(500);
	}
	cmdQuit=1;
	return 0;
}

int main(int argv,char** argc){
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

	//initialize window stuff
	SDL_Window* window = SDL_CreateWindow("David's Game Client",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,SDL_WINDOW_OPENGL);
	if(!window){
		printf("failed to intialize window: %s\n",SDL_GetError());
		Exit();
		return 1;
	}
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
	if(Do_gl3wInit()){
		printf("failed to intialize glfw\n");
		Exit();
		return 1;
	}
	igCreateContext(NULL);
	ImGui_ImplSDL2_InitForOpenGL(window,glContext);
	ImGui_ImplOpenGL3_Init("#version 130");
	igStyleColorsDark(NULL);

	//main loop
	recieveThred = SDL_CreateThread(Recieve,"Recieve",NULL);
	//nextLine++;
	//lines[0] = "This is a test string";
	while(!cmdQuit) {
		// ParseCmdLine();

		SDL_Event event;
		if(SDL_PollEvent(&event)){
			ImGui_ImplSDL2_ProcessEvent(&event);
			if(event.type == SDL_QUIT){
				cmdQuit=1;
				break;
			}
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		igNewFrame();

		ImVec2 vector = {0};
		vector.y= -( igGetStyle()->ItemSpacing.y + igGetFrameHeightWithSpacing() );
		if(igBegin("Console",NULL,0)){
			igBeginChildStr("scroller",vector,true,0);
			int i = nextLine;
			while(1){
				if(lines[i]!=NULL){
					igText(lines[i]);
				}
				if(++i>19){
					i=0;
				}
				if(i==nextLine) break;
			}
			igEndChild();
		}
		ImGuiInputTextFlags textInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;
		char inputBuffer[255] = {0};
		if(igInputText("input",inputBuffer,255,textInputFlags,NULL,NULL)){
			//enter was pressed!
 
			RefreshCmdFile();

			int rez;
			if((rez=luaL_loadstring(state,inputBuffer))==LUA_OK){
				if(lua_pcall(state,0,0,0)!=LUA_OK)
				printf("oh no, lua threw an error! : %s\n",lua_tostring(state,-1));
			}else{
				if(rez==LUA_ERRSYNTAX)
					printf("Cmd line syntax error: %s\n",lua_tostring(state,-1));
				else
					printf("cmd line memory error: %s\n",lua_tostring(state,-1));
			}
		}
		igEnd();
		bool open=1;
		igShowDemoWindow(&open);
		
		igRender();
		SDL_GL_MakeCurrent(window, glContext);
		//is this needed for window resizing?
		// glViewport(0, 0, 800, 800);
		glClearColor(.1f, .1f, .1f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
		SDL_GL_SwapWindow(window);
	}
	int status;
	SDL_WaitThread(recieveThred,&status);
	SDLNet_TCP_DelSocket(sockSet,sock);
	SDLNet_TCP_Close(sock);
	SDLNet_FreeSocketSet(sockSet);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	igDestroyContext(NULL);
	SDL_GL_DeleteContext(glClear);
	SDL_DestroyWindow(window);
	window=NULL;
	//disconnect
	Exit();
	return 0;
}