#ifndef LISTNODE_H
#define LISTNODE_H

typedef struct lnode ListNode; 

struct lnode{
	ListNode *prev;
	ListNode *next;
	int pid;
};

ListNode *initializeListNode(int);

//operations on the ends of the list
ListNode *pop(ListNode **);
int push(ListNode **, ListNode *);
int enqueue(ListNode **, ListNode *);
ListNode *dequeue(ListNode **);

//operations within the list
ListNode *removeListNode(ListNode **, ListNode *, int);
ListNode *findListNode(ListNode *, int);
ListNode *peek_bottom(ListNode*);

void printListNode(ListNode *);
void printList(ListNode *);
void copyList(ListNode **, ListNode *);

char* serializeListNode(ListNode *);
ListNode* deserializeListNode(char*);

#endif
