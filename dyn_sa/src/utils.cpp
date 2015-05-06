#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
using namespace std;

bool isPowerOfTwo(int n) {
  return (n) && !(n & (n - 1)); 
}


/**
 * @return the binary logarithm of an integer. 
 */
uint log2 (ulong i) {
  uint size_in_bits = 8*sizeof(i);
  ulong res;
  bool match=0;
  uint min=0, max=size_in_bits-1, shift=(min+max)/2;

  /* binary searching the highest bit */
  match=0;
  res = i;
  while (! match) {
    switch (res >> shift) {
    case 0:  max=shift-1; shift = (max+min)/2; break;
    case 1:  match=1; break;
    default:min = shift+1; shift = (max+min)/2;
    } 
  }
  return shift;
}

ullong getChrono() {
	struct timeval time;
	time_t sec;
	suseconds_t usec;

	if (gettimeofday(&time,NULL) != -1) {
		sec = time.tv_sec;
		usec = time.tv_usec;
		return (ullong)sec*1000000+usec;
	}
	return 0;

  /*struct rusage rusage;
  getrusage(0,&rusage);
  return (ullong)rusage.ru_utime.tv_sec*1000000+rusage.ru_utime.tv_usec;
	*/
}

