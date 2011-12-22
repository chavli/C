/******************************************************************************
*	city.h
*
*	Name: Cha Li
*	Date: 09/19/2011
*	Class:
*
*	OS:
*	Kernel:
******************************************************************************/

typedef struct city CityNode;
typedef struct neighbor NeighborNode;

struct city{
	char *name;
	NeighborNode *neighbors;
	CityNode *next;		//used for the adj list
	CityNode *parent;	//used to display the route to goal
	int g_n;
	int h_n;
};

struct neighbor{
	NeighborNode *next;
	CityNode *dest;
	int cost;
};


CityNode *initializeEmptyCityNode(void);
CityNode *initializeCityNode(char *, int);
void freeCityNode(CityNode *);

NeighborNode *initializeEmptyNeighborNode(void);
NeighborNode *initializeNeighborNode(CityNode *, int);
void freeNeighborNode(NeighborNode *);

void addNeighbor(CityNode **, NeighborNode *);
void addCity(CityNode **, CityNode *);
int matchCities(CityNode *, CityNode *);
CityNode *getCityNode(CityNode *, char *);
NeighborNode *getNeighborNode(CityNode *, char *);


void printCityNode(CityNode *);
void printCityList(CityNode *);


