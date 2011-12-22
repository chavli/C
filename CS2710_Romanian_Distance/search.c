/******************************************************************************
*	search.c
*
*	Name: Cha Li
*	Date: 09/19/2011
*	Class:
*
*	OS:
*	Kernel:
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "search.h"

/*
*private interface
*/
void push(SearchNode **, SearchNode *);
void enqueue(SearchNode **, SearchNode *);
void priorityEnqueue(SearchNode **,	SearchNode *, 
											int (*comp)(void *, void *));
SearchNode *pop(SearchNode **);
SearchNode *dequeue(SearchNode **);
int isExpanded(SearchNode *, SearchNode *);
void printResult(SearchNode *);

void push(SearchNode **root, SearchNode *new){
	if(*root == 0)
		*root = new;
	else{
		new->next = *root;
		(*root)->prev = new;
		*root = new;
	}
}

void enqueue(SearchNode **root, SearchNode *new){
	if(*root == 0)
		*root = new;
	else{
		SearchNode *iter = *root;
		while(iter->next)
			iter = iter->next;
		new->prev = iter;
		iter->next = new;
	}
}

void priorityEnqueue(SearchNode **root, SearchNode *new, 
											int (*comp)(void *, void *)){
	//no comparison function given, do a regular enqueue
	if(comp == 0)
		enqueue(root, new);
	else{
		if((*comp)(*root, new) > 0)
			push(root, new);
		else{
			SearchNode *iter = (*root)->next;
			while(iter){
				//insert new searchnode
				if((*comp)(iter, new) > 0){
					(iter->prev)->next = new;
					new->prev = iter->prev;
					iter->prev = new;
					new->next = iter;
					break;
				}
				iter = iter->next;
			}
			if(iter == 0)
				enqueue(root, new);
		}
	}
}

SearchNode *pop(SearchNode **root){
	SearchNode *head = *root;
	if(*root){
		*root = (*root)->next;

		//disconnect head
		head->prev = 0;
		head->next = 0;
	}
	return head;
}

SearchNode *dequeue(SearchNode **root){
	return pop(root);
}

int isExpanded(SearchNode *haystack, SearchNode *needle){
	int found = 0;
	while(haystack->city && !found){
		if(matchCities(haystack->city, needle->city))
			found = 1;
		haystack = haystack->next;
	}
	return found;
}

void printResult(SearchNode *node){
  CityNode *city = node->city;
	printf("=== SEARCH STATISTICS ===\n");
	int cost = city->g_n;
	printf("PATH: ");
	while(city){
		printf("%s <- ", city->name);
		city = city->parent;
	}
	printf("start\nPATH LENGTH: %d\n", cost);
  printf("PATH DEPTH: %d\n", node->depth);

}
//end private interface

SearchNode *initializeEmptySearchNode(){
	SearchNode *sn = malloc(sizeof(SearchNode));
	sn->prev = 0;
	sn->next = 0;
	sn->city = 0;
	sn->priority = 0;
	sn->depth = 0;
	return sn;
}

SearchNode *initializeSearchNode(CityNode *city){
	SearchNode *sn = initializeEmptySearchNode();
	sn->city = city;
	return sn;
}

void printSearchNode(SearchNode *node){
	printf("[%d]SearchNode Contents:\n", node->priority);
	printCityNode(node->city);
}

void printSearchList(SearchNode *root){
	printf("==== Search List ====");
	while(root){
		printSearchNode(root);
		root = root->next;
	}
}

/*
*	Search Algorithms
*/
int runDepthFirst(CityNode *start, CityNode *finish, int max_depth){
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
	SearchNode *initial = initializeSearchNode(start);
	SearchNode *goal = initializeSearchNode(finish);

	int found = 0;
	push(&root, initial);
	while((cur = pop(&root)) && cur->city){
		//test if the current node is the goal state
		if(matchCities(goal->city, cur->city)){
      printResult(cur);
			found = 1;
			break;
		}
		else if(cur->depth < max_depth){
			push(&expanded, cur);
			CityNode *src = cur->city;
			NeighborNode *neighbors = src->neighbors;
			while(neighbors){
				SearchNode *temp = initializeSearchNode(neighbors->dest);
				temp->depth = cur->depth + 1;
				//check for repeat states
				if(!isExpanded(expanded, temp)){
					push(&root, temp);
					(neighbors->dest)->parent = src;
					neighbors->dest->g_n = src->g_n + neighbors->cost;
				}
				neighbors = neighbors->next;
			}
		}
	}
	return found;
}

