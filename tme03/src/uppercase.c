#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>

void* upper(char* name)
{
    FILE* fp1, *fp2;
    int c = 1;
    
    fp1= fopen (name, "r");
    fp2= fopen (name, "r+");
    
    if ((fp1== NULL) || (fp2== NULL)) {
        perror ("fopen");
        exit (1);
    }

    while (c !=EOF) {
        c=fgetc(fp1);
        if (c!=EOF)
            fputc(toupper(c),fp2);
    }

    fclose (fp1);
    fclose (fp2);

    return NULL;
}
int main (int argc, char ** argv)
{
    int i;
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t useless;

    for(i = 1; i < argc; i++)
    {
        pthread_create(&useless, &attr, (void *(*)(void*))upper, (void*) argv[i]); 
    }

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
