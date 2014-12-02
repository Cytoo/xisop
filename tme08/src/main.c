#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if(argc == 2 && !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s ARGUMENTS...\n", argv[0]);
        return EXIT_FAILURE;
    }

    return 0;
}
