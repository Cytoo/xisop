#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv) {
    int* addr_at;
    int shm_id, i, nb_proc, total;
    pid_t* pids;
    pid_t pid;

    if(argc != 2 || !strcmp(argv[1], "-h"))
    {
        fprintf(stderr, "usage: %s NB_THREADS\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nb_proc = atoi(argv[1]);
    pids = malloc(nb_proc * sizeof(pid_t));
    total = 0;

    if((shm_id = shm_open("posixshm", O_CREAT | O_RDWR, 0600)) == -1)
    {
        perror("can't open shared memory");
        exit(EXIT_FAILURE);
    }

    ftruncate(shm_id, nb_proc * sizeof(int));

    if((addr_at = mmap(NULL, nb_proc * sizeof(int),
                       PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0))
        == MAP_FAILED)
    {
        perror("failed to mmap shared memory");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < nb_proc; i++)
    {
        if((pid = fork()) == 0)
        {
            srand(time(NULL) ^ (getpid() << 16));
            addr_at[i] = (int) (10 * (float) rand() / RAND_MAX);
            exit(EXIT_SUCCESS);
        }

        else pids[i] = pid;
    }

    for(i = 0; i < nb_proc; i++)
        waitpid(pids[i], NULL, 0);

    for(i = 0; i < nb_proc; i++)
        total = total + addr_at[i];

    printf("total = %d\n", total);
    munmap(addr_at, nb_proc * sizeof(int));
    shm_unlink("posixshm");
    exit(EXIT_SUCCESS);
}
