#pragma once
#include <stdio.h>
#include <stdlib.h>
// #include "luaManager.h"
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
typedef struct{
	Link next;
	Link last;
	Link this;
	List* root;
	int i;
}Iter;


Link AddNode(List* list,int index,void* data,size_t dataSize);
Link AddNode(List* list,Iter* iter,void* data,size_t dataSize);
Link PushBack(List* list,void* data,size_t dataSize);
void RemoveElement(Iter* iter);
void RemoveElementNF(Iter* iter);
void FreeList(List* list);

void NewIter(List* list,Iter* iter);
int Inc(Iter* iter);