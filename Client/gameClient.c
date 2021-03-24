
#include "gameClient.h"
IPaddress serverAddress;

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
				printf("error, gotN & msgSize mismatch {%d -> %d}: %s\n",SDLNet_GetError(),gotN,msgSize);
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

	//NOTE: Look at cimgui_extras.h/.cpp in cimgui!
	//maybe just write cmakelists youself for cimgui-make (maybe in same file)
	recieveThred = SDL_CreateThread(Recieve,"Recieve",NULL);
	while(!cmdQuit) {
		// ParseCmdLine();
		// glClearColor(.1f,.1f,.1f,1);
		// glClear(GL_COLOR_BUFFER_BIT);

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

		//imgui
		// if(igBegin("Tool",NULL,0)){
		// 	igText("yello");
		// 	igEnd();
		// }
		bool open=1;
		igShowDemoWindow(&open);
		
		igRender();
		SDL_GL_MakeCurrent(window, glContext);
		// glViewport(0, 0, 800, 800);
		glClearColor(.1f, .1f, .1f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
		SDL_GL_SwapWindow(window);
		// SDL_Delay(500);
	}
	int status;
	SDL_WaitThread(recieveThred,&status);
	SDLNet_TCP_DelSocket(sockSet,sock);
	SDLNet_TCP_Close(sock);
	SDLNet_FreeSocketSet(sockSet);
	//disconnect
	Exit();
	return 0;
}