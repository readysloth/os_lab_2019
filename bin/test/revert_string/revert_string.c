#include "revert_string.h"
#include "swap.h"

void RevertString(char *str)
{
	// your code here
	char* endPtr = str;
	while(*(endPtr+1) != '\0')
	   endPtr++;
	
	for(int i = 0; i < (endPtr-str)/2; i++)
	   Swap(endPtr-i,str+i);
}

