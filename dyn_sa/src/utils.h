#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "types.h"

/* #include <iostream> */
/* using namespace std; */


bool isPowerOfTwo(int n);

/**
 * @return the binary logarithm of an integer. 
 */
uint log2 (ulong i);

ullong getChrono();

#endif
