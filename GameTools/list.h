#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "luaManager.h"
typedef struct Link_t* Link;
typedef struct{
	Link start;
	Link end;
	unsigned int count;
}List;
typedef struct Link_t{
	List* root;
	struct Link_t* next;
	struct Link_t* last;
	void* data;
	size_t dataSize; 
}*Link;

Link AddNode(List* list,int index,void* data,size_t dataSize);
Link PushBack(List* list,void* data,size_t dataSize);
void RemoveNode(Link* link);
void RemoveNodeNF(Link* link);
void FreeList(List list);