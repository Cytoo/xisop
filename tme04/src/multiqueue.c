#define _SVID_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <assert.h>

struct msgbuf
{
    long mtype;
    char mtext[2];
};

void child(int* mq, int nbproc, int idproc)
{
    srand(getpid());
    int max_msg = (rand() % nbproc) + 1;
    
    struct msgbuf buf;
    buf.mtype = 1;
    buf.mtext[0] = (char) idproc;
    buf.mtext[1] = (char) max_msg;
    
    printf("[%d]: want to receive %d msgs, sending to father process\n", getpid(), max_msg);
    msgsnd(mq[nbproc], &buf, 2, 0);

    int sum = 0;
    int i;
    for(i = 0; i < max_msg; i++)
    {
        struct msgbuf rec;
        msgrcv(mq[idproc], &rec, 1, 0, MSG_NOERROR);
        printf("[%d]: received number n%d/%d : <%d> \n", getpid(), i + 1, max_msg, (int)rec.mtext[0]); 
        sum += (int) rec.mtext[0];
    }
    printf("[%d]: sum is %d\n", getpid(), sum);


}


int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("usage: %s NPROC\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nbproc = atoi(argv[1]);
    
    int i;
    int *mq = malloc((nbproc + 1) * sizeof(int));
    for(i = 0; i <= nbproc; i++)
    {
        mq[i] = msgget(ftok(argv[0], i + 1), IPC_CREAT | 0600);
        if(mq[i] == -1)
        {
            int j;
            perror("error creating msg queues");
            for(j = i - 1; j >= 0; j--)
            {
                msgctl(mq[j], IPC_RMID, NULL);         
            }
            free(mq);
            exit(EXIT_FAILURE);
        }
    }

    pid_t p;
    for(i = 0; i < nbproc; i++)
    {
        if((p = fork()) == 0)
        {
            child(mq, nbproc, i);
            exit(EXIT_SUCCESS);
        }
        else if(p == -1)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

    }
    srand(time(NULL));
    for(i = 0; i < nbproc; i++)
    {
        struct msgbuf buf;
        if(msgrcv(mq[nbproc], &buf, 2, 0,  MSG_NOERROR) == -1)
        {
            perror("error msgrcv");
        }
        printf("Father: received max_msg from %d: %d\n", (int)buf.mtext[0], (int)buf.mtext[1]);
        
        int j;
        for(j = 0; j < buf.mtext[1]; j++)
        {
            int r = rand() % 101;

            struct msgbuf sen;
            sen.mtype = 1;
            sen.mtext[0] = (char) r;
            printf("Father: about to send %d to %d\n", r, (int)buf.mtext[0]);
            msgsnd(mq[(int)buf.mtext[0]], &sen, 1, 0);
        }


    }
    for(i = 0; i < nbproc; i++)
        wait(NULL);

    for(i = 0; i <= nbproc; i++)
    {
        msgctl(mq[i], IPC_RMID, NULL);
    }

    free(mq);

    return 0;
}

