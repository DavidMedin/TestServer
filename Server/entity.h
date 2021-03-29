#pragma once
#include "list.h"
List types;//list of strings


typedef struct{int index,uuid;} Entity;//Somehow indexes into entities
List entities;//is a list of Pools, which contain entity_t's

typedef struct{
	Link me;//references the link in openPools that is this instance
	int openN;
	char open[256];//are offsets into itself, that when hit, represent an open space
	void* data[255];
}Pool;
List openPools;
void* GetPool(Pool* pool, int i);


struct entity_t{
	List components;
};
typedef struct{
	void* data;
	char* type;
}Component;

typedef struct{
	struct Vec2{float x,y;} pos;
	float mag;
}Doohicky;
#define Find(ent,type) (type*)Find_i(ent,#type)
void* Find_i(struct entity_t ent,char* string){
	//iterate through Component list of entity and find type;
}
#define ALOT 123213
void MyFunciton(struct entity_t ent){
	Doohicky* hick = Find(ent,Doohicky);
	hick->pos.x += ALOT;
}
void AlteringFunction(struct entity_t ent){
	//check all entities for collitions
	Entity bumped_id = GetCollision(ent);
	struct entity_t* bumped = GetEntity(bumped_id);
	if(!bumped){
		//faile
	}
	bumped->components.start.data->Quizner=  123;

}