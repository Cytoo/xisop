#define _POSIX_C_SOURCE 199309L
#define _POSIX_SOURCE 1

#include <aio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int n = 0;
union sigval value;


/* A function to send SIGRTMIN to father  */
int signal_father(){
    if (sigqueue(getppid(), SIGRTMIN, value) == -1) {
        perror("sigqueue");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


int main(int argc, char ** argv){

    int nbproc;
    int mysig;
    sigset_t block_mask;


    if (argc != 2 || !strcmp(argv[1], "-h")){
        fprintf(stderr, "usage: %s NB_PROC\n", argv[0]);
        return EXIT_FAILURE;
    }

    nbproc = atoi(argv[1]);

    sigfillset(&block_mask);
    sigprocmask(SIG_SETMASK, &block_mask, 0);

    /* Main loop */
    while (n < nbproc - 1 ){

        /* Father stops and waits SIGRTMIN */
        if (fork()){

            sigemptyset(&block_mask);
            sigaddset(&block_mask, SIGRTMIN);

            if (sigwait(&block_mask, &mysig) == -1) {
                perror("sigwait");
                return EXIT_FAILURE;
            }

            /* Displays info after reception */
            printf("Process %d PID %d\n",n,getpid());

            /* Send SIGRTMIN to own father */
            if(n != 0)
                return signal_father();

            break;

        /* Last child starts signal chain */
        } else {
            n++;

            if (n == nbproc - 1 ){
                printf("Process %d PID %d\n",n,getpid());
                return signal_father();
            }

            else
                continue;
        }

    }

    return EXIT_SUCCESS;
}
