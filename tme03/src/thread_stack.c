#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define STACK_SIZE_MAX 100

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
char stack[STACK_SIZE_MAX];
int stack_size = 0;

void Push(char c){
    pthread_mutex_lock(&lock);
    if(stack_size == STACK_SIZE_MAX )
        pthread_cond_wait(&full,&lock);
    stack[stack_size++] = c;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&lock);
}

char Pop(){
    char c;
    pthread_mutex_lock(&lock);
    if(stack_size == 0)
        pthread_cond_wait(&empty,&lock);
    c = stack[--stack_size];
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&lock);
    return c;
}
