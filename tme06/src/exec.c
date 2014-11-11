#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void hdlr(int sig) { (void) sig; }

int main (void) {
    int fd1, fd2, fd3;

    if((fd1 = open("./fich1", O_RDWR| O_CREAT | O_TRUNC, 0600)) == -1)
        return EXIT_FAILURE;
    if(write(fd1,"abcde", strlen("abcde")) == -1)
        return EXIT_FAILURE;

    if(fork () == 0) {
        if((fd2 = open ("./fich1", O_RDWR)) == -1)
            return EXIT_FAILURE;
        if(write(fd1,"123", strlen ("123")) == -1)
            return EXIT_FAILURE;

        kill(getppid(), SIGUSR1);

        if(write(fd2,"45", strlen ("45")) == -1)
            return EXIT_FAILURE;

        close(fd2); 
    } else {
        struct sigaction act;
        sigset_t mask;

        sigfillset(&mask);
        act.sa_handler = hdlr;
        act.sa_mask = mask;
        sigdelset(&mask, SIGUSR1);
        sigaction(SIGUSR1, &act, NULL);

        sigsuspend(&mask);

        fd3 = dup(fd1);
        if(lseek(fd3, 0,SEEK_SET) == -1)
            return EXIT_FAILURE;
        if(write(fd3, "fg", strlen("fg")) == -1)
            return EXIT_FAILURE;
        if(write(fd1, "hi", strlen("hi")) == -1)
            return EXIT_FAILURE;

        wait(NULL);
        close(fd1);
        close(fd3);

        return EXIT_SUCCESS;
    }
}
