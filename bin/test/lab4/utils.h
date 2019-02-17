#ifndef UTILS_H
#define UTILS_H


struct __attribute__((packed)) MinMax {
  int min;
  int max;
};

void GenerateArray(int *array, unsigned int array_size, unsigned int seed);

#endif
