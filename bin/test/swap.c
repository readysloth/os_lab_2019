#include "swap.h"

void Swap(char *left, char *right)
{
	// ваш код здесь
	*left ^= *right;
    *right^= *left;
    *left ^= *right;
}
