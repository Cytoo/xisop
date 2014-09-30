#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void nop() {}

int main(int argc, char **argv)
{
	if(argc <= 1)
	{
		perror("./procstr [nbproc]");
		exit(EXIT_FAILURE);
	}

	int nb = atoi(argv[1]);

	sigset_t sig_proc;
	sigfillset(&sig_proc);
    sigdelset(&sig_proc, SIGINT);
    sigdelset(&sig_proc, SIGSTOP);
    sigdelset(&sig_proc, SIGCONT);
    sigdelset(&sig_proc, SIGCHLD);
	sigprocmask(SIG_BLOCK, &sig_proc, NULL);

    struct sigaction act;
    act.sa_handler = nop;
    act.sa_mask = sig_proc;
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, NULL);

    pid_t* pids = malloc((nb + 1) * sizeof(pid_t));

    if(!pids)
    {
        perror("cant allocate memory");
        exit(EXIT_FAILURE);
    }

    pids[0] = getpid();
    printf("initial process %d\n", pids[0]);

    int i;
    for(i = 1; i <= nb; i++) {
        pid_t t;

        if((t = fork()) == 0) {
            pids[i] = getpid();
            if(i == nb) {
               // kill(pids[i - 1], SIGCHLD);
                kill(getpid(), SIGSTOP);
                exit(EXIT_SUCCESS);
            }
        }

        else if ( t == -1) {
            perror("cannot create a processus");
            exit(EXIT_FAILURE);
        }

        else {
            sigsuspend(&sig_proc);
            act.sa_flags = SA_NOCLDSTOP;
            sigaction(SIGCHLD, &act, NULL);

            if(getpid() == pids[0]) {
                printf("all processus stopped\n");
                kill(pids[1], SIGCONT);

                sigsuspend(&sig_proc);
                printf("all processus exitted\n");
                exit(EXIT_SUCCESS);
            } else {
               // kill(pids[i - 2], SIGCHLD);
                kill(getpid(), SIGSTOP);
                kill(pids[i], SIGCONT);
                sigsuspend(&sig_proc);
                exit(EXIT_SUCCESS);
            }
        }
    }

    return EXIT_SUCCESS;
}
