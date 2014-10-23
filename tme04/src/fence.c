#define _SVID_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>


static int* psem;


void wait_fence(int nbproc)
{
    if(!psem)
    {
        printf("no semaphore...\n");
        exit(EXIT_FAILURE);
    }

    //lock critic section with mutex
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op  = -1;
    op.sem_flg = 0;
    semop(*psem, &op, 1);

    //if nbproc - 1 process wait on the resource, release it
    int nb = semctl(*psem, 1, GETNCNT);

    if(nb == nbproc - 1)
    {
        op.sem_num = 1;
        op.sem_op  = nb;
        op.sem_flg = 0;
        semop(*psem, &op, 1); //Relache de la barriere

        op.sem_num = 0;
        op.sem_op  = 1;
        op.sem_flg = 0;
        semop(*psem, &op, 1); //relache du mutex
    }
    else
    {
        op.sem_num = 0;
        op.sem_flg = 0;
        op.sem_op = 1;
        semop(*psem, &op, 1); //relache mutex

        op.sem_num = 1;
        op.sem_flg = 0;
        op.sem_op = -1;
        semop(*psem, &op, 1); //prise barriere

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

    //create 2 sem : #0 is mutex, #1 is the actual fence
    int idsem   = semget(ftok(argv[0], 1), 2, IPC_CREAT | IPC_EXCL | 0600);
    
    if(idsem == -1)
    {
        perror("cant create sem");
        exit(EXIT_FAILURE);
    } 
    
    psem = &idsem;

    //init mutex to 1
    semctl(idsem, 0, SETVAL, 1);
    //init fence to 0
    semctl(idsem, 1, SETVAL, 0); 



    //create child process
    pid_t p;
    int   i;
    for(i = 0; i < nb_proc; i++)
    {
        if((p = fork()) == 0)
        {
            process(nb_proc);
        }
        else if(p == -1)
        {
            perror("error process");
            exit(EXIT_FAILURE);
        }
    }


    //waiting for child process to end
    for(i = 0; i < nb_proc; i++)
    {
        wait(NULL);
    }
    

    semctl(idsem, 0, IPC_RMID);
    return 0;
}
