#include "entity.h"
#include "CEntity.h"

const int componentLengths[] = {sizeof(int)};

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
	arr->count++;
}
//void AppendItem(Array* arr,void* data){
//	ArrAddItem(arr,data,arr->count);
//}
Sparse MakeSparse(int typeSize, int initCount){
	Sparse array;
	array.sparse = MakeArray(sizeof(int),initCount);
	array.packed =MakeArray(sizeof(int)+typeSize,initCount);//added an int to the packed array to easily know what entity the
	//component belongs to, and iterate through those
	return array;
}
int SAddItem(Sparse* sparse,int index){
	//sparse is ALWAYS an int array
	if(*(int*)GET_ARRAY(sparse->sparse,index) >= sparse->sparse.count) return 0; //return 0 if item already exists

	int pIndex = sparse->sparse.count;//packed index
	//*(int*)GET_ARRAY(sparse->sparse,index) = sparse->sparse.count;//count of sparse and packed should be the same
	ArrAddItem(&sparse->sparse,pIndex,index);
	//initialize the component to zero (for now)
	memset(GET_ARRAY(sparse->packed,pIndex),0,sparse->packed.typeSize);//set component (and ent) to zero
	*(int*)GET_ARRAY(sparse->packed,pIndex) = index;//write the entity into the packed slot
	sparse->packed.count++;
	return 1;
}

void InitECS(){
	int entityCount = 300;

	recycleCount = entityCount;
	recycle = MakeArray(sizeof(int),recycleCount);
	
	//use COMPONENT_COUNT to initialize component arrays
	//componentArrs = MakeArray(sizeof(Sparse),COMPONENT_COUNT);
	for(int i = 0;i < COMPONENT_COUNT;i++){
		componentArrs[i] = MakeSparse(componentLengths[i-1],entityCount);
		//AppendItem(&componentArrs,&sparse);
	}
}

void _AddEntity(Entity ent, ...){
	//use first index in recycle is going to be added
	//all entity entries in component arrays must be updated to point to next obj in packed array
	//and size increases
	Entity beMade = startRec;
	va_list components;
	va_start(components,ent);
	//iterate through components and intiialize them to zero (for now)
	//look for a macro-injected-zero in arguments
	int component;
	while(component = va_arg(components,int)){
		//add entity to the component's array
		SAddItem(&componentArrs[component-1],ent);
	}
}
