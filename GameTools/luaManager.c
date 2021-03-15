#include "luaManager.h"

// extern void OpenLuaServerLib(lua_State* L);
typedef void (*CustomLibrary)(lua_State* L);

void InitLua(CustomLibrary lib){
	state = luaL_newstate();
	luaL_openlibs(state);
	if(lib) lib(state);
	// lua_pushnumber(state,0);
	// lua_setglobal(state,"CmdQuit");//when this is 1, cmd will quit
	luaFiles=NULL;
}
void RefreshLuaFiles(){
	//remove all files
	FreeList(&luaFiles);
	DIR* dir;
	struct dirent* ent;
	if((dir=opendir(".")) != NULL){//interesting
		while((ent=readdir(dir))!=NULL){
			if(strstr(ent->d_name,"lua")){//jank but whatever
				int nameLen = strlen(ent->d_name)+1;
				char* data=malloc(nameLen);
				memcpy(data,ent->d_name,nameLen);
				PushBack(&luaFiles,data,nameLen);
				//execute the file
				if(luaL_dofile(state,data)){
					printf("error: %s\n",lua_tostring(state,-1));
					//lua_close(state);//why was this here
				}
			}
		}
		closedir(dir);
	}else
		printf("Couldn't open directory \".\"\n");
}
void RefreshCmdFile() {
	List iter = luaFiles;
	while (iter!=NULL) {
		if (strcmp(iter->data, "commands.lua") == 0) {
			RemoveNode(&iter);
			break;//found the file.
		}
		iter = iter->next;
	}
	if (luaL_dofile(state, "commands.lua"))
		printf("error: %s\n", lua_tostring(state, -1));
	else {
		char* data = malloc(14);
		strcpy(data, "commands.lua");
		PushBack(&luaFiles, data,14);
	}
}
void ExecuteLuaFile(char* fileName){//ExecuteLuaFile("dir");
	List iter=luaFiles;
	do{
		char buff[256]={0};//assuming this is zeroed by default
		memcpy(buff,fileName,strlen(fileName));//strleng doesn't include \0
		strcat(buff,".lua");
		if(strcmp(iter->data,buff)==0){
			if(luaL_dofile(state,buff)){
				printf("error: %s\n",lua_tostring(state,-1));
				lua_close(state);
			}
		}
	}while(iter=iter->next);
}
void CleanupLua(){
	FreeList(&luaFiles);
	lua_close(state);
}


//----------------utilities

void ParseCmdLine(){
	//get text input as commands
	List chunks=0;
	unsigned int byteCount=0;
	while(1){
		char* buff = malloc(256);
		// putc((int)'>',stdout);
		char* rez = fgets(buff,256,stdin);
		size_t rezLen = strlen(rez);
		if(rez==NULL){
			free(buff);//not needed
			printf("oh no, fgets raised an error!\n");
			break;
		}
		else if(rezLen!=255){
			byteCount += (unsigned int)rezLen+1;//include the \0
			List tmp = PushBack(&chunks,buff,rezLen+1);//include the \0
			break;
		}
		else{
			byteCount += 255;//excluding /0
			List tmp = PushBack(&chunks,buff,256);
			printf("not done %d\n",(int)tmp->dataSize);
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
		start+=(int)iter->dataSize-1;//remove the \0
		iter=iter->next;
	}while(iter!=NULL);
	*(bigString+byteCount-1)=(char)0;//add the null character to the end
	*(bigString+byteCount-2)=(char)0;//get rid of the \n
	FreeList(&chunks);
	//refresh lua files for hot reloading
	RefreshCmdFile();

	int rez;
	if((rez=luaL_loadstring(state,bigString))==LUA_OK){
		if(lua_pcall(state,0,0,0)!=LUA_OK)
		printf("oh no, lua threw an error! : %s\n",lua_tostring(state,-1));
	}else{
		if(rez==LUA_ERRSYNTAX)
			printf("Cmd line syntax error: %s\n",lua_tostring(state,-1));
		else
			printf("cmd line memory error: %s\n",lua_tostring(state,-1));
	}
	//CallString(bigString);

	free(bigString);

}
