#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc != 2 || !strcmp(argv[1], "-h"))
    {
        fprintf(stderr, "usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd;

    if((fd = open(argv[1], O_RDONLY)) == -1)
    {
        perror("unable to open input file");
        return EXIT_FAILURE;
    }

    if(fork())
        fork();

    char c;
    int n;

    while((n = read(fd, (void*) &c, sizeof(char))) > 0)
        printf("[%d] '%c'\n", getpid(), c);

    if(n == -1)
    {
        perror("error while reading");
        close(fd);
        return EXIT_FAILURE;
    }

    close(fd);
    
    return EXIT_SUCCESS;
}
