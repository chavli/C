/******************************************************************************
*	node.c
*
*	Name: Cha Li
*	Date: 09/13/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "node.h"

EightNode *initializeEmptyEightNode(char *name){
	EightNode *node = malloc(sizeof(EightNode));
	node->name = name;
	node->parent = 0;
	node->e_n = 0;
	node->g_n = 0;
	node->h_n = 0;
	return node;
}

EightNode *initializeEightNode(Board *state, char *name){
	EightNode *node = initializeEmptyEightNode(name);
	node->state = state;
	return node;
}

void printEightNode(EightNode *node){
	printf("[EightNode] g(n): %d, h(n): %d\n", node->g_n, node->h_n);
	printBoard(node->state);
}

void printPath(EightNode *node){
	while(node){
		printEightNode(node);
		node = node->parent;
	}
}


int isMatch(EightNode *goal, EightNode *test){
	return match(goal->state, test->state);
}


EightNode **expandEightNode(EightNode *node){
	Board **states = generateChildren(node->state);
	EightNode **children = malloc(5 * sizeof(EightNode));
		
	int i = 0;
	while(states[i]){
		children[i] = initializeEightNode(states[i], states[i]->name);
		children[i++]->parent = node;
	}

	return children;
}
