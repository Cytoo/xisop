#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void pense() {}
void mange() {}

void philosopher()
{
   //while(1) 
    {
        pense();
        mange();
    }
}

int main(int argc, char **argv)
{
    if(argc != 2 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s N_PHILOSOPHERS...\n", argv[0]);
        return EXIT_FAILURE;
    }

    int nb = atoi(argv[1]);
    
    int shmd = shm_open("/philosopher", O_RDWR | O_CREAT, 0600);
    if(shmd == -1)
    {
        perror("no shm");
        exit(EXIT_FAILURE);
    }

    int i;
    pid_t p;
    for(i = 0; i < nb; i++)
    {
        if((p = fork()) == 0)
        {
            philosopher();
            exit(EXIT_FAILURE);
        }
        else if(p == -1)
        {
            perror("oops fork");
            exit(EXIT_FAILURE);
        }
    }

    while(1)
        sleep(1);

    return 0;
}
