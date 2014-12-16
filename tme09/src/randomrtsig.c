#define _XOPEN_SOURCE 700

#include <aio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

void hdlr(int sig) { (void) sig; }

int main(int argc, char** argv)
{
    int i, nb_proc, total;

    if(argc != 2 || !strcmp(argv[1], "-h"))
    {
        fprintf(stderr, "usage: %s NB_THREADS\n", argv[0]);
        return EXIT_FAILURE;
    }

    nb_proc = atoi(argv[1]);
    total = 0;

    sigset_t msk;
    sigfillset(&msk);
    sigprocmask(SIG_BLOCK, &msk, NULL);

    for(i = 0; i < nb_proc; ++i)
    {
        if(fork() == 0)
        {
            srand(time(NULL) ^ (getpid() << 16));
            int rand_val = (int) (10 * (float) rand() / RAND_MAX);
            union sigval value;
            value.sival_int = rand_val;
            sigqueue(getppid(), SIGRTMIN, value);
            return EXIT_SUCCESS;
        }
    }

    struct sigaction act;
    act.sa_handler = hdlr;
    act.sa_flags = 0;
    act.sa_mask = msk;
    sigaction(SIGRTMIN, &act, NULL);
    sigemptyset(&msk);
    sigaddset(&msk, SIGRTMIN);

    siginfo_t info;

    for(i = 0; i < nb_proc; ++i) {
        sigwaitinfo(&msk, &info);
        total += info.si_value.sival_int;
    }

    printf("total = %d\n", total);

    return EXIT_SUCCESS;
}
