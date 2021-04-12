#pragma once
#include <varargs.h>
#include <string.h>

#include "list.h"

#define ECS_Null 0
#define ECS_Orc 1
const int componentLengths[];
#define COMPONENT_COUNT 1

#define GET_ARRAY(arr,index) ((char*)arr.array+arr.typeSize*index)
typedef int Entity;// first 2 bytes is the index, last 2 bytes is the verison

//remember to inc count if you add
typedef struct{
	int allocSize;
	int count;
	void* array;
	int typeSize;
}Array;
Array MakeArray(int typeSize,int initCount,int index);
void ArrAddItem(Array* arr,void* data,int index);
void AppendItem(Array* arr,void* data);
typedef struct Sparse{
	Array sparse;//{p}
	Array packed;//{ent,data}
}Sparse;
Sparse MakeSparse(int typeSize, int initCount);

#define PP_THIRD_ARG(a,b,c,...) c
#define VA_OPT_SUPPORTED_I(...) PP_THIRD_ARG(__VA_OPT__(,),1,0,)
#define VA_OPT_SUPPORTED VA_OPT_SUPPORTED_I(?)

//UNTESTED!!!! (repl.it doesn't support __VA_OPT__ I don't thing)
#define NEXTITER(thing,...) thing __VA_OPT__(,ITER(__VA_ARGS__))
#define ITER(thing,...) thing __VA_OPT__(,NEXTITER(__VA_ARGS__))

#define AddEntity(ent,...) _AddEntity(ent,__VA_ARGS__,0)
void _AddEntity(Entity ent,...);

Sparse componentArrs[COMPONENT_COUNT];

int startRec;
int recycleCount;
Array recycle;

typedef void (*System)(Entity);
Array systems;

void Update(float dt);