int runBreadthFirst(CityNode *start, CityNode *finish, 
                                int (*comp)(void *, void *)){
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
	SearchNode *initial = initializeSearchNode(start);
	SearchNode *goal = initializeSearchNode(finish);

	int found = 0;
	priorityEnqueue(&root, initial, comp);
	while((cur = dequeue(&root)) && cur->city){
		//test if the current node is the goal state
		if(matchCities(goal->city, cur->city)){
      printResult(cur);
			found = 1;
			break;
		}
		else{
			push(&expanded, cur);
			CityNode *src = cur->city;
			NeighborNode *neighbors = src->neighbors;
			while(neighbors){
				SearchNode *temp = initializeSearchNode(neighbors->dest);
				temp->depth = cur->depth + 1;
				//check for repeat states
				if(!isExpanded(expanded, temp)){
					priorityEnqueue(&root, temp, comp);
					(neighbors->dest)->parent = src;
					neighbors->dest->g_n = src->g_n + neighbors->cost;
				}
				neighbors = neighbors->next;
			}
		}
	}
	return found;
}

int runIterDeepening(CityNode *start, CityNode *end){
    int found = 0, depth = 0;
    while(!found){
        printf("Max Depth: %d\n", depth);
        found = runDepthFirst(start, end, depth++);
    }
    return found;
}

int runUniformCost(CityNode *start, CityNode *end, 
                                int (*comp)(void *, void *)){
    return runBreadthFirst(start, end, comp);
}

int runBestFirst(CityNode *start, CityNode *end, 
									int (*comp)(void *, void *),
									int (*heur)(void *)){
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
  SearchNode *initial = initializeSearchNode(start);
	SearchNode *goal = initializeSearchNode(end);
	unsigned int found = 0;

	priorityEnqueue(&root, initial, comp);
	while((cur = dequeue(&root)) && cur->city){
		//test if the current node is the goal state
		if(matchCities(goal->city, cur->city)){
      printResult(cur);
			found = 1;
			break;
		}
		else{
			push(&expanded, cur);
      CityNode *src = cur->city;
      NeighborNode *neighbors = src->neighbors;
			while(neighbors){
				SearchNode *temp = initializeSearchNode(neighbors->dest);
				temp->depth = cur->depth + 1;
				//best first search f(n) = h(n)
				//h(n) is hardcoded in this exercise.
				temp->priority = heur(neighbors->dest);
				//check for repeat states
				if(!isExpanded(expanded, temp)){
          (neighbors->dest)->parent = src;
					(neighbors->dest)->g_n = src->g_n + neighbors->cost;
					priorityEnqueue(&root, temp, comp);
				}
				neighbors = neighbors->next;
			}
		}
	}
	return found;
}

int runAStar(CityNode *start, CityNode *end, 
									int (*comp)(void *, void *),
									int (*heur)(void *),
									int bubble_radius){
  SearchNode *initial = initializeSearchNode(start);
	SearchNode *goal = initializeSearchNode(end);
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
	int found = 0;

	priorityEnqueue(&root, initial, comp);
	while((cur = dequeue(&root)) && cur->city){
		//test if the current node is the goal state
		if(matchCities(goal->city, cur->city)){
      printResult(cur);
			found = 1;
			break;
		}
		else if(cur->priority <= bubble_radius){
			push(&expanded, cur);
      CityNode *src = cur->city;
      NeighborNode *neighbors = src->neighbors;
			while(neighbors){
				SearchNode *temp = initializeSearchNode(neighbors->dest);
				temp->depth = cur->depth + 1;
				//best first search f(n) = g(n) + h(n)
				temp->priority = (neighbors->dest)->g_n + heur(neighbors->dest);
				//check for repeat states
				if(!isExpanded(expanded, temp)){
          (neighbors->dest)->parent = src;
					(neighbors->dest)->g_n = src->g_n + neighbors->cost;
					priorityEnqueue(&root, temp, comp);
				}
				neighbors = neighbors->next;
			}
		}
		//remember the min value of the values that exceed the radius this value 
		//will be used next during the next interation of IDAstar.
		//a negative number is used since + nums mean success
		else if( (-1 * cur->priority) < found ){
			found = -1 * cur->priority;
			break;
		}
	}
	//need to clean up memory here
	return found;
}

int runIDAStar(CityNode *start, CityNode *end, 
									int (*comp)(void *, void *),
									int (*heur)(void *),
									int start_bubble){
	int found = -1;
	while(found < 0){
		printf("Bubble radius: %d\n", start_bubble);
		found = runAStar(start, end, comp, heur, start_bubble);
		start_bubble = found * -1;
	}
	return found;
}
