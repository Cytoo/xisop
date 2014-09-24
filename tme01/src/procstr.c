#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	
	if(argc <= 1)
	{
		perror("./procstr [nbproc]");
		exit(EXIT_FAILURE);
	}

	int nb = atoi(argv[1]);

	pid_t* pids = malloc((nb + 1) * sizeof(pid_t));	

	if(!pids)
	{
		perror("cant allocate memory");
		exit(EXIT_FAILURE);
	}

	pids[0] = getpid();
	printf("initial process %d\n", pids[0]);

	int i;
	for(i = 1; i <= nb; i++)
	{
		pid_t t;

		if((t = fork()) == 0)
		{
			pids[i] = getpid();
			if(i == nb)
			{
				int j;
				for(j = 0; j <= nb; j++)
				{
					printf("%d: %d . %d\n", getpid(), j, pids[j]);
				}
				
			}
		}
		else if ( t == -1)
		{
			perror("cannot create a processus");
			exit(EXIT_FAILURE);
		}
		else
		{
			wait(NULL);
			printf("%d: father %d, child %d\n", getpid(), getppid(), t);
			exit(EXIT_SUCCESS);
		}
		
	}
	

	free(pids);
	
	return 0;
}
