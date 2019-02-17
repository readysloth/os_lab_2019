#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include <pthread.h>
#include "utils.h"
#include "threadSum.h"

#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _NC "[0m"




void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */

  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"threads_num", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"seed", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "",options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            threads_num = atoi(optarg);
            if (threads_num <= 0) {
                printf("threads_num must be a positive number\n");
                return 1;
              }
            break;
          case 1:
            array_size = atoi(optarg);
             if (array_size <= 0) {
                printf("array_size must be a positive number\n");
                return 1;
              }
            break;
          case 2:
            seed = atoi(optarg);
             if (seed <= 0) {
                printf("seed must be a positive number\n");
                return 1;
              }
            break;
        }

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
    
  }
  
  
  pthread_t threads[threads_num];
  

  /*
   * TODO:
   * your code here
   * Generate array here
   */

  int *array = malloc(sizeof(int) * array_size);

  GenerateArray(array, array_size, seed);


  struct SumArgs args[threads_num];
  int part = array_size/threads_num;
  
  for(int i = 0; i < threads_num; i++)
    args[i].array = array,\
    args[i].begin = part * i,\
    args[i].end = args[i].begin + part;
  
  
  for(int i = 0; i < array_size; i++)
    array[i]%=23, printf("%i ",array[i]); 
  printf("\n");
  
  
  
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args)) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }


  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;


  printf("Elapsed time: %fms\n" COLOR _NC, elapsed_time);
  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}
