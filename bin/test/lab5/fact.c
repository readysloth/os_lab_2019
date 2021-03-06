#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>

#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _MAGENTA ";35m"
#define _NC "[0m"


pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
typedef struct {
     uint64_t begin;
     uint64_t current;
     uint64_t end;
     uint64_t mod;
    
} Args;

uint64_t fact(Args* arg){
    
    printf(COLOR _BOLD _YELLOW "\tArgs"\
           COLOR _BOLD _MAGENTA " %p"\
           COLOR _BOLD _YELLOW " are {begin: %d current: %d end: %d mod: %d}" COLOR _NC "\n",\
                                    arg,\
                                    arg->begin,\
                                    arg->current,\
                                    arg->end,\
                                    arg->mod);
    if(arg->begin < arg->end){
        
        arg->begin++;
        arg->current *= arg->begin % arg->mod;
        return fact(arg);
    }
    
    //arg->current *= arg->begin+1;
    //arg->current %= arg->mod;
    printf("\n");
    return arg->current;
}




int main(int argc, char **argv)
{
    int k = -1, pnum = -1, mod = -1;
    
    while (true) {
    int current_optind = optind ? optind : 1;
     static struct option options[] = {{"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"k", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    
    int c = getopt_long(argc, argv, "",options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum must be a positive number\n");
                return 1;
              }
            break;
          case 1:
            mod = atoi(optarg);
             if (mod <= 0) {
                printf("mod must be a positive number\n");
                return 1;
              }
            break;
            
          case 2:
            k = atoi(optarg);
                 if (k <= 0) {
                    printf("k must be a positive number\n");
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
  if (mod == -1 || k == -1 || pnum == -1) {
    printf("Usage: %s -k \"num\" --pnum \"num\" --mod \"num\"\n",
           argv[0]);
    return 1;
  }
  
  
  pthread_t threads[pnum];
  uint64_t part;
  
    printf(COLOR _BOLD _BLUE"BEFORE: [part: %d k: %d pnum: %d]\n" COLOR _NC "\n", k/pnum, k, pnum);
  
  if(k/pnum < 2)
    part = k,pnum = 1;
  else
    part = k/pnum;
    
  printf(COLOR _BOLD _BLUE"AFTER: [part: %d k: %d pnum: %d]\n" COLOR _NC "\n", part, k, pnum);
  
  Args args[pnum];
  for (uint32_t i = 0, j = 0; i < pnum; i++, j = i + 1) {
    printf("i: %d\n",i);
    
    
    if(pnum != 1 && j == pnum && pnum*part <= k)
        args[i] = (Args){i*part + 1, i*part + 1, k, mod};
    else
        if(i == 0)
            args[i] = (Args){1, 1, part, mod};
        else
            args[i] = (Args){i*part + 1, i*part + 1, j*part, mod};
        
    printf(COLOR _THIN _YELLOW "\tArgs"\
           COLOR _THIN _MAGENTA "[%p]"\
           COLOR _THIN _YELLOW  " are {begin: %d current: %d end: %d mod: %d}" COLOR _NC "\n",\
                                    &args[i],\
                                    args[i].begin,\
                                    args[i].current,\
                                    args[i].end,\
                                    args[i].mod);
                                    
    printf(COLOR _THIN _GREEN "values are {i: %d j: %d current: %d end: %d mod: %d}" COLOR _NC "\n",\
                                    i,\
                                    j,\
                                    i*part,\
                                    (i+1)*part,\
                                    mod);
                                    
    if (pthread_create(&threads[i], NULL, fact, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }
  
  uint64_t result = 1;
  for (uint32_t i = 0; i < pnum; i++) {
     uint64_t res = 0;
    pthread_join(threads[i], (void **)&res);
    printf(COLOR _THIN _RED "res: %d" COLOR _NC "\n",\
                                    res);
                                    
    pthread_mutex_lock(&mut);
    result *= res;
    result %= mod;
    pthread_mutex_unlock(&mut);
  }
  
  printf(COLOR _BOLD _GREEN "Result is %d" COLOR _NC "\n",result);
  return 0;
}