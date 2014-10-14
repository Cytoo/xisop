#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;
pthread_mutex_t cond_mutex;
pthread_cond_t cond;

void handler(int sig) { printf("SIGINT %d\n", sig); (void) sig; }

void* thread_main(void* n) {
    if(n != 0) {
        pthread_t t;
        pthread_create(&t, NULL, thread_main, (void*) ((uintptr_t) n - 1));
        pthread_mutex_lock(&cond_mutex);
        pthread_cond_wait(&cond, &cond_mutex);
        pthread_mutex_unlock(&cond_mutex);
        pthread_join(t, NULL);
    } else {
        pthread_mutex_unlock(&lock);
        pthread_cond_wait(&cond, &cond_mutex);
    }
    
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("usage: %s NB_THREADS\n", argv[0]);
        return EXIT_FAILURE;
    }

    uintptr_t NB_THREADS = atoi(argv[1]);

    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, NULL);

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&cond_mutex, NULL);
    pthread_mutex_lock(&lock);

    pthread_t t;
    pthread_create(&t, NULL, thread_main, (void*) NB_THREADS);

    pthread_mutex_lock(&lock);
    printf("Tous mes descendants sont créés\n");

    struct sigaction act;
    act.sa_handler = handler;
    act.sa_mask = set;
    sigaction(SIGINT, &act, NULL);
    sigdelset(&set, SIGINT);
    sigsuspend(&set);

    pthread_cond_broadcast(&cond);
    pthread_join(t, NULL);
    printf("Tous mes descendants se sont terminés.\n");

    return EXIT_SUCCESS;
}

