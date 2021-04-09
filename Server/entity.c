#include "entity.h"
#include "CEntity.h"

Array MakeArray(int typeSize,int initCount){
	Array vec;
	int totalSize = typeSize*initCount;
	vec.array = malloc(totalSize);
	vec.count = 0;
	vec.typeSize = typeSize;
	vec.allocSize = totalSize;
	return vec;
}

void ArrAddItem(Array* arr, void* data,int index){
	memcpy((char*)arr->array+(index*arr->typeSize),data,arr->typeSize);
}

Sparse MakeSparse(int typeSize, int initCount){
	Sparse array;
	array.sparse = MakeArray(typeSize,initCount);
	array.packed = MakeArray(typeSize,initCount);
	return array;
}

void InitECS(){
	int entityCount = 300;
}

void AddEntity(Entity ent, void* data){
	//use first index in recycle is going to be added	
}
