#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>


#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _NC "[0m"

int main(void)
{
    int size = 100;
    pid_t children[size];
    for(int i = 0; i < size; i++){
        pid_t child = fork();
        if(child == 0)
            printf(COLOR _BOLD _RED "le Zombie has arrived" COLOR _NC "\n"),exit(0);
        else
            children[i] = child, printf(COLOR _BOLD _GREEN"Child created: %i" COLOR _NC "\n",child);
    }
    sleep(10);
    
    for(int i = 0; i < size; i++){
        wait(NULL);
        printf(COLOR _BOLD _YELLOW "Collecting zombie" COLOR _NC "\n");
    }
    printf("\n");
    return 0;
}