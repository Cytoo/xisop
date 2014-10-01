#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int thread_rand(void* arg) {
    int64_t ord = (int64_t) arg;
    printf("Thread. Order: %ld, tid: %d\n", ord, (int) pthread_self());
    return ord << 1;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("usage: %s THREAD_COUNT\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int thread_count = atoi(argv[1]);
    pthread_t *t = malloc(thread_count * sizeof(pthread_t));
    for(int64_t i = 0; i < thread_count; ++i) {
        pthread_create(t + i, NULL, (void* (*)(void*)) thread_rand, (void*) i);
    }

    for(int i = 0; i < thread_count; ++i) {
        void* retval;
        pthread_join(t[i], &retval);
        printf("Thread %ld terminated\n", (int64_t) retval);
    }

    return EXIT_SUCCESS;
}
