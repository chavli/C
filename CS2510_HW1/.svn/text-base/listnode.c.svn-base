#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "listnode.h"

ListNode *initializeListNode(int pid){
	ListNode *node = malloc(sizeof(ListNode));
	node->prev = 0;
	node->next = 0;
	node->pid = pid;
	return node;
}

int push(ListNode **root, ListNode *new){
	if(*root == 0)
		*root = new;
	else{
		new->next = *root;
		(*root)->prev = new;
		*root = new;
	}
}

ListNode *pop(ListNode **root){
	ListNode *head = *root;
	if(*root){
		*root = (*root)->next;

		//disconnect head
		head->prev = 0;
		head->next = 0;
	}
	return head;
}

int enqueue(ListNode **root, ListNode *new){
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

ListNode *dequeue(ListNode **root){
	return pop(root);
}

/*
*	returns the removed node or null if the node isn't found.
*	if hardkill > 0, the node is deallocated and null is always returned
*
*/
ListNode *removeListNode(ListNode **root, ListNode *target, int hardkill){
	assert(*root != 0);
	//temp node
	ListNode *iter;

	if( (*root)->pid == target->pid ){
		iter = (*root)->next;
		
		//disconnect old root
		(*root)->prev = 0;
		(*root)->next = 0;
	
		*root = iter;
	}
	else{
		iter = (*root)->next;
		while(iter){
			if( iter->pid == target->pid ){
				//move pointers around and remove target node
				
				//disconnect neighbors from iter
				(iter->prev)->next = iter->next;
				(iter->next)->prev = iter->prev;
				
				//disconnect and free iter
				iter->next = 0;
				iter->prev = 0;

				//if hardkill, completely remove the node
				if(hardkill)
					free(iter);

				break;
			}
			iter = iter->next;
		}
		return iter;
	}
}

ListNode *findListNode(ListNode *iter, int pid){
	while(iter){
		if( iter->pid == pid )
			break;
		iter = iter->next;
	}
	return iter;
}

void printListNode(ListNode *node){
  printf("LISTNODE:(%i), ", node->pid);
  /*if (node->next)
	  printf("next: %s\n", node->next->pid);
  if (node->prev), 
	  printf("prev: %s\n", node->prev->pid);*/
}

void printList(ListNode *root){
	
  printf("=== LIST DUMP(%d) ===: ", getpid());
  while(root){
    printListNode(root);
    root = root->next;
  }
  printf("\n");
}

void copyList(ListNode **dest, ListNode *src){
  ListNode *node;
  while(src){
    node = initializeListNode(src->pid);
    enqueue(dest, node);    
    src = src->next;
  }
}

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
