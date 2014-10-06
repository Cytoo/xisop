#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int NB_THREADS,count;
pthread_cond_t cond;
pthread_mutex_t lock;

void wait_barrier(int n){
  pthread_mutex_lock(&lock);
  (++count < n) ?
    pthread_cond_wait(&cond,&lock) :
    pthread_cond_broadcast(&cond);
  --count;
  pthread_mutex_unlock(&lock);
}

void* thread_func () {
  printf ("avant barriere\n");
  wait_barrier (NB_THREADS);
  printf ("après barriere\n");
  pthread_exit ((void*)0);
}

int main(int argc, char* argv[]) {
  if(argc != 2) {
    printf("usage: %s THREAD_COUNT\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  pthread_cond_init(&cond,NULL);
  pthread_mutex_init(&lock,NULL);
  count = 0;
  NB_THREADS=atoi(argv[1]);
 
  pthread_t* t = malloc(NB_THREADS * sizeof(pthread_t));

  for(int64_t i = 0; i < NB_THREADS; ++i) {
    pthread_create(t + i, NULL, (void* (*)(void*)) thread_func,NULL);
  }
  
  
  for(int64_t i = 0; i < NB_THREADS; ++i) {
    pthread_join(t[i],NULL);
  }
  
  return EXIT_SUCCESS;
}
