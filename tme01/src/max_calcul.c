#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <max_func.h>

int main(int argc, char *argv[]) {
    assert(argc > 1 && "Invalid number of parameters");

    int max, i;
    int s = argc - 1;
    int array[s];

    for(i = 0; i < s; ++i)
        array[i] = atoi(argv[i + 1]);

    max = max_func(array, s);
    printf("le max est : %d\n", max);

    return EXIT_SUCCESS;
}
