#include "find_min_max.h"

#include <limits.h>
#include "stdlib.h"
#include "string.h"
#include "utils.h"
#define  COUNTOF(x) (sizeof(x)/sizeof(int))

int Compare(int* l, int* r){
    return *l-*r;
}

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
    int size = (end-begin)*sizeof(int);
    int* localArr = (int*)alloca(size);
    memcpy(localArr, array + begin, size);
        
    qsort(localArr, end-begin, sizeof(int), Compare);

    min_max.min = localArr[0];
    min_max.max = localArr[end-begin-1];
  // your code here
  return min_max;
}