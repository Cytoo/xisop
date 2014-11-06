#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>

static sem_t* fence;
static sem_t* waiting;
static sem_t* mutex;
static uint8_t* addr_at;

static void close_ipc(void)
{
    sem_close(fence);
    sem_close(mutex);
    sem_close(waiting);
    munmap(addr_at, 3 * sizeof(sem_t));
    shm_unlink("fence");
}

void wait_fence(int nbproc)
{
    sem_wait(mutex);
    sem_post(waiting);

    // if nbproc - 1 process wait on the resource, release it
    int nwait;
    sem_getvalue(waiting, &nwait);

    if(nwait == nbproc)
    {
        for(int i = 0; i < nbproc - 1; ++i)
            sem_post(fence);
        sem_post(mutex);
    }

    else
    {
        sem_post(mutex);
        sem_wait(fence);
    }
}

void process(int nbproc)
{
    printf("[%d]: avant barriere\n", getpid());
    wait_fence(nbproc);
    printf("[%d]: apres barriere\n", getpid());
    exit(0);
}

int main(int argc, char **argv)
{
    if(argc != 2 || !strcmp(argv[1], "-h"))
    {
        fprintf(stderr, "usage: %s NPROC\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nb_proc = atoi(argv[1]);        
    int shm_id;

    if((shm_id = shm_open("fence", O_CREAT | O_RDWR, 0600)) == -1)
    {
        perror("can't open shared memory");
        exit(EXIT_FAILURE);
    }

    ftruncate(shm_id, 3 * sizeof(sem_t));

    if((addr_at = mmap(NULL, 3 * sizeof(sem_t),
                       PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0))
        == MAP_FAILED)
    {
        perror("failed to mmap shared memory");
        exit(EXIT_FAILURE);
    }

    waiting = (sem_t*) addr_at;
    fence = (sem_t*) (addr_at + sizeof(sem_t));
    mutex = (sem_t*) (addr_at + 2 * sizeof(sem_t));

    int s1 = sem_init(waiting, 1, 0);
    int s2 = sem_init(fence, 1, 0);
    int s3 = sem_init(mutex, 1, 1);
    
    if(s1 || s2 || s3)
    {
        perror("can't create sem");
        close_ipc();
        exit(EXIT_FAILURE);
    } 
    
    // create child process
    pid_t p;
    int   i;

    for(i = 0; i < nb_proc; i++)
    {
        if((p = fork()) == 0)
            process(nb_proc);

        else if(p == -1)
        {
            perror("error process");
            close_ipc();
            exit(EXIT_FAILURE);
        }
    }

    atexit(close_ipc);

    // waiting for child process to end
    for(i = 0; i < nb_proc; i++)
        wait(NULL);
    
    return 0;
}
