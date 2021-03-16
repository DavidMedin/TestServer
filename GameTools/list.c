#include "list.h"

0 is before list, increments from there
Link AddNode(List* list,int index,void* data,size_t dataSize){
	check if out of range
	if(index>list->count || index<0){
		printf("index %d out of range!\n",index);
		return NULL;
	}
	Link newLink = malloc(sizeof(struct Link_t));
	newLink->data = data;
	newLink->dataSize = dataSize;
	newLink->root = list;
	if(list->start==NULL){
		newLink->last = NULL;
		newLink->next = NULL;
		list->start=newLink;
		list->end=newLink;
	}else if(index==list->count){//is pushback now
		newLink->last = list->end;
		newLink->next = NULL;
		list->end->next=newLink;
		list->end=newLink;
	}else//somewhere in the list
	{
		Link iter = list->start;
		iterate to find target to move behind (0 will move to behind this list)
		for(int i = 0;i < index;i++){
			iter=iter->next;
		}
		use iter to move behind
		if(iter==list->start)
			list->start=newLink;
		Link oldBehind=iter->last;
		newLink->last=oldBehind;
		newLink->next=iter;
		if(oldBehind)
			oldBehind->next=newLink;
		iter->last = newLink;
		
	}
	list->count++;
	return newLink;
}
Link PushBack(List* list,void* data,size_t dataSize){
	return AddNode(list,list->count,data,dataSize);
}
void _RemoveNode(Link* link,int freeData){
	if((*link)->last==NULL && (*link)->next==NULL){
		//just destroy
		if(freeData) free((*link)->data);
		(*link)->root->start=NULL;
		(*link)->root->end=NULL;
		(*link)->root->count--;
		free(*link);
	}else
	if((*link)->last==NULL){
		//first in List
		Link tmp = link;
		tmp->root->start=tmp->next;

		(*link)->last = NULL;//get rid of last
		if(freeData) free(tmp->data);//free the dynamic mem
		free(tmp);
	}else if(link->last!=NULL && link->next!=NULL){
		//somewhere in the list
		List tmp = *link;
		link->next->last = link->last;//point front to back of this node
		link->last->next = link->next;//point back to front of this node
		*link=link->next;//the next node is what list will point to now
		//free tmp & data
		if(freeData) free(tmp->data);
		free(tmp);
	}else if(link->next==NULL){
		//last in list
		List tmp = *link;
		link->last->next=NULL;
		// *list = (*list)->last;
		*link=NULL;
		if(freeData) free(tmp->data);
		free(tmp);
	}else{
		printf("ERROR!!! missing case for _RemoveNode!!!\n");
	}
}
void RemoveNode(List* list){
	_RemoveNode(list,1);
}
//does not free data
void RemoveNodeNF(List* list){
	_RemoveNode(list,0);
}
//removes/frees everything in list
//warning, this will not deallocate ANY of the data's data (but will dealocate the member "data")
//if you want to deallocate any of that, you'll have to create the loop yourself
void FreeList(List* list){
	while(*list){
		RemoveNode(list);
	}
}
