#include "find_min_max.h"

#include <limits.h>
#include "stdlib.h"
#include "string.h"

#include "utils.c"
#include "stdio.h"
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
    
    for(int i = 0; i < end-begin; i++)
        printf("{%i} ",localArr[i]);
        printf("\n");
        
    qsort(localArr, end-begin, sizeof(int), Compare);
    
    for(int i = 0; i < end-begin; i++)
        printf("[%i] ",localArr[i]);

    min_max.min = localArr[0];
    min_max.max = localArr[end-begin-1];
  // your code here
  return min_max;
}


void main(){
    int arr[15];
    GenerateArray(arr, COUNTOF(arr), 9919);
    
    for(int i = 0; i < COUNTOF(arr); i++)
        printf("%i ",arr[i]%21), arr[i]%=21;
        printf("\n");
        
    struct MinMax mm = GetMinMax(arr,5,9);
    printf("\nmin %i max %i\n",mm.min, mm.max);
    
}