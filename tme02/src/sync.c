#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int globl = 0;
int count = 0;
pthread_mutex_t lock;

int thread_rand(pthread_mutex_t* arg) {
    pthread_mutex_lock(arg);
    globl += (int) (10 * ((double) rand()) / RAND_MAX);
    --count;
    if(count == 0) {
        pthread_mutex_unlock(&lock);
    }
    pthread_mutex_unlock(arg);
    return 0;
}

int thread_print(void* arg) {
    (void) arg;
    pthread_mutex_lock(&lock);
    printf("globl = %d\n", globl);
    pthread_mutex_unlock(&lock);
    return 0;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("usage: %s THREAD_COUNT\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int thread_count = atoi(argv[1]);
    pthread_t *t = malloc(thread_count * sizeof(pthread_t));
    pthread_t print;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    pthread_create(&print, NULL, (void* (*)(void*)) thread_print, (void*) NULL);
    count = thread_count;
    srand(time(NULL));

    for(int64_t i = 0; i < thread_count; ++i) {
        pthread_create(t + i, NULL, (void* (*)(void*)) thread_rand, (void*) &mutex);
    }

    pthread_join(print, NULL);

    return EXIT_SUCCESS;
}
