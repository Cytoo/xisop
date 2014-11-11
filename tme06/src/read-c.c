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

    FILE* fd;

    if((fd = fopen(argv[1], "r")) == NULL)
    {
        perror("unable to open input file");
        return EXIT_FAILURE;
    }

    if(fork())
        fork();

    char c;

    while((c = fgetc(fd)) > 0)
        printf("[%d] '%c'\n", getpid(), c);

    if(ferror(fd))
    {
        perror("error while reading");
        fclose(fd);
        return EXIT_FAILURE;
    }

    fclose(fd);
    
    return EXIT_SUCCESS;
}
