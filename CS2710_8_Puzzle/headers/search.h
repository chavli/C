/******************************************************************************
*	search.h
*
*	Name: Cha Li
*	Date: 09/14/2011
*	Class:
*
*	OS:
*	Kernel:
*****************************************************************************/

#include "node.h"

//public interface
typedef struct search SearchNode;

struct search{
	SearchNode *prev;
	SearchNode *next;
	EightNode *data;
	int priority;
	int depth;
};

SearchNode *initializeEmptySearchNode(void);
SearchNode *initializeSearchNode(EightNode *);
void printSearchNode(SearchNode *);
void printSearchList(SearchNode *);

//dumb searches
int runDepthFirst(SearchNode *, SearchNode *, unsigned int);		//Depth First
int runIterDeepening(SearchNode *, SearchNode *, unsigned int);	//Interative Deepening
int runBreadthFirst(SearchNode *, SearchNode *,
										int (*comp)(void *, void *));								//Breadth First
int runUniformCost(SearchNode *, SearchNode *,
										int (*comp)(void *, void *));								//Uniform Cost

//heuristic searches
int runBestFirst(SearchNode *, SearchNode *,			//Best First
						 			int (*comp)(void *, void*),
									int (*heur)(void *, void *));
int runAStar(SearchNode *, SearchNode *,					//A*
						 	int (*comp)(void *, void*),
						 	int (*heur)(void *, void*), 
							int);	
int runIDAStar(SearchNode *, SearchNode *,				//Iterative Deepening A*
								int (*comp)(void *, void*),
								int (*heur)(void *, void*), 
								int);	




