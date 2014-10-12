#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

char** file_pool;
int pool_size,pool_index;
pthread_mutex_t mutex;

void* upper()
{
    char* name;
    FILE* fp1, *fp2;
    int c;
    int file_index = 0;

    while(file_index != -1){
        // Read concurrent pool
        pthread_mutex_lock(&mutex);

        if(pool_index < pool_size)
            file_index = pool_index++;
        else
            file_index = -1;
        pthread_mutex_unlock(&mutex);

        // Continue only if there is a file to handle
        if(file_index == -1)
            return NULL;

        // Handle file
        c = 1;
        name = file_pool[file_index];
        printf("handling file %d : %s \n",file_index,name);
        fp1 = fopen (name, "r");
        fp2 = fopen (name, "r+");
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
    }
    return NULL;
}
int main (int argc, char ** argv)
{
    if(argc < 3){
        printf("usage: %s NB_THREADS FILE_NAMES...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int i;
    int NB_THREADS = atoi(argv[1]);

    pool_size = argc - 2;
    pool_index = 0;
    file_pool = malloc(pool_size * sizeof(char *));

    for(i=0;i<pool_size;i++)
    {
        file_pool[i]= strdup(argv[i+2]);
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&mutex, NULL);
    pthread_t thread;

    for(i = 0; i < NB_THREADS; i++)
    {
        pthread_create(&thread, &attr, (void *(*)(void*))upper, (void*) argv[i]); 
    }

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}
