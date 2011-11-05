/******************************************************************************
*	8puzzle.c
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
#include <limits.h>
#include "heuristics.h"

char *readBoardFile(FILE *, char *);
void resetGame(Board *, Board *, SearchNode **, SearchNode **);

int main(int argc, char *argv[]){
	if(argc != 3)
		printf("usage: ./8puzzle start_file goal_file\n");
	else{
		FILE *start_state = fopen(argv[1], "r");
		FILE *goal_state = fopen(argv[2], "r");
		char *new_board;
    EightNode *node;
		SearchNode *final, *initial;

		//create start state
		new_board = malloc(BOARD_LEN * sizeof(char));
		readBoardFile(start_state, new_board);
		Board *start = initializeBoard(new_board, "start state");
		node = initializeEightNode(start, "root node");
		initial = initializeSearchNode(node);
		
		//create goal state
		new_board = malloc(BOARD_LEN * sizeof(char));
		readBoardFile(goal_state, new_board);
		Board *goal = initializeBoard(new_board, "goal state");
		node = initializeEightNode(goal, "goal node");
		final = initializeSearchNode(node);
		
		fclose(start_state);
		fclose(goal_state);

		printf("START FILE: %s\n", argv[1]);
		printf("GOAL FILE: %s\n", argv[2]);
		printf("TIME: %d\n", time(NULL));
		printSearchNode(initial);
		printSearchNode(final);
		
		printf("Running Breadth First Search...\n");
		runBreadthFirst(initial, final, 0);
		printf("TIME: %d\n\n", time(NULL));
		
		resetGame(start, goal, &initial, &final);
		printf("Running Uniform Cost Search...\n");
		runUniformCost(initial, final, (int (*)(void *, void *))basicComp);
		printf("TIME: %d\n\n", time(NULL));
		
		resetGame(start, goal, &initial, &final);
		printf("Running Depth First Search...\n");
		runDepthFirst(initial, final, 10000);
		printf("TIME: %d\n\n", time(NULL));

		resetGame(start, goal, &initial, &final);
		printf("Running Iterative Deepening Search...\n");
		runIterDeepening(initial, final, 10000);
		printf("TIME: %d\n\n", time(NULL));
   	
		resetGame(start, goal, &initial, &final);    
		printf("Running Best First Search with misplacedTiles...\n");
		runBestFirst(initial, final,
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))misplacedTiles);
		printf("TIME: %d\n\n", time(NULL));
		
		resetGame(start, goal, &initial, &final);
		printf("Running Best First Search with manhattanDistance...\n");
		runBestFirst(initial, final,
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))manhattanDistance);
		printf("TIME: %d\n\n", time(NULL));

		resetGame(start, goal, &initial, &final);
		printf("Running Best First Search with dominoDistance...\n");
		runBestFirst(initial, final,
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))dominoDistance);
		printf("TIME: %d\n\n", time(NULL));

		resetGame(start, goal, &initial, &final);
		printf("Running A* with misplacedTiles...\n");
		runAStar(initial, final, 
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))misplacedTiles, 
			INT_MAX);
		printf("TIME: %d\n\n", time(NULL));

		resetGame(start, goal, &initial, &final);
		printf("Running A* with manhattanDistance...\n");
		runAStar(initial, final, 
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))manhattanDistance, 
			INT_MAX);
		printf("TIME: %d\n\n", time(NULL));

		resetGame(start, goal, &initial, &final);
		printf("Running A* with dominoDistance...\n");
		runAStar(initial, final, 
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))dominoDistance, 
			INT_MAX);
		printf("TIME: %d\n\n", time(NULL));
    
		resetGame(start, goal, &initial, &final);
		printf("Running IDA* with misplacedTiles...\n");
		runIDAStar(initial, final, 
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))misplacedTiles, 
			0);
		printf("TIME: %d\n\n", time(NULL));
		
		resetGame(start, goal, &initial, &final);
		printf("Running IDA* with manhattanDistance...\n");
		runIDAStar(initial, final, 
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))manhattanDistance, 
			0);
		printf("TIME: %d\n\n", time(NULL));
    
		resetGame(start, goal, &initial, &final);
		printf("Running IDA* with dominoDistance...\n");
		runIDAStar(initial, final, 
			(int (*)(void *, void *))basicComp, 
			(int (*)(void *, void *))dominoDistance, 
			0);
		printf("TIME: %d\n\n", time(NULL));   
	}
	return 0;
}

char *readBoardFile(FILE *fp, char *board){
	char line[32];
	int offset = 0;

	while(fgets(line, 32, fp)){
		sscanf(line, "%d %d %d", 
			(board + offset), 
			(board + offset + 1), 
			(board + offset + 2)
		);
		offset += 3;
	}
}

void resetGame(Board *start, Board *goal, 
								SearchNode **initial,
								SearchNode **final){

	if(*initial && *final){
		free(*initial);
		free(*final);
	}
	EightNode *node;
	node = initializeEightNode(start, "root node");
	*initial = initializeSearchNode(node);
	node = initializeEightNode(goal, "goal node");
	*final = initializeSearchNode(node);
}
