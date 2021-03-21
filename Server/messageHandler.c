#include "messageHandler.h"

void HandleDisplayText(Sock* sock,unsigned int dataSize,void* data){
	printf("%s\n",(char*)data);
}
void HandleLogIn(Sock* sock,unsigned int dataSize,void* data){
	//check if user is already logged int
	if(sock->loggedIn){
		printf("User already logged in, ignoring\n");
		return;
	}
	sock->loggedIn=1;
	unsigned int nameSize = strlen(data)+1;
	sock->name = malloc(nameSize);
	strcpy(sock->name,data);
}
void HandleTextToUser(Sock* sock,unsigned int dataSize,void* data){
	if(sock->loggedIn){
		/*data
		char* receiving name \0
		char* text \0
		*/
		char* sendTo = data;
		unsigned int toSize = strlen(data)+1;
		char* text = ((char*)data)+toSize;
		unsigned int dataSize = strlen(text)+1;

		//find the reciever
		For_Each(drawers,drawIter){
			Drawer* drawer = Iter_Val(drawIter,Drawer);
			if(drawer->count){
				for(int i = 0;i < DRAWERSIZE;i++){
					Sock* sendSock = &drawer->socks[i];
					if(IsThereSock(drawer,i) && strcmp(sendSock->name,sendTo)==0){
						//this is our guy
						void* data = CreateStringPacket(DisplayText,text,&dataSize);
						if(!SendToSocket(sendSock->socket,data,dataSize)){
							printf("failed to send text?\n");
						}
						free(data);
						return;
					}
				}
			}
		}
	}else
	printf("User 1 tried to send text to user 2, User 1 is not logged in. Ignoring\n");
}