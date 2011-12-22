/******************************************************************************
*	heuristics.c
*
*	Name: Cha Li
*	Date: 09/19/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "heuristics.h"

//properties of the earth in km
#define RADIUS 6378.00
#define CIRCUM 40075.0
#define PI 3.141592653

int straightLineDistance(CityNode *city){
	return city->h_n;
}

//the world is round, not flat(globe distance)
//see report for explanation
int nonEuclideanDistance(CityNode *city){
	double arclength = (CIRCUM * asin( city->h_n / (2 * RADIUS))) / PI;
	return (int)arclength;
}

//priority = cost, and lower cost is better
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


