#include "list.h"

//0 is before list, increments from there
List AddNode(List* list,int index,void* data,size_t dataSize){
	List newNode = (List)malloc(sizeof(struct List_t));
	newNode->data = data;
	newNode->dataSize = dataSize;
	if(*list==NULL){
		newNode->last = NULL;
		newNode->next = NULL;
		newNode->root = list;//set root, used for NULL-ing when last element is removed
		*list=newNode;
	}else{
		//gets to root that the entire list should share
		newNode->root = (*list)->root;

		List iter = *list;
		//iterate to find target to move behind (0 will move to behind this list)
		for(int i = 0;i < index;i++){
			if(i+1==index){
				//when the the index to insert into is directly after a element
				List oldNext=iter->next;
				newNode->last=iter;
				newNode->next=oldNext;//can be NULL
				iter->next=newNode;
				if(oldNext)
					oldNext->last=newNode;
				return newNode;
			}else if(iter->next==NULL){
				printf("index %d out of range!\n",index);
				free(newNode);
				return 0;
			}
			iter=iter->next;
		}
		//use iter to move behind
		List oldBehind=iter->last;
		newNode->last=oldBehind;
		newNode->next=iter;
		if(oldBehind)
			oldBehind->next=newNode;
		iter->last = newNode;
		if(*list == iter)
			*list=newNode;//so the list you give it, if it is in front, it will still be in front
	}
	return newNode;
}
List PushBack(List* list,void* data,size_t dataSize){
	int index = 0;
	List iter=*list;
	while(iter){
		iter=iter->next;
		index++;
	}
	return AddNode(list,index,data,dataSize);
}
void _RemoveNode(List* list,int freeData){
	if((*list)->last==NULL && (*list)->next==NULL){
		//just destroy
		List* root = (*list)->root;
		if(freeData) free((*list)->data);
		free(*list);
		*root=0;
		*list=0;
	}else
	if((*list)->last==NULL){
		//first in List
		List tmp = *list;//storing for freeing later
		*list=(*list)->next;//make list equal to the next list node
		(*list)->last = NULL;//get rid of last
		if(freeData) free(tmp->data);//free the dynamic mem
		free(tmp);
		*(*list)->root=*list;//set root variable to first node in list
	}else if((*list)->last!=NULL && (*list)->next!=NULL){
		//somewhere in the list
		List tmp = *list;
		(*list)->next->last = (*list)->last;//point front to back of this node
		(*list)->last->next = (*list)->next;//point back to front of this node
		*list=(*list)->next;//the next node is what list will point to now
		//free tmp & data
		if(freeData) free(tmp->data);
		free(tmp);
	}else if((*list)->next==NULL){
		//last in list
		List tmp = *list;
		(*list)->last->next=NULL;
		// *list = (*list)->last;
		*list=NULL;
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

int ListCount(List list){
	List iter=list;
	int i=0;
	while(iter){
		i++;
		iter=iter->next;
	}
	return i;
}