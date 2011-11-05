/******************************************************************************
*	city.c
*
*	Name: Cha Li
*	Date: 09/19/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "city.h"

CityNode *initializeEmptyCityNode(){
	CityNode *cn = malloc(sizeof(CityNode));
	cn->name = "";
	cn->neighbors = 0;
	cn->next = 0;
	cn->parent = 0;
	cn->g_n = 0;
	cn->h_n = 0;
	return cn;
}

CityNode *initializeCityNode(char *name, int h){
	CityNode *cn = initializeEmptyCityNode();
	cn->name = name;
	cn->h_n = h;
	return cn;
}

NeighborNode *initializeEmptyNeighborNode(){
	NeighborNode *nn = malloc(sizeof(NeighborNode));
	nn->next = 0;
	nn->dest = 0;
	nn->cost = 0;
	return nn;
}

NeighborNode *initializeNeighborNode(CityNode *dest, int cost){
	NeighborNode *nn = initializeEmptyNeighborNode();
	nn->dest = dest;
	nn->cost = cost;
	return nn;
}

void addNeighbor(CityNode **source, NeighborNode *dest){
	if(!(*source)->neighbors)
		(*source)->neighbors = dest;
	else{
		NeighborNode *iter = (*source)->neighbors;
		while(iter->next)
			iter = iter->next;
		iter->next = dest;
	}
}

void addCity(CityNode **root, CityNode *new){
	if(*root == 0)
		*root = new;
	else{
		CityNode *iter = *root;
		while(iter->next)
			iter = iter->next;
		iter->next = new;
	}
}

CityNode *getCityNode(CityNode *root, char *name){
	while(root && strcmp(root->name, name) != 0 && (root = root->next));
	return root;
}

NeighborNode *getNeighborNode(CityNode *src, char *name){
	NeighborNode *iter = src->neighbors;
	while(iter && strcmp(iter->dest->name, name) != 0 && (iter = iter->next));
	return iter;
}


int matchCities(CityNode *c1, CityNode *c2){
	return strcmp(c1->name, c2->name) == 0;
}

void printCityNode(CityNode *source){
	NeighborNode *iter = source->neighbors;
	printf("[%p]%s h(n) = %d: ", source, source->name, source->h_n);
	while(iter){
		printf("[%s: %d] ", (iter->dest)->name, iter->cost);
		iter = iter->next;
	}
	printf("\n");
}

void printCityList(CityNode *root){
	printf("=== City Adjacency Data ===\n");
	while(root){
		printCityNode(root);
		root = root->next;
	}
	printf("\n");
}

