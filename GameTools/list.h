#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct List_t{
	struct List_t** root;//points to a list pointer. For example, a pointer to luaFiles in luaManager.h
	struct List_t* next;
	struct List_t* last;
	void* data;
	size_t dataSize;//in bytes
}*List;


List AddNode(List* list,int index,void* data,size_t dataSize);
List PushBack(List* list,void* data,size_t dataSize);
void RemoveNode(List* list);
void RemoveNodeNF(List* list);
void FreeList(List* list);