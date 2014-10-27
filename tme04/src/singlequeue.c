#define _SVID_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <assert.h>

struct msgbuf {
    long mtype;
    char mtext[2];
};

void child(int mq, int nbproc, int idproc) {
    srand(getpid());
    int max_msg = (rand() % nbproc) + 1;
    
    struct msgbuf buf;
    buf.mtype = nbproc + 1;
    buf.mtext[0] = (char) idproc;
    buf.mtext[1] = (char) max_msg;
    
    printf("[%d]: want to receive %d msgs, sending to father process\n", getpid(), max_msg);
    msgsnd(mq, &buf, 2, 0);

    int sum = 0;
    int i;

    for(i = 0; i < max_msg; i++) {
        struct msgbuf rec;
        msgrcv(mq, &rec, 1, idproc + 1, MSG_NOERROR);
        printf("[%d]: received number n%d/%d : <%d> \n", getpid(), i + 1, max_msg, (int)rec.mtext[0]); 
        sum += (int) rec.mtext[0];
    }

    printf("[%d]: sum is %d\n", getpid(), sum);
}


int main(int argc, char** argv) {
    if(argc != 2 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s NPROC\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nbproc = atoi(argv[1]);
    int mq;
    int i;
    
    mq = msgget(ftok(argv[0], 0), IPC_CREAT | 0600);

    if(mq == -1) {
        perror("error creating msg queues");
        exit(EXIT_FAILURE);
    }

    pid_t p;

    for(i = 0; i < nbproc; i++) {
        if((p = fork()) == 0) {
            child(mq, nbproc, i);
            exit(EXIT_SUCCESS);
        } else if(p == -1) {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

    }

    srand(time(NULL));

    for(i = 0; i < nbproc; i++) {
        struct msgbuf buf;

        if(msgrcv(mq, &buf, 2, nbproc + 1, MSG_NOERROR) == -1) {
            perror("error msgrcv");
        }

        printf("Father: received max_msg from %d: %d\n", (int)buf.mtext[0], (int)buf.mtext[1]);
        int j;
        
        for(j = 0; j < buf.mtext[1]; j++) {
            int r = rand() % 101;
            struct msgbuf sen;

            sen.mtype = buf.mtext[0] + 1;
            sen.mtext[0] = (char) r;
            printf("Father: about to send %d to %d\n", r, (int) buf.mtext[0]);
            msgsnd(mq, &sen, 1, 0);
        }
    }

    for(i = 0; i < nbproc; i++)
        wait(NULL);

    msgctl(mq, IPC_RMID, NULL);
    return 0;
}

