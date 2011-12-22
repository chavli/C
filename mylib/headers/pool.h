#include <stdlib.h>


#ifndef POOL_H
#define POOL_H

typedef struct pool pool_t;

struct pool{
  void *pool;
  unsigned int offset;
};

//allocate a pool of memory
pool_t *palloc(unsigned int);

//compare the memory addresses of two pools
int pcompare(pool_t *, pool_t *);

//fetch a piece of memory from the pool to use
void* pfetch(pool_t *, size_t);

//return the size of the pool in bytes
int psize(pool_t *);

//return the amount of free space in the pool in bytes
int pavail(pool_t *);

//free the entire pool. return the amount of bytes freed
int pfree(pool_t *);




//TODO: mark a section of the pool as unused
//void *prelease(void *);

//merge sequential blocks of free space
//void *pdefrag(pool_t *);


#endif


