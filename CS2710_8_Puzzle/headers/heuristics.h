/******************************************************************************
*	heuristics.h
*
*	Name: Cha Li
*	Date: 09/13/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

#include "search.h"

//heuristics
int manhattanDistance(EightNode *, EightNode *);
int misplacedTiles(EightNode *, EightNode *);
int dominoDistance(EightNode *, EightNode *);

//comparison
int basicComp(SearchNode *, SearchNode *);

