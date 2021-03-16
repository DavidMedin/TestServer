#include "list.h"

//0 is before list, increments from there
Link AddNode(List* list,int index,void* data,size_t dataSize){
	//check if out of range
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
		//iterate to find target to move behind (0 will move to behind this list)
		for(int i = 0;i < index;i++){
			iter=iter->next;
		}
		//use iter to move behind
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
// List PushBack(List* list,void* data,size_t dataSize){
// 	int index = 0;
// 	List iter=*list;
// 	while(iter){
// 		iter=iter->next;
// 		index++;
// 	}
// 	return AddNode(list,index,data,dataSize);
// }
// void _RemoveNode(List* list,int freeData){
// 	if((*list)->last==NULL && (*list)->next==NULL){
// 		//just destroy
// 		List* root = (*list)->root;
// 		if(freeData) free((*list)->data);
// 		free(*list);
// 		*root=0;
// 		*list=0;
// 	}else
// 	if((*list)->last==NULL){
// 		//first in List
// 		List tmp = *list;//storing for freeing later
// 		*list=(*list)->next;//make list equal to the next list node
// 		(*list)->last = NULL;//get rid of last
// 		if(freeData) free(tmp->data);//free the dynamic mem
// 		free(tmp);
// 		*(*list)->root=*list;//set root variable to first node in list
// 	}else if((*list)->last!=NULL && (*list)->next!=NULL){
// 		//somewhere in the list
// 		List tmp = *list;
// 		(*list)->next->last = (*list)->last;//point front to back of this node
// 		(*list)->last->next = (*list)->next;//point back to front of this node
// 		*list=(*list)->next;//the next node is what list will point to now
// 		//free tmp & data
// 		if(freeData) free(tmp->data);
// 		free(tmp);
// 	}else if((*list)->next==NULL){
// 		//last in list
// 		List tmp = *list;
// 		(*list)->last->next=NULL;
// 		// *list = (*list)->last;
// 		*list=NULL;
// 		if(freeData) free(tmp->data);
// 		free(tmp);
// 	}else{
// 		printf("ERROR!!! missing case for _RemoveNode!!!\n");
		
// 	}
// }
// void RemoveNode(List* list){
// 	_RemoveNode(list,1);
// }
// //does not free data
// void RemoveNodeNF(List* list){
// 	_RemoveNode(list,0);
// }
// //removes/frees everything in list
// //warning, this will not deallocate ANY of the data's data (but will dealocate the member "data")
// //if you want to deallocate any of that, you'll have to create the loop yourself
// void FreeList(List* list){
// 	while(*list){
// 		RemoveNode(list);
// 	}
// }
