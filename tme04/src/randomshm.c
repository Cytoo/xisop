#define _SVID_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    int shm_id,i,nb_proc,total;
    int *addr_at;
    pid_t * pids;
    pid_t pid;
    key_t key;

    if(argc != 2 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s NB_THREADS\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    nb_proc = atoi(argv[1]);
    pids = malloc(nb_proc * sizeof(pid_t));
    total = 0;

    if ((key = ftok("randomshm",'Q')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((shm_id = shmget(key,nb_proc * sizeof(int),  0666 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    addr_at = shmat(shm_id,0,0600);

    for(i=0;i<nb_proc;i++){
        if((pid = fork()) == 0){
            srand(time(NULL) ^ (getpid()<<16));
            addr_at[i] = (int) (10*(float)rand()/ RAND_MAX);
            exit(EXIT_SUCCESS);
        }
        else {
            pids[i] = pid;
        }
    }

    for(i=0;i<nb_proc;i++){
        waitpid(pids[i],NULL,0);
    }

    for(i=0;i<nb_proc;i++){
        total = total + addr_at[i];
    }

    printf("total = %d\n", total);

    shmctl(shm_id,IPC_RMID,NULL);
    exit(EXIT_SUCCESS);
}
