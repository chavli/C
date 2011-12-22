/******************************************************************************
*	search.h
*
*	Name: Cha Li
*	Date: 09/19/2011
*	Class:
*
*	OS:
*	Kernel:
*****************************************************************************/

#include "city.h"

//public interface
typedef struct search SearchNode;

struct search{
	SearchNode *prev;
	SearchNode *next;
	CityNode *city;
	int priority;
	int depth;
};

SearchNode *initializeEmptySearchNode(void);
SearchNode *initializeSearchNode(CityNode *);
void printSearchNode(SearchNode *);
void printSearchList(SearchNode *);
void setAdjList(CityNode *);

//uninformed searches
int runDepthFirst(CityNode *, CityNode *, int);
int runIterDeepening(CityNode *, CityNode *);
int runBreadthFirst(CityNode *, CityNode *, int (*comp)(void *, void *));
int runUniformCost(CityNode *, CityNode *, int (*comp)(void *, void *));

//heuristic searches
int runBestFirst(CityNode *, CityNode *,
									int (*comp)(void *, void *),
									int (*heur)(void *));
int runAStar(CityNode *, CityNode *,
									int (*comp)(void *, void *),
									int (*heur)(void *),
									int);
int runIDAStar(CityNode *, CityNode *,
									int (*comp)(void *, void *),
									int (*heur)(void *),
									int);

