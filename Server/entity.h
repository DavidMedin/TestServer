#pragma once
#include "list.h"

typedef int Entity;// first 2 bytes is the index, last 2 bytes is the verison

//remember to inc count if you add
typedef struct{
	int allocSize;
	int count;
	void* array;
	int typeSize;
}Array;
Array MakeArray(int typeSize,int initCount,int index);
void ArrAddItem(Array* arr,void* data);

typedef struct{
	Array sparse;
	Array packed;
}Sparse;
Sparse MakeSparse(int typeSize, int initCount);
void AddEntity(Entity ent,void* data);

List componentArrs;

int startRec;
int recycleCount;
Array recycle;

typedef void (*System)(Entity);
Array systems;

void Update(float dt);
