#define _SVID_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct rand_msg{
    long type;
    int rand_val;
};


int main(int argc, char** argv) {
    int queue_id,i,nb_proc,total;
    struct rand_msg msg;
    pid_t * pids;
    pid_t pid;
    key_t key;

    if(argc < 2){
        perror("usage: randomqueue NB_THREADS");
        exit(EXIT_FAILURE);
    }

    nb_proc = atoi(argv[1]);
    pids = malloc(nb_proc * sizeof(pid_t));
    total = 0;

    if ((key = ftok("randomqueue",'Q')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    if ((queue_id = msgget(key , 0644 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }


    msg.type = 1;
    for(i=0;i<nb_proc;i++){
        if((pid = fork()) == 0){
            srand(time(NULL) ^ (getpid()<<16));
            msg.rand_val = (int) (10*(float)rand()/ RAND_MAX);
            msgsnd(queue_id,&msg,sizeof(int),0);
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
        msgrcv(queue_id,&msg,sizeof(int),1,0);
        total = total + msg.rand_val;
    }

    printf("total = %d\n", total);
    msgctl(queue_id,IPC_RMID,NULL);
    exit(EXIT_SUCCESS);
}
