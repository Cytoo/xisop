#define _SVID_SOURCE 1

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <thread_stack.h>

static int shmid = -2;
static int semid = -2;

static char *shm = NULL;
static int  *size = NULL;
#define WRITE 0
#define READ  1
#define LOCK  2

void careOfShmidSem()
{
   if(shmid < -1)
   {
        shmid = shmget(0xC0FFEE, STACK_SIZE + 1 * sizeof(int) , IPC_CREAT | IPC_EXCL | 0600);
        
        if(shmid == -1 && errno == EEXIST)
        {
            shmid = shmget(0xC0FFEE, STACK_SIZE + 1 * sizeof(int), IPC_CREAT | 0600);       
        }
        
        shm = shmat(shmid, NULL, 0);
        size =(int*) (shm + 100);
   }

   if(semid < -1)
   {
       semid = semget(0xC0FFEE,  3, IPC_CREAT| IPC_EXCL | 0600);

       if(semid == -1 && errno == EEXIST)
       {
            semid = semget(0xC0FFEE, 3, IPC_CREAT | 0600);
       }
       else if(semid > 0)
       {
           //If semaphore was created, init 
            semctl(semid, WRITE, SETVAL, STACK_SIZE);
            semctl(semid, READ , SETVAL, 0);
            semctl(semid, LOCK,  SETVAL, 1);
       }
   }

    if(shmid == -1)
    {
        perror("shared memory access error");
        exit(EXIT_FAILURE);
    }
    if(semid == -1)
    {
        perror("sem access error");
        exit(EXIT_FAILURE);
    }

}

void push(char c)
{
    careOfShmidSem();

    struct sembuf buf[2];
    buf[0].sem_num = WRITE;    //We wait on the write ability (is it full or not?)
    buf[0].sem_op  = -1;       //We want to write 1 char so we lower of 1
    buf[0].sem_flg = 0;
    buf[1].sem_num = LOCK;
    buf[1].sem_op  = -1;
    buf[1].sem_flg = 0;
    semop(semid, buf, 2);  //We know we can write 
    
   // printf("[%d]: push %c, size %d\n", getpid(), c, *size);
    shm[*size] = c;        //Actually writing
    (*size)++;             //it grows
   // printf("[%d]: did push %c, size %d\n", getpid(), c, *size);

    buf[0].sem_num  = LOCK;
    buf[0].sem_op   = 1;
    buf[0].sem_flg  = 0;
    buf[1].sem_num  = READ;
    buf[1].sem_op   = 1;
    buf[1].sem_flg  = 0;

    semop(semid, buf, 2);
}


char pop()
{
    careOfShmidSem();
    
    struct sembuf buf[2];

    buf[0].sem_num = READ;
    buf[0].sem_op  = -1;
    buf[0].sem_flg = 0;
    buf[1].sem_num = LOCK;
    buf[1].sem_op  = -1;
    buf[1].sem_flg = 0;
    
    semop(semid, buf, 2);
   
 //   printf("[%d]: extract size %d\n", getpid(), *size);
    char c = shm[*(size - 1)];
    (*size)--;
   // printf("[%d]: extracted %c, size %d\n", getpid(), c, *size);

    buf[0].sem_num = LOCK;
    buf[0].sem_op  = 1;
    buf[0].sem_flg = 0;
    buf[1].sem_num = WRITE;
    buf[1].sem_op  = 1;
    buf[1].sem_flg = 0;
    
    semop(semid, buf, 2);

    return c;
}
