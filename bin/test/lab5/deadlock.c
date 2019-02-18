#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <stddef.h>

#define COLOR "\033"
#define _BOLD "[1"
#define _THIN "[0"
#define _RED ";31m"
#define _BLUE ";34m"
#define _GREEN ";32m"
#define _YELLOW ";33m"
#define _MAGENTA ";35m"
#define _NC "[0m"

void f(ptrdiff_t adr);
void g(ptrdiff_t adr);

pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

void f(ptrdiff_t adr){
          
    pthread_mutex_lock(&mut1);
    printf(COLOR _BOLD _GREEN "Im %p and im locking f() now!" COLOR _NC "\n", adr);
    printf(COLOR _BOLD _YELLOW "And will try launch g()..." COLOR _NC "\n", adr);
    g(adr);
}

void g(ptrdiff_t adr){
          
    pthread_mutex_lock(&mut2);
    printf(COLOR _BOLD _GREEN "Im %p and im locking g() now!" COLOR _NC "\n", adr);
    printf(COLOR _BOLD _YELLOW "And will try launch f()..." COLOR _NC "\n", adr);
    f(adr);
}

int main(void)
{
    pthread_t thread1;
    pthread_t thread2;
    
    printf(COLOR _BOLD _RED "thread1 is %p" COLOR _NC "\n", &thread1);
    printf(COLOR _BOLD _RED "thread2 is %p" COLOR _NC "\n", &thread2);
    
    pthread_create(&thread1, NULL, f, (void *)&thread1);
    pthread_create(&thread2, NULL, g, (void *)&thread2);
    
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    
    printf("hello, world\n");
}