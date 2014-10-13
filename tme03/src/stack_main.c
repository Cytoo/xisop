#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <thread_stack.h>

void* Producteur() {
    int c;
    while ((c = getchar()) != EOF) {
        Push(c);
    }
    return NULL;
}

void* Consommateur() {
    for (;;) {
        putchar(Pop());
        fflush(stdout);
    }
    return NULL;
}


int main (){
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t prod;
    pthread_t cons;

    pthread_create(&prod, &attr,
            (void *(*)(void *))Producteur, NULL);

    pthread_create(&cons, &attr,
            (void *(*)(void *))Consommateur, NULL);

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
