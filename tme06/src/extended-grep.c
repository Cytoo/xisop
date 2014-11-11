#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "usage: %s EXPR PATH\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* expr = argv[1];
    char* path = argv[2];
    int found = 0;

    int fd;
    DIR* d;
    void* map;

    if((d = opendir(path)) == NULL)
    {
        perror("unable to open directory");
        return EXIT_FAILURE;
    }

    fchdir(dirfd(d));

    for(struct dirent* ent = readdir(d); ent != NULL; ent = readdir(d))
    {
        if((fd = open(ent->d_name, O_RDONLY)) == -1)
        {
            perror("unable to open file %s");
            continue;
        }

        struct stat st;
        if(fstat(fd, &st) == -1)
        {
            perror("unable to retrieve information");
            close(fd);
            continue;
        }

        if(!S_ISREG(st.st_mode))
        {
            close(fd);
            continue;
        }

        int seek = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        if((map = mmap(NULL, seek, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
        {
            perror("unable to mmap");
            close(fd);
            continue;
        }

        if(strstr((char*) map, expr) != NULL)
        {
            printf("%s\n", ent->d_name);
            found = 1;
        }

        munmap(map, seek);
        close(fd);
    }

    closedir(d);

    if(!found)
        printf("Aucun fichier trouvé\n");

    return EXIT_SUCCESS;
}
