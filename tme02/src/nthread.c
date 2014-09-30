#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdlib.h>

struct threadinfo
{
    int val;
    pthread_t t;
};

void *thread_rand(void * data)
{
    unsigned int * id = (unsigned int*) data;
    
    //glibc doesn't implement gettid(). Had to use this non posix trick
    printf("[%lu] number %d\n", syscall(SYS_gettid), *id);  
    
    *id *= 2;
    
    return (void*) id;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        perror("./nthread <N>");
        exit(EXIT_FAILURE);
    }
   
    unsigned int n = atoi(argv[1]);
    unsigned int i;

    struct threadinfo* threads = malloc(n * sizeof(struct threadinfo));

    for(i = 0; i < n; ++i)
    {
        threads[i].val = i;
        pthread_create(&(threads[i].t), NULL, thread_rand, &(threads[i].val));
    }

    for(i = 0; i < n; ++i)
    {
        int *ret;
        pthread_join(threads[i].t, (void **) &ret);
        printf("received %d from %d\n", *ret, i);
    }

    free(threads);
    return 0;
}
