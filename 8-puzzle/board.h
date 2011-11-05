/*****************************************************************************
*	board.h
*
*	Name: Cha Li
*	Date: 09/13/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

#define BOARD_LEN 9
#define BOARD_DIM 3

typedef struct board Board;

struct board{
	char *state;  //an array of length 9 
	char *name;		//name given to this board
	char empty;		//index of unused space
};

Board *initializeBoard(char *, char *);
Board *initializeEmptyBoard(char *);
Board **generateChildren(Board *); //DOUBLE POINTERS!!!!@ ARGH
int match(Board *, Board *);
void printBoard(Board *);

