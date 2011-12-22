#include "list.h"
#include "pool.h"

#ifndef UTIL_H
#define UTIL_H

#define MAX_BYTES sizeof(long double)

typedef unsigned long int bytes_t;


//public interface
int memview(void *, bytes_t);

#endif
