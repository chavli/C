/******************************************************************************
*	heuristics.c
*
*	Name: Cha Li
*	Date: 09/13/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/
#include <math.h>
#include <stdio.h>
#include "heuristics.h"

int abs(int);
int blockDistance(int, int);

int manhattanDistance(EightNode *goal, EightNode *current){
	int g_col, g_row, c_col, c_row, manhat = 0;
	int i, j;
	
	char *g_board = (goal->state)->state;
	char *c_board = (current->state)->state;

	for(i = 0; i < BOARD_LEN; i++)	//pick a tile from current
		for(j = 0; j < BOARD_LEN; j++){	//find it in goal and calculate md
			if(c_board[i] == g_board[j])
				manhat += blockDistance(i, j);
		}
	return manhat;
}


int misplacedTiles(EightNode *goal, EightNode *current){
	int i, misplaced = 0;
	for(i = 0; i < BOARD_LEN; i++){
		char *board = (goal->state)->state;
		//skip counthing the misplaced empty spot
		if(board[i] > 0 && board[i] != (current->state)->state[i])
			misplaced++;
	}
	return misplaced;
}

//see report for description of this heuristic
int dominoDistance(EightNode *goal, EightNode *current){
	int domino_distance = 0, i, j;
	int domino_zero = (current->state)->empty;

	char *c_board = (current->state)->state;
	char *g_board = (goal->state)->state;

	for(i = 0; i < BOARD_LEN; i++)
		for(j = 0; j < BOARD_LEN; j++){
			if(c_board[i] == g_board[j]){
				if(domino_zero == j)
					domino_distance += blockDistance(i, j);
				else{
					domino_distance += blockDistance(j, domino_zero);
					domino_distance += blockDistance(i, j);
				}
				domino_zero = i;
			}
		}	
	return domino_distance;
}

//i consider priority to be cost, and lower cost is better
int basicComp(SearchNode *sn_1, SearchNode *sn_2){
  int retval;
	if(sn_1 == 0)
		return 1;
	
	if(sn_2->priority < sn_1->priority)
		retval = 1;
	else if(sn_2->priority > sn_1->priority)
		retval = -1; 
	else
		retval = 0;
	return retval;
}

int abs(int x){
	return (x < 0) ? -1 * x : x;
}

//start and finish are 1d index values
int blockDistance(int start, int finish){
	int s_col, s_row, f_col, f_row;
	
	//convert 1d index to 2d coordinates
	s_col = start % BOARD_DIM;
	s_row = start / BOARD_DIM;
	f_col = finish % BOARD_DIM;
	f_row = finish / BOARD_DIM;
	return (abs(s_col - f_col) + abs(s_row - f_row));
}
