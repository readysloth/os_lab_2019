#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include "libFact.h"

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

uint64_t Factorial(const struct FactorialArgs *args) {
  uint64_t ans = 1;
  for(uint64_t i = 0; args->begin + i <= args->end; i++){        
        ans *= (args->begin + i);
        //ans %= args->mod;
        
        printf(COLOR _BOLD _YELLOW "begin+i: %llu end: %llu mod: %llu \n\t ans: %llu" COLOR _NC "\n",\
                                                        args->begin + i,\
                                                        args->end,\
                                                        args->mod,\
                                                        ans);
                                                        
                                                       
        printf(COLOR _THIN _YELLOW "ans: %llu" COLOR _NC "\n", ans);
    }
    
  return ans;
}

void *ThreadFactorial(void *args) {
  struct FactorialArgs *fargs = (struct FactorialArgs *)args;
  return (void *)(uint64_t *)Factorial(fargs);
}

int main(int argc, char **argv) {
  int tnum = -1;
  int port = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        /*if (port != 20001) {
            printf("port must be 20001\n");
            return 1;
          }*/
        break;
      case 1:
        tnum = atoi(optarg);
        if (tnum < 1) {
            printf("tnum must be >= 1\n");
            return 1;
          }
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Unknown argument\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Using: %s --port \"port_num\" --tnum \"thread_num\"\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    fprintf(stderr, "Can not create server socket!");
    return 1;
  }

////////////////////EVERYTHING UP TO THIS MOMENT IS WORKING

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) {
    fprintf(stderr, "Can not bind to socket!");
    return 1;
  }

  err = listen(server_fd, 128);
  if (err < 0) {
    fprintf(stderr, "Could not listen on socket\n");
    return 1;
  }

  printf("Server listening at %d\n", port);

  while (true) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0) {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    while (true) {
      unsigned int buffer_size = sizeof(uint64_t) * 3;
      char from_client[buffer_size];
      int read = recv(client_fd, from_client, buffer_size, 0);

      if (!read)
        break;
      if (read < 0) {
        fprintf(stderr, "Client read failed\n");
        break;
      }
      if (read < buffer_size) {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }

      pthread_t threads[tnum];

      uint64_t begin = 0;
      uint64_t end = 0;
      uint64_t mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      fprintf(stdout, "Receive: %llu %llu %llu\n", begin, end, mod);

      printf("RECIEVED: [begin: %llu end: %llu mod: %llu]\n", begin, end, mod);
      struct FactorialArgs args[tnum];
      
      uint64_t part;
      uint64_t real_tnum = tnum;
      uint64_t size = end-begin;
      if(size/tnum < 2)
        part = size, real_tnum = 1;
      else
        part = size/tnum;
      
      for (uint32_t i = 0, j = 1; i < real_tnum; i++, j=i+1) {
        // TODO: parallel somehow 
        
            
        //pthread_mutex_lock(&mut);
        
        /*
        args[i].begin = begin*part;
        args[i].end = end;
        args[i].mod = mod;
        */
        args[i].mod = mod;
        if(real_tnum != 1 && j == real_tnum && real_tnum*part <= end){
            args[i].begin = i*part + 1;
            args[i].end = end;
        }
        else
            if(i == 0){
                args[i].begin = begin;
                args[i].end = begin + part;
            }
            else{
                args[i].begin = i*part + 1;
                args[i].end = j*part;
            }
              
        //pthread_mutex_unlock(&mut);

        if (pthread_create(&threads[i], NULL, ThreadFactorial,
                           (void *)&args[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

      uint64_t total = 1;
          
      printf(COLOR _THIN _GREEN "tnum: %llu real_tnum: %llu" COLOR _NC "\n", tnum, real_tnum);
      for (uint32_t i = 0; i < real_tnum; i++) {
               
               ///mycode
        pthread_mutex_lock(&mut);
        uint64_t result = 0;
        pthread_join(threads[i], (void **)&result);
            
        printf("\ttotal in cycle: %llu\n\tresult in cycle: %llu\n", total, result);
        
        if(i == 0)
            total = result % mod;
        else
            total *= result, total %= mod;
        
        
        /*if(real_tnum < tnum)
            for(uint32_t j = 0; j < tnum; j++)
                    total = MultModulo(total, result, mod);
        else
            total = MultModulo(total, result, mod);
            */
        
        pthread_mutex_unlock(&mut);
      }

      printf("Total: %llu\n", total);

      char buffer[sizeof(total)];
      memcpy(buffer, &total, sizeof(total));
      err = send(client_fd, buffer, sizeof(total), 0);
      if (err < 0) {
        fprintf(stderr, "Can't send data to client\n");
        break;
      }
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }

  return 0;
}
