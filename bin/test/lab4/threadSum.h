#ifndef VARIABLE
#define VARIABLE

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args);
#endif