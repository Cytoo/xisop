#define _XOPEN_SOURCE 700

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int* baguettes;
sem_t* mutex;
int N;

void pense(int i)
{
    printf("le philosophe %d pense... \n", i);
}

void mange(int i)
{
    printf("le philosophe %d mange... \n", i);
}

void philosopher(int i)
{
    for(;;)
    {
        pense(i);
        sem_wait(mutex);

        if(baguettes[i] == 1 &&
            baguettes[(i + 1) % N] == 1)
        {
            baguettes[i] = 0;
            baguettes[(i + 1) % N] = 0;
        }

        sem_post(mutex);
        mange(i);
    }
}

int main(int argc, char **argv)
{
    if(argc != 2 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s N_PHILOSOPHERS...\n", argv[0]);
        return EXIT_FAILURE;
    }

    N = atoi(argv[1]);
    
    int shmd = shm_open("/philosopher", O_RDWR | O_CREAT, 0600);
    if(shmd == -1)
    {
        perror("no shm");
        exit(EXIT_FAILURE);
    }

    ftruncate(shmd, N * sizeof(int) + sizeof(sem_t));
    void* shm;

    if((shm = mmap(NULL, N * sizeof(int) + sizeof(sem_t), PROT_READ | PROT_WRITE,
                   MAP_SHARED, shmd, 0))
        == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    baguettes = (int*) shm;
    mutex = (sem_t*) (baguettes + N);
    sem_init(mutex, 1, 1);

    for(int i = 0; i < N; ++i)
        baguettes[i] = 1;

    int i;
    pid_t p;

    for(i = 0; i < N; i++)
    {
        if((p = fork()) == 0)
        {
            philosopher(i);
            exit(EXIT_FAILURE);
        }

        else if(p == -1)
        {
            perror("oops fork");
            exit(EXIT_FAILURE);
        }
    }

    while(1)
        sleep(1);

    return 0;
}
