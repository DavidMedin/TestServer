#include "luaServerLib.h"
extern int cmdQuit;


void CallString(char* bigString){
	char* context=0;
	char* token=0;
	token = strtok_safe(bigString," ",&context);
	//token is function name
	if(lua_getglobal(state,token)==LUA_TNIL){//pushes token to the stack, is to be called as a function
		printf("command %s doesn't exit!\n",token);
		lua_pop(state,1);
		free(bigString);
		return 1;
	}
	int argumentCount=0;
	token = strtok_safe(NULL," ",&context);

	int sentenceSize=0;
	int ongoingSent=0;
	List sentenceFrags={0};//this stores pointers into bigString. When a sentance has been formed,
					//these are counted up and copied into another big string with spaces to lua
	List completeSents={0};
	while(token){
		// printf("%s\n",token);
		//extract string if there is one there
		int tokLen = strlen(token);
		// printf("first character is %c\n",*token);
		if(*(token+tokLen-1)=='"'){//if the last character of token is "
			if(sentenceFrags.count==0^*token=='"'){//word has quotes on both sides
				printf("Quote syntax error: %s! skipping.\n",token);
				//cleanup string
				For_Each(sentenceFrags){
					RemoveElementNF(&iter);
				}
				sentenceSize=0;
				ongoingSent=0;
				token = strtok_safe(NULL," ",&context);//iterating
				continue;
			}
			sentenceSize += tokLen + 1;// +1 include space or, at the end, a null
			PushBack(&sentenceFrags, token, tokLen);
			char* allocArg = malloc(sentenceSize);
			char* next=allocArg;
			For_Each(sentenceFrags){
				memcpy(next,iter.this->data,iter.this->dataSize);
				next+=iter.this->dataSize+1;//include null/space
				RemoveElementNF(&iter);
				*(next - 1) = sentenceFrags.count ? ' ' : '\0';
			}
			char* quoteContex;
			char* quoteDelim = strtok_safe(allocArg,"\"",&quoteContex);
			lua_pushstring(state,quoteDelim);
			PushBack(&completeSents,allocArg,sentenceSize);
			argumentCount++;
			ongoingSent=0;
		}else if(*token=='"' || ongoingSent){//if the first character of token is "
			ongoingSent=1;
			PushBack(&sentenceFrags,token,tokLen);
			sentenceSize+=tokLen+1;// +1 include space or, at the end, a null
		}else{
			argumentCount++;
			char* end=0;
			long num = strtol(token,&end,10);
			if(token+tokLen==end){
				lua_pushnumber(state,(double)num);
			}else
				lua_pushstring(state,token);
		}
		token = strtok_safe(NULL," ",&context);
	}
	if(sentenceFrags.count){
		printf("Quote syntax error: missing end quote in sentance \"");
		For_Each(sentenceFrags){
			printf("%s ",iter.this->data);
			RemoveElementNF(&iter);
		}
		printf("\"\n");
	}
	if(lua_pcall(state,argumentCount,0,0)!=LUA_OK)
		printf("oh no, lua threw an error! : %s\n",lua_tostring(state,-1));
	For_Each(completeSents){
		RemoveElement(&iter);//IN THEORY this will free all sentences
	}
}

static int ServerQuit(lua_State* L){
	// KillConnection();
	cmdQuit=1;
	return 0;
}
static int ServerSendText(lua_State* L){
	//one argument, the string message
	if(!lua_isstring(L,1)){
		printf("ServerSendText expected a string!\n");
		return 0;
	}
	char* msg = lua_tostring(L,1);
	unsigned int msgLen = (unsigned int)strlen(msg)+1;
	void* data = malloc(msgLen+sizeof(MessageType)+sizeof(unsigned int));
	*((int*)data)=DisplayText;//write at the beginning
	*((int*)data+1)=msgLen;//write after MessageType
	memcpy((char*)data+sizeof(MessageType)+sizeof(unsigned int),msg,msgLen);
	SendToAllClients(data,msgLen+sizeof(MessageType)+sizeof(unsigned int));
	free(data);
	return 0;
}
static const struct luaL_Reg serverLib[]={
	{"ServerQuit",ServerQuit},
	{NULL,NULL}
};

void OpenLuaServerLib(lua_State* L){
	// luaL_setfuncs(L,serverLib,0);
	lua_register(L,"ServerQuit",ServerQuit);
	lua_register(L,"ServerSendText",ServerSendText);
}