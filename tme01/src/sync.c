#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void nop() {}

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;	

	pid_t t;
	pid_t p = getpid();
	
	sigset_t sig_proc;
	sigfillset(&sig_proc);
	sigprocmask(SIG_BLOCK, &sig_proc, NULL);

	if((t = fork()) == 0)
	{
		printf("created p2\n");

		if((t = fork()) == 0)
		{
			printf("created p3\n");
			kill(p, SIGUSR1);
		}

		else if(t == -1)
		{
			perror("process creation failed");
			exit(EXIT_FAILURE);
		}

		else
		{
			wait(NULL);
			kill(p, SIGUSR2);
		}
	}	

	else if ( t == -1)
	{
		perror("process creation failed");
		exit(EXIT_FAILURE);
	}

	else
	{	
		struct sigaction act;
		sigfillset(&sig_proc);
		act.sa_handler = nop;
		act.sa_mask = sig_proc;
		act.sa_flags = 0;
		sigaction(SIGUSR1, &act, NULL);
		sigaction(SIGUSR2, &act, NULL);

		sigfillset(&sig_proc);
		sigdelset(&sig_proc,SIGUSR1);
		sigsuspend(&sig_proc);
		printf("father : grandchild created\n");

		sigfillset(&sig_proc);
		sigdelset(&sig_proc, SIGUSR2);
		sigsuspend(&sig_proc);
		printf("father : child aknowledged grandchild death\n");

		wait(NULL);
	}

	return 0;
}
