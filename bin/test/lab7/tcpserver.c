#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>
#define LEN(X) (sizeof(X)/sizeof(X[0]))

#define SADDR struct sockaddr

#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _NC "[0m"

int main(int argc, char **argv) {


    
    pid_t child_pid[4];
    
  int BUFSIZE = -1;
  int SERV_PORT = -1;
  
  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                      {"serv_port", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "",options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            BUFSIZE = atoi(optarg);
            if (BUFSIZE <= 0) {
                printf("BUFSIZE must be a positive number\n");
                return 1;
              }
            break;
            
            case 1:
            SERV_PORT = atoi(optarg);
            if (SERV_PORT <= 0) {
                printf("SERV_PORT must be a positive number\n");
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
  if (BUFSIZE == -1 || SERV_PORT == -1) {
    printf("Usage: %s --bufsize \"buffer_size\" --serv_port \"serv_port\"\n",
           argv[0]);
    return 1;
  }  
    
    pid_t currentPID;
    int i = 0;
    char ports[LEN(child_pid)][100];
    
    for( ; i < LEN(child_pid); i++){
        currentPID = fork();
        child_pid[i] = currentPID;
        if(currentPID)
            sprintf(ports[i],"%d",SERV_PORT);
        if(!currentPID)
            break;
    }
        
  printf(COLOR _BOLD _RED "\n\tPARENT: %d, this->PID: %d, CHILD: %s\n" COLOR _NC, getppid(), getpid(), currentPID ? "no" : "yes");
  //PORT = pid now

  if(!currentPID)
      SERV_PORT = getpid(),sleep(3);

  printf("SERV_PORT: %i\n",SERV_PORT);
    
  const size_t kSize = sizeof(struct sockaddr_in);

  int lfd, cfd;
  int nread;
  char buf[BUFSIZE];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, kSize);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);


  if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(lfd, 5) < 0) {
    perror("listen");
    exit(1);
  }


  while (1) {
    unsigned int clilen = kSize;

    if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {

      perror("accept");
      exit(1);
    }
     
    nread = read(cfd, buf, BUFSIZE);
   
    printf("nread: %i\n",nread);
    while (nread > 0) {
        printf(COLOR _BOLD _GREEN "\n\tMessage %s recieved, this->PID: %d, CHILD: %s\n" COLOR _NC, buf, getpid(), currentPID ? "no" : "yes");
        
            sleep(5);
      if(!currentPID)
        sleep(5),printf(COLOR _BOLD _YELLOW "\n\tPARENT: %d, this->PID: %d, MESSAGE:%s\n" COLOR _NC, getppid(), getpid(), buf);//write(1, &buf, nread);
      if(currentPID){
            sleep(1);
            execl("tcpclient", "tcpclient", "--bufsize", "100", "--serv_port", ports[rand()%4], "--addr", "127.0.0.1", "--message", buf, NULL); 
        }

    }
 
    if (nread == -1) {
      perror("read");
      exit(1);
    }
    close(cfd);

  }
}
