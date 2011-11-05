/******************************************************************************
*	romanian.c
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
#include <limits.h>
#include "heuristics.h"


int main(int argc, char *argv[]){
	if(argc != 4)
		printf("usage: ./program city_file edge_file h_file\n");
	else{
		FILE *cities = fopen(argv[1], "r");
		FILE *edges = fopen(argv[2], "r");
		FILE *hvals =	fopen(argv[3], "r");
		
		//this is our adjacency list
		CityNode *root = 0, *new_city;
		NeighborNode *new_neighbor;

		char line[128];
		char *temp, *temp2;

		//set up all the cities
		while(fgets(line, 128, cities)){
			temp = strtok(line, ":");
			char *city = malloc(16 * sizeof(char));
			strcpy(city, temp);
			new_city = initializeCityNode(city, 0);
			addCity(&root, new_city);
		}
		rewind(cities);
		
		//set up neighbors
		while(fgets(line, 128, cities)){
			temp = strtok(line, ":");
			char *neighbor = malloc(16 * sizeof(char));
			while(temp2 = strtok(NULL, ",")){
				strcpy(neighbor, temp2);
				if(strcmp(neighbor, "\n") != 0){
					CityNode *dest = getCityNode(root, neighbor);
					CityNode *src = getCityNode(root, temp);
					new_neighbor = initializeNeighborNode(dest, 0);
					addNeighbor(&src, new_neighbor);
				}
			}
		}
		
		fclose(cities);

		//set up edge costs
		int cost;
		char from[16], to[16], line2[128];
		while(fgets(line2, 128, edges)){
			sscanf(line2, "%s %s %d", from, to, &cost);
			CityNode *src = getCityNode(root, from);
			NeighborNode *dest = getNeighborNode(src, to);
			dest->cost = cost;
		}
		fclose(edges);
		
		//set up h(n)
		while(fgets(line, 128, hvals)){
			sscanf(line, "%s %d", from, &cost);
			CityNode *src = getCityNode(root, from);
			src->h_n = cost;
		}
		fclose(hvals);
		
		//the adjacency list
		printCityList(root);
		
		CityNode *start = getCityNode(root, "arad");
		CityNode *end = getCityNode(root, "buch");

		printf("\nRunning Depth First Search...\n");
		runDepthFirst(start, end, INT_MAX);

		printf("\nRunning Iterative Deepening Search...\n");
		runIterDeepening(start, end);

		printf("\nRunning Breadth First Search...\n");
		runBreadthFirst(start, end, 0);

		printf("\nRunning Uniform Cost Search...\n");
		runUniformCost(start, end, (int (*)(void *, void *))basicComp);

		printf("\nRunning Best First Search with straightLineDistance...\n");
		runBestFirst(start, end, 
										(int (*)(void *, void *))basicComp,
										(int (*)(void *))straightLineDistance);

		printf("\nRunning Best First Search with nonEuclideanDistance...\n");
		runBestFirst(start, end, 
										(int (*)(void *, void *))basicComp,
										(int (*)(void *))nonEuclideanDistance);

		printf("\nRunning A* Search with straightLineDistance...\n");
		runAStar(start, end, 
										(int (*)(void *, void *))basicComp,
										(int (*)(void *))straightLineDistance,
										INT_MAX);

		printf("\nRunning A* Search with nonEuclideanDistance...\n");
		runAStar(start, end, 
										(int (*)(void *, void *))basicComp,
										(int (*)(void *))nonEuclideanDistance,
										INT_MAX);
		
		printf("\nRunning IDA* Search  with straightLineDistance...\n");
		runIDAStar(start, end, 
										(int (*)(void *, void *))basicComp,
										(int (*)(void *))straightLineDistance,
										0);

		printf("\nRunning IDA* Search  with nonEuclideanDistance...\n");
		runIDAStar(start, end, 
										(int (*)(void *, void *))basicComp,
										(int (*)(void *))nonEuclideanDistance,
										0);
	}
	return 0;
}

