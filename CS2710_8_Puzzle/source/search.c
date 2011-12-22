/******************************************************************************
*	search.c
*
*	Name: Cha Li
*	Date: 09/14/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "search.h"

//statistics
static unsigned int STAT_FRINGE = 0;
static unsigned int STAT_GENERATED = 0;
static unsigned int STAT_EXPANDED = 0;
static unsigned int STAT_DEPTH = 0;

//private interfaace for data structure manipulation
void push(SearchNode **, SearchNode *);
void enqueue(SearchNode **, SearchNode *);
void priorityEnqueue(SearchNode **,	SearchNode *, 
											int (*comp)(void *, void *));
SearchNode *pop(SearchNode **);
SearchNode *dequeue(SearchNode **);
int isExpanded(SearchNode *, SearchNode *);
void clearSearchList(SearchNode **);
void resetStatistics(void);
void printStatistics(void);
void printResult(SearchNode *);

/*
*	function definitions
*/
SearchNode *initializeEmptySearchNode(){
	SearchNode *node = malloc(sizeof(SearchNode));
	node->prev = 0;
	node->next = 0;
	node->data = 0;
	node->priority = 0;
	node->depth = 0;
	return node;
}

SearchNode *initializeSearchNode(EightNode *data){
	SearchNode *node = initializeEmptySearchNode();
	node->data = data;
	return node;
}

void printSearchNode(SearchNode *node){
	printf("[%d]SearchNode Contents\n", node->priority);
	printEightNode(node->data);	
}

void printSearchList(SearchNode *root){
	printf("==== Search List ====\n");
	while(root){
		printSearchNode(root);
		root = root->next;
	}
}

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

//TODO: implement as heap -> logarithmic search time instead of linear
//FIXME: clean up this function 
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
	while(haystack->data && !found){
		if(isMatch(haystack->data, needle->data))
			found = 1;
		haystack = haystack->next;
	}
	return found;
}

void resetStatistics(){
	STAT_FRINGE = 0;
	STAT_EXPANDED = 0;
	STAT_DEPTH = 0;
}

void printStatistics(){
	printf("=== SEARCH STATISTICS ===\n");
	printf("MAX FRINGE LENGTH: %d\n", STAT_FRINGE);
	printf("NODES EXPANDED(explored): %d\n", STAT_EXPANDED);
	printf("GOAL DEPTH: %d\n", STAT_DEPTH);
}

void printResult(SearchNode *answer){
  STAT_DEPTH = answer->depth;
  printStatistics();
  printf("Solution (backwards):\n");
  answer->data->state->name = "goal state";
  printPath(answer->data);
}

/*
* the search algorithms
*/
int runDepthFirst(SearchNode *initial, SearchNode *goal, unsigned int max_depth){
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
	unsigned int found = 0, q_len = 0;
	resetStatistics();

	push(&root, initial);
	++q_len;
	while((cur = pop(&root)) && cur->data){
		--q_len;
		//test if the current node is the goal state
		if(isMatch(goal->data, cur->data)){
      printResult(cur);
			found = 1;
			break;
		}
		else if(cur->depth < max_depth){
			push(&expanded, cur);
			++STAT_EXPANDED;
			EightNode **children = expandEightNode(cur->data);
			int i = 0;
			while(children[i]){
				SearchNode *temp = initializeSearchNode(children[i]);
				temp->depth = cur->depth + 1;
        children[i++]->parent = cur->data;
				//check for repeat states
				if(!isExpanded(expanded, temp)){
					push(&root, temp);
					++q_len;
					++STAT_GENERATED;
				}
			}
			if(q_len > STAT_FRINGE)
				STAT_FRINGE = q_len;
		}
	}
	//need to clean up memory here
	return found;
}

