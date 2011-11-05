/******************************************************************************
*	board.c
*
*	Name: Cha Li
*	Date: 09/13/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"


//private interface
Board *createChild(Board *, int);
int isValidNeighbor(int , int);


Board *initializeEmptyBoard(char *name){
	Board *b = malloc(sizeof(Board));
	b->state = malloc(BOARD_LEN * sizeof(char));
	b->empty = 0;
	b->name = name;
	return b;
}

Board *initializeBoard(char *data, char *name){
	Board *b = initializeEmptyBoard(name);
	int i;
	for(i = 0; i < BOARD_LEN; i++){
		(b->state)[i] = data[i];
		if(data[i] == 0)
			b->empty = i;
	}
	return b;
}

/*
* The indexes of neighbors of the empty index, i, are
* N: i - 3
*	S: i + 3
*	W: i - 1
*	E: i + 1
*
*/
Board **generateChildren(Board *parent){
	Board **children = malloc(5 * sizeof(Board));
	int origin = parent->empty;
	int i = 0;

	//potential northern neighbor
	if(isValidNeighbor(origin, origin - BOARD_DIM))
		children[i++] = createChild(parent, parent->empty - 3);
	
	//potential southern neighbor
	if(isValidNeighbor(origin, origin + BOARD_DIM))
		children[i++] = createChild(parent, parent->empty + 3);
	
	//potential western neighbor
	if(isValidNeighbor(origin, origin - 1))
		children[i++] = createChild(parent, parent->empty - 1);
	
	//potential eastern neighbor
	if(isValidNeighbor(origin, origin + 1))
		children[i++] = createChild(parent, parent->empty + 1);
	
	return children;
}


int match(Board *ref, Board *check){
	int i = 0;
	while(ref->state[i++] == check->state[i]);
	return i >= BOARD_LEN;
}


void printBoard(Board *b){
	printf("-------\n|%d %d %d|\n|%d %d %d|%s\n|%d %d %d|\n-------\n\n",
		(b->state)[0],
		(b->state)[1],
		(b->state)[2],
		(b->state)[3],
		(b->state)[4],
		(b->state)[5],
		b->name,
		(b->state)[6],
		(b->state)[7],
		(b->state)[8]
	);
}

Board *createChild(Board *parent, int swap_index){
	char *name = malloc(32 * sizeof(char));
	sprintf(name, "Board %d@%d -> 0@%d", 
								parent->state[swap_index], swap_index, parent->empty);

	Board *child = initializeEmptyBoard(name);
	int i;
	//copy the state of parent
	for(i = 0; i < BOARD_LEN; i++)
		child->state[i] = parent->state[i];

	//move tile into empty position
	child->state[parent->empty] = parent->state[swap_index];
	child->state[swap_index] = 0;
	child->empty = swap_index;

	return child;
}

int isValidNeighbor(int origin, int neighbor){
	if(neighbor < 0 || neighbor > 8)
		return 0;

	//convert the linear index values to 2d index values
	int ori_col, ori_row, nei_col, nei_row;
	
	ori_col = origin % BOARD_DIM;
	ori_row = origin / BOARD_DIM;
	
	nei_col = neighbor % BOARD_DIM;
	nei_row = neighbor / BOARD_DIM;

	return (ori_col == nei_col) || (ori_row == nei_row);
}

