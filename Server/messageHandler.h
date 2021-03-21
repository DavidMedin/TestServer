#pragma once
#include "server.h"
void HandleDisplayText(Sock* sock,unsigned int dataSize,void* data);
void HandleLogIn(Sock* sock,unsigned int dataSize,void* data);
void HandleTextToUser(Sock* sock,unsigned int dataSize,void* data);