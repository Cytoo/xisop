#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <thread_stack.h>

pthread_mutex_t readlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writelock = PTHREAD_MUTEX_INITIALIZER;

void* Producteur() {
    int c;
    for(;;){
        pthread_mutex_lock(&readlock);
        c = getchar();
        if ( c == EOF ){
            pthread_mutex_unlock(&readlock);
            return NULL;
        }
        Push(c);
        pthread_mutex_unlock(&readlock);
    }
    return NULL;
}

void* Consommateur() {
    char c;
    for (;;) {
        pthread_mutex_lock(&writelock);
        c = Pop();
        putchar(c);
        fflush(stdout);
        pthread_mutex_unlock(&writelock);
    }
    return NULL;
}


int main (int argc, char ** argv){
    int i,nbp,nbc;
    if(argc < 3){
        printf("usage: %s NB_PROD NB_CONS\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nbp = atoi(argv[1]);
    nbc = atoi(argv[2]);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t prod;
    pthread_t cons;

    for(i=0;i<nbp;i++){
        pthread_create(&prod, &attr,
                (void *(*)(void *))Producteur, NULL);
    }

    for(i=0;i<nbc;i++){
        pthread_create(&cons, &attr,
                (void *(*)(void *))Consommateur, NULL);
    }

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
