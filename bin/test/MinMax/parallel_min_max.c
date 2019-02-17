#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _NC "[0m"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed must be a positive number\n");
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
            pnum = atoi(optarg);
             if (pnum <= 0) {
                printf("pnum must be a positive number\n");
                return 1;
              }
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  for(int i = 0; i < array_size; i++)
    array[i]%=21, printf("%i ", array[i]);
    printf("\n");
    
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

    int part = array_size/pnum;
    int pipefd[2];
    
    pipe(pipefd);
    
    pid_t currentPID;
    remove("processOut.txt");
    
    
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    currentPID = child_pid;
    if (child_pid >= 0) {
      // successful fork
      active_child_processes++;
      if (child_pid == 0) {
         struct MinMax mm;
         if(i*part < array_size)
             mm = GetMinMax(array + i*part, 
                            0,
                            part);
         else
            mm = GetMinMax(array + array_size - part, 0, part);

            printf(COLOR _BOLD _RED "\n\tPARENT: %d, this->PID: %d, CHILD: %d || min: %i, max: %i\n" COLOR _NC, getppid(), getpid(), currentPID, mm.min, mm.max);
        if (with_files) {
          FILE* fp = fopen("processOut.txt", "a");
          fwrite(&mm, sizeof(struct MinMax), 1, fp);
          fclose(fp);
        } else {
          // use pipe here
          write(pipefd[1],&mm,sizeof(struct MinMax));
          
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    // your code here
    close(pipefd[1]);
    printf(kill(currentPID, SIGKILL) == 0 ? "killed child process\n" : "error while killing\n");
    
    active_child_processes--;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

    waitpid(currentPID);

  for (int i = 0; i < pnum; i++) {
    struct MinMax mm;
    
    if (with_files) {
      // read from files
        FILE* fp = fopen("processOut.txt", "rb");
        printf(COLOR _BOLD _BLUE "\nBYTE FILE POS: %i, FILE POINTER: %p" COLOR _NC, i*sizeof(struct MinMax),*fp);
        fseek(fp, i*sizeof(struct MinMax), SEEK_SET);
        fread(&mm, sizeof(struct MinMax), 1, fp);
        printf(COLOR _BOLD _GREEN "\n[FROM FILE: min:%i  max:%i]" COLOR _NC,mm.min, mm.max);
        fclose(fp);
    } else {
      // read from pipes
      read(pipefd[0], &mm, sizeof(struct MinMax));
      printf(COLOR _BOLD _GREEN "\n[FROM PIPE: min:%i  max:%i]" COLOR _NC,mm.min, mm.max);
    }

    if (mm.min < min_max.min) min_max.min = mm.min;
    if (mm.max > min_max.max) min_max.max = mm.max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf(COLOR _BOLD _YELLOW "\nMin: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n" COLOR _NC, elapsed_time);
  fflush(NULL);
  return 0;
}
