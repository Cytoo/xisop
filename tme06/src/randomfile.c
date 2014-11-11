#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

    char* path = "random.ipc";
    int fd;

    if((fd = open(path, O_RDONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    {
        perror("unable to open IPC file");
        return EXIT_FAILURE;
    }

    for(i = 0; i < nb_proc; ++i)
    {
        if(fork() == 0)
        {
            srand(time(NULL) ^ (getpid() << 16));
            int rand_val = (int) (10 * (float) rand() / RAND_MAX);

            if((fd = open(path, O_RDWR)) == -1)
            {
                perror("[%d] unable to reopen IPC file");
                return EXIT_FAILURE;
            }
            
            lseek(fd, i * sizeof(int), SEEK_SET);
            write(fd, (void*) &rand_val, sizeof(int));
            close(fd);

            return EXIT_SUCCESS;
        }
    }

    for(i = 0; i < nb_proc; ++i)
        wait(NULL);

    for(i = 0; i < nb_proc; ++i)
    {
        int buf;
        read(fd, (void*) &buf, sizeof(int));
        total += buf;
    }

    printf("total = %d\n", total);
    close(fd);

    return EXIT_SUCCESS;
}
