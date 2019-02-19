#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>


#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _MAGENTA ";35m"
#define _NC "[0m"

char** Load(char* filename);

struct Server {
  char ip[255];
  int port;
};

typedef struct {
    struct sockaddr_in server;
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
} Mail;


uint64_t ConnectAndSR(Mail* mail){
    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
      fprintf(stderr, "Socket creation failed!\n");
      exit(1);
    }

    if (connect(sck, (struct sockaddr *)&(mail->server), sizeof(struct sockaddr)) < 0) {
      fprintf(stderr, "Connection failed\n");
      exit(1);
    }


    char task[sizeof(uint64_t) * 3];
    memcpy(task, &mail->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &mail->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mail->mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
      fprintf(stderr, "Send failed\n");
      exit(1);
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
      fprintf(stderr, "Recieve failed\n");
      exit(1);
    }

    // TODO: from one server
    // unite results
    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));
    printf("answer: %llu\n", answer);

    close(sck);
    return answer;
}


bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255
  char** server_port_list;
  int serverCNT = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
         if (k <= 0) {
            printf("k must be a positive number\n");
            return 1;
          }
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        if (mod <= 0) {
            printf("mod must be a positive number\n");
            return 1;
          }
        break;
      case 2:
        // TODO: your code here
        
        ///get addresses from file
        memcpy(servers, optarg, strlen(optarg));
        server_port_list = Load(servers);
        char** s_p_ptr = server_port_list;
        while(*s_p_ptr != NULL)
            printf(COLOR _THIN _YELLOW "SERVER from file: %s" COLOR _NC "\n", *s_p_ptr++);
        serverCNT = s_p_ptr - server_port_list;
        
        break;
      default:
        printf("Index %llu is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  // TODO: for one server here, rewrite with servers from file
  unsigned int servers_num = serverCNT;
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  // TODO: delete this and parallel work between servers
  
  for(int i = 0; i < servers_num; i++){
      char* addr_part = strtok(server_port_list[i],":");
      
      ///separate ip and port
      strcpy(to[i].ip, addr_part);
      addr_part = strtok(NULL,":");
      to[i].port = atoi(addr_part);
      printf(COLOR _BOLD _YELLOW "SERVER: [ip: %10s port: %7i]" COLOR _NC "\n", to[i].ip, to[i].port);
  }

////////////////////EVERYTHING UP TO THIS MOMENT IS WORKING


    uint64_t part;

    if(k/servers_num < 2)
        part = k,servers_num = 1;
      else
        part = k/servers_num;
        
    pthread_t threads[servers_num];
    
    Mail mail2server[servers_num];
    uint64_t partial_res[servers_num];
    
    
  // TODO: work continiously, rewrite to make parallel
  for (int i = 0 , j = 1; i < servers_num; i++, j = i+1) {
    struct hostent *hostname = gethostbyname(to[i].ip);
    if (hostname == NULL) {
      fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
      exit(1);  
    }

    mail2server[i].server.sin_family = AF_INET;
    mail2server[i].server.sin_port = htons(to[i].port);
    mail2server[i].server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);
    if(servers_num != 1 && j == servers_num && servers_num*part <= k){
        mail2server[i].begin = i*part + 1;
        mail2server[i].end = k;
    }
    else
        if(i == 0){
            mail2server[i].begin = 1;
            mail2server[i].end = part;
        }
        else{
            mail2server[i].begin = i*part + 1;
            mail2server[i].end = j*part;
        }

    mail2server[i].mod = mod;
    
    printf(COLOR _BOLD _BLUE "Values are {begin: %llu end: %llu mod: %llu}" COLOR _NC "\n",\
                                    mail2server[i].begin,\
                                    mail2server[i].end,\
                                    mail2server[i].mod);
    
    
    pthread_create(&threads[i], NULL, ConnectAndSR, (void *)&mail2server[i]);
    
    pthread_join(threads[i], (void **)&partial_res[i]);
    printf(COLOR _THIN _RED "Partial res: %llu" COLOR _NC "\n",\
                                    partial_res[i]);
  }
  free(to);
  uint64_t real_result = 1;
  
  for(int i = 0; i < servers_num; i++)
    real_result *= partial_res[i];

  printf(COLOR _BOLD _GREEN "Final result: %llu" COLOR _NC "\n",\
                                    real_result);
  return 0;
}


char** Load(char* filename){
  /*Функция читает файл и возвращает массив строк, заканчивающийся NULL*/
  FILE* file = fopen(filename, "r");

  if (file == NULL){
    printf("\nError occured while reading\n");
    return NULL;
  }


  fseek(file, 0, SEEK_END);
  int FileSize = ftell(file);    //узнаем размер файла
  fseek(file, 0, SEEK_SET);

  char* txt = (char*)malloc(sizeof(char)*FileSize + 1);
  FileSize = fread(txt, sizeof(char), FileSize, file);    //Присваиваем FileSize количество реально прочитанных байт
  txt[FileSize] = '\0';

  void* search_ptr = txt;
  char** strings = (char**)calloc(1000, sizeof(char*));          //выделим побольше памяти, потом ее урежем realloc'ом
  int cnt = 0;
  do{                                    //считаем количество строк
    int length = 0;
    if (memchr(search_ptr, '\n', FileSize*sizeof(char)) != NULL)    //длина строки
      length = (intptr_t)memchr(search_ptr, '\n', FileSize*sizeof(char)) - (intptr_t)search_ptr + 1;
    else
      length = strlen((char*)search_ptr) + 1;

    strings[cnt] = (char*)calloc(length, sizeof(char));
    memcpy(strings[cnt], search_ptr, length - 1);            //копируем -1 символ, чтобы сохранить \0 в конце строки
    cnt++;
  } while ((search_ptr = (void*)((intptr_t)memchr(search_ptr, '\n', FileSize*sizeof(char)) + (intptr_t)sizeof(char))) != (void*)1);

  strings = (char**)realloc(strings, (cnt + 1)*sizeof(char*));
  fclose(file);
  return strings;
}
