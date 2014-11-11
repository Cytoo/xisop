#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFSIZE 32

int main(int argc, char* argv[])
{
    int fd_in, fd_out;

    if(argc != 3)
    {
        fprintf(stderr, "usage: %s IN_FILE OUT_FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    if((fd_in = open(argv[1], O_RDONLY)) == -1)
    {
        perror("unable to open input file for reading");
        return EXIT_FAILURE;
    }

    struct stat st;
    if(fstat(fd_in, &st) == -1)
    {
        perror("unable to retrieve information");
        return EXIT_FAILURE;
    }

    if(!S_ISREG(st.st_mode))
    {
        fprintf(stderr, "not a regular file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    if((fd_out = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
    {
        perror("unable to open output file for writing");
        return EXIT_FAILURE;
    }

    char buf[BUFSIZE];
    int siz;

    do
    {
        siz = read(fd_in, (void*) buf, BUFSIZE);
        
        if(siz == -1)
        {
            perror("error while reading");
            return EXIT_FAILURE;
        }

        if(write(fd_out, (void*) buf, siz) == -1)
        {
            perror("error while writing");
            return EXIT_FAILURE;
        }
    } while(siz == BUFSIZE);

    close(fd_in);
    close(fd_out);

    return EXIT_SUCCESS;
}