int runBreadthFirst(SearchNode *initial, SearchNode *goal, 
										int (*comp)(void *, void *)){
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
	unsigned int found = 0, q_len = 0;
	resetStatistics();

	priorityEnqueue(&root, initial, comp);
	++q_len;
	while((cur = dequeue(&root)) && cur->data){
		--q_len;
		//test if the current node is the goal state
		if(isMatch(goal->data, cur->data)){
      printResult(cur);
			found = 1;
			break;
		}
		else{
			push(&expanded, cur);
			++STAT_EXPANDED;
			EightNode **children = expandEightNode(cur->data);
			int i = 0;
			while(children[i]){
				SearchNode *temp = initializeSearchNode(children[i]);
				temp->depth = cur->depth + 1;
        children[i]->parent = cur->data;
				temp->priority = children[i++]->e_n;
				//check for repeat states
				if(!isExpanded(expanded, temp)){
					priorityEnqueue(&root, temp, comp);
					++q_len;
					++STAT_GENERATED;
				}
			}
			if(q_len > STAT_FRINGE)
				STAT_FRINGE = q_len;
		}
	}
	//need to clean up memory here
	return found;
}

int runIterDeepening(SearchNode *initial, SearchNode *goal, unsigned int max){
	int i, found = 0;
	for(i = 0; i < max && !found; i++){
		printf("Max Depth: %d\n", i);
		found = runDepthFirst(initial, goal, i);
	}
	return found;
}

int runUniformCost(SearchNode *initial, SearchNode *goal, 
										int (*comp)(void *, void *)){
	return runBreadthFirst(initial, goal, comp);
}


int runBestFirst(SearchNode *initial, SearchNode *goal,
									int (*comp)(void *, void *), 
									int (*heur)(void *, void *)){
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
	unsigned int found = 0, q_len = 0;
	resetStatistics();

	priorityEnqueue(&root, initial, comp);
	++q_len;
	while((cur = dequeue(&root)) && cur->data){
		--q_len;
		//test if the current node is the goal state
		if(isMatch(goal->data, cur->data)){
      printResult(cur);
			found = 1;
			break;
		}
		else{
			push(&expanded, cur);
			++STAT_EXPANDED;
			EightNode **children = expandEightNode(cur->data);
			int i = 0;
			while(children[i]){
				SearchNode *temp = initializeSearchNode(children[i]);
				temp->depth = cur->depth + 1;
				//best first search f(n) = h(n)
				//g(n) will just be the current depth of the node
				children[i]->h_n = heur(goal->data, children[i]);
				children[i]->parent = cur->data;
				temp->priority = heur(goal->data, children[i++]);
				//check for repeat states
				if(!isExpanded(expanded, temp)){
					priorityEnqueue(&root, temp, comp);
					++q_len;
					++STAT_GENERATED;
				}
			}
			if(q_len > STAT_FRINGE)
				STAT_FRINGE = q_len;
		}
	}
	//need to clean up memory here
	return found;
}


int runAStar(SearchNode *initial, SearchNode *goal, 
							int (*comp)(void *, void*), 
							int (*heur)(void *, void*), 
							int radius){
	SearchNode *cur, *root = 0, *expanded = initializeEmptySearchNode();
	unsigned int q_len = 0;
	int found = 0;
	resetStatistics();

	priorityEnqueue(&root, initial, comp);
	++q_len;
	while((cur = dequeue(&root)) && cur->data){
		--q_len;
		//test if the current node is the goal state
		if(isMatch(goal->data, cur->data)){
      printResult(cur);
			found = 1;
			break;
		}
		else if(cur->priority <= radius){
			push(&expanded, cur);
			++STAT_EXPANDED;
			EightNode **children = expandEightNode(cur->data);
			int i = 0;
			while(children[i]){
				SearchNode *temp = initializeSearchNode(children[i]);
				temp->depth = cur->depth + 1;
				//calculate some f(n) = g(n) + h(n) here
				//g(n) will just be the current depth of the node
				children[i]->g_n = temp->depth;
				children[i]->h_n = heur(goal->data, children[i]);
				children[i]->parent = cur->data;
				temp->priority = temp->depth + heur(goal->data, children[i++]);
				//check for repeat states
				if(!isExpanded(expanded, temp)){
					priorityEnqueue(&root, temp, comp);
					++q_len;
					++STAT_GENERATED;
				}
			}

			if(q_len > STAT_FRINGE)
				STAT_FRINGE = q_len;
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

int runIDAStar(SearchNode *initial, SearchNode *goal, 
								int (*comp)(void *, void*), 
								int (*heur)(void *, void*), 
								int start_radius){
	int found = -1;
	while(found < 0){
		printf("Bubble radius: %d\n", start_radius);
		initial->next = 0;
		found = runAStar(initial, goal, comp, heur, start_radius);
		start_radius = found * -1;

	}
	
	return found;
}

