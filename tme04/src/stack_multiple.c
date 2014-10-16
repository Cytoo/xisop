#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <thread_stack.h>


void producteur() 
{
    int c;
    for(;;)
    {
        c = getchar();
        if ( c == EOF )
            break;
        push(c);
    }
    exit(0);
}

void consommateur() 
{
    char c;
    for (;;) 
    {
        c = pop();
        putchar(c);
        fflush(stdout);
    }
}


int main (int argc, char ** argv)
{

    if(argc != 3)
    {
        printf("usage: %s NB_PROD NB_CONS\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nbp, nbc;
    nbp = atoi(argv[1]);
    nbc = atoi(argv[2]);

    int i;
    pid_t p;
    for(i = 0; i < nbp; i++)
    {
        if((p = fork()) == 0)
        {
            producteur();
            exit(0);
        }
        else if(p == -1)
        {
            perror("error creating child");
            exit(EXIT_FAILURE);
        }
    }

    for(i = 0; i < nbc; i++)
    {

        if((p = fork()) == 0)
        {
            consommateur();
            exit(0);
        }
        else if(p == -1)
        {
            perror("error creating child");
            exit(EXIT_FAILURE);
        }
        
    }

    for(i = 0; i < nbp; i++)
    {
        wait(NULL);
    }

    sleep(5);
    kill(0, SIGKILL);

    return EXIT_SUCCESS;
}
