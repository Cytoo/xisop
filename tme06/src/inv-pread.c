#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int fd_in, fd_out;
    off_t offset;
    char* out_name;
    char c;

    if(argc != 2 || !strcmp(argv[1], "-h"))
    {
        fprintf(stderr,"usage: %s FILE\n",argv[0]);
        return EXIT_FAILURE;
    }

    if((fd_in = open(argv[1],O_RDONLY)) == -1)
    {
        perror("unable to open input file for reading");
        return EXIT_FAILURE;
    }

    out_name = malloc(sizeof(char)*(strlen(argv[1]) + 4));
    sprintf(out_name,"%s.inv",argv[1]);

    if((fd_out = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    {
        fprintf(stderr,"unable to create and open %s for writing",out_name);
        return EXIT_FAILURE;
    }

    /* Change -1 to -2 to ignore ending '\n'  */
    
    struct stat fst;
    fstat(fd_in, &fst);

    offset          = fst.st_size - 1;

    while(offset >= 0){
        pread(fd_in,&c,1, offset);
        write(fd_out,&c,1);
        offset--;
    }

    close(fd_in);
    close(fd_out);

    return EXIT_SUCCESS;
}
