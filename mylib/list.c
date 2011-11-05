#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"


//allocate memory for a new listnode and return it
ListNode *initializeListNode(void *data){
	ListNode *node = malloc(sizeof(ListNode));
	node->prev = 0;
	node->next = 0;
	node->data = data;
	return node;
}

//push a listnode to the front of the list
//  push -> | list |
int push(ListNode **root, ListNode *new){
  if(new == 0)
    return -1;

	if(*root == 0)
		*root = new;
	else{
		new->next = *root;
		(*root)->prev = new;
		*root = new;
	}
  return 0;
}

//remove a listnode from the front of the list and return it
//  pop <- | list |
ListNode *pop(ListNode **root){
  if(*root == 0)
    return NULL;

	ListNode *head = *root;
	if(*root){
		*root = (*root)->next;

		//disconnect head
		head->prev = 0;
		head->next = 0;
	}
	return head;
}

//add a listnode to the end of a list
//  | list | <- enqueue
int enqueue(ListNode **root, ListNode *new){
  if(new == 0)
    return -1;

	if(*root == 0)
		*root = new;
	else{
		ListNode *iter = *root;
		while(iter->next)
			iter = iter->next;
		new->prev = iter;
		iter->next = new;
	}
}

//remove a listnode from the front of a list and return it
// dequeue <- | list | 
ListNode *dequeue(ListNode **root){
	return pop(root);
}

/*
* remove a node that isnt at the head or tail of a list.
*	returns the removed node or null if the node isn't found.
*/
ListNode *removeListNode(ListNode **root, ListNode *target,
  int (* comp)(void *, void *)){
	if(*root == 0 || target == 0)
    return NULL;
	//temp node
	ListNode *iter;

	if( (*comp)( (*root)->data, target->data) == 0 ){
		iter = (*root)->next;
		
		//disconnect old root
		(*root)->prev = 0;
		(*root)->next = 0;
	
		*root = iter;
	}
	else{
		iter = (*root)->next;
		while(iter){
	    if( (*comp)( iter->data, target->data) == 0 ){
				//move pointers around and remove target node
				
				//disconnect neighbors from iter
				(iter->prev)->next = iter->next;
				(iter->next)->prev = iter->prev;
				
				//disconnect and free iter
				iter->next = 0;
				iter->prev = 0;
				break;
			}
			iter = iter->next;
		}
		return iter;
	}
}

/*
* find a listnode and return a pointer to it, or null if it doesn't exist
*/
ListNode *findListNode(ListNode *root, void *needle,
  int (* comp)(void *, void *)){
  if(root == 0 || needle == 0)
    return NULL;

	while(root){
		if( (*comp)(root->data, needle ) == 0)
			break;
		root = root->next;
	}
	return root;
}

//print some info about a node
int printListNode(ListNode *node){
  if(node == 0)
    return -1;

  printf("LISTNODE:(%p), ", node->data);
  if (node->next)
	  printf("next: %p\n", node->next->data);
  if (node->prev) 
	  printf("prev: %p\n", node->prev->data);
  
  return 0;
}

//print the list
int printList(ListNode *root){
  if(root == 0)
    return -1;

  printf("=== LIST DUMP  ===: ");
  while(root){
    printListNode(root);
    root = root->next;
  }
  printf("\n");
  return 0;
}

//not a deep copy
int copyList(ListNode **dest, ListNode *src){
  if(*dest == 0 || src == 0)
    return -1;

  ListNode *node;
  while(src){
    node = initializeListNode(src->data);
    enqueue(dest, node);    
    src = src->next;
  }

  return 0;
}

/*
char* serializeListNode(ListNode* list) {
	char* pid = 0;
	char* convert = malloc(sizeof(char) * 6);
	int offset = 0;
	
	while (list) {
		sprintf(convert, "%d", list->pid);
		pid = realloc(pid, offset + strlen(convert) + 2);
		memcpy(pid + offset, convert, strlen(convert));
		pid[strlen(convert) + offset] = '\0';
		offset += strlen(convert) + 1;
		strcat(pid, ",");

		list = list->next;
	}
	pid = realloc(pid, offset + 1);
	strcat(pid, "\0");

	return pid;
}

ListNode *deserializeListNode(char *serialized){
  char *pid, *iter = 0;
  char *delim = ",";
  ListNode *root = 0, *node;

  pid = strtok_r(serialized, delim, &iter);
  while( pid != NULL  ){
    node = initializeListNode(atoi(pid));
    enqueue(&root, node);
    pid = strtok_r(NULL, delim, &iter);

  }
	
  return root;
}
*/
