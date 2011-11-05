/******************************************************************************
*	node.h
*
*	Name: Cha Li
*	Date: 09/13/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

#include "board.h"

typedef struct node EightNode;

struct node{
	char *name;
	Board *state;
	EightNode *parent;
	int e_n;	//cost from parent to this node
	int g_n;	//cost getting to this point
	int h_n;	//estimated cost to goal
};

EightNode *initializeEmptyEightNode(char *);
EightNode *initializeEightNode(Board *, char *);
int isMatch(EightNode *, EightNode *);


void printEightNode(EightNode *);
void printPath(EightNode *);
EightNode **expandEightNode(EightNode *);


/*
*seperate search.c
*
*/




