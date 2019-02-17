#include "revert_string.h"
#include "swap.h"

void RevertString(char *str)
{
	// your code here
	char* endPtr = str;
	while(*(endPtr+1) != '\0')
	   endPtr++;
	
	int boundary = (endPtr-str)%2 == 0 ? (endPtr-str)/2 : (endPtr-str)/2+1; 
	for(int i = 0; i < boundary; i++)
	   Swap(endPtr-i,str+i);
}

