#ifndef LISTNODE_H
#define LISTNODE_H

typedef struct lnode ListNode; 

struct lnode{
	ListNode *prev;
	ListNode *next;
	void *data;
};

ListNode *initializeListNode(void *);

//operations on the ends of the list
ListNode *pop(ListNode **);
int push(ListNode **, ListNode *);
int enqueue(ListNode **, ListNode *);
ListNode *dequeue(ListNode **);

//operations within the list
ListNode *removeListNode(ListNode **, ListNode *, int (*)(void *, void *));
ListNode *findListNode(ListNode *, void *, int (*)(void *, void *));

int printListNode(ListNode *);
int printList(ListNode *);
int copyList(ListNode **, ListNode *);

#endif
