#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int globl = 0;

int thread_rand(pthread_mutex_t* arg) {
    pthread_mutex_lock(arg);
    globl += (int) (10 * ((double) rand()) / RAND_MAX);
    pthread_mutex_unlock(arg);
    return 0;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("usage: %s THREAD_COUNT\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int thread_count = atoi(argv[1]);
    pthread_t *t = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    srand(time(NULL));

    for(int64_t i = 0; i < thread_count; ++i) {
        pthread_create(t + i, NULL, (void* (*)(void*)) thread_rand, (void*) &mutex);
    }

    for(int i = 0; i < thread_count; ++i) {
        void* retval;
        pthread_join(t[i], &retval);
    }

    printf("globl = %d\n", globl);

    return EXIT_SUCCESS;
}
