#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MAX_CLIENTS 256
#define ST_SIZE 32
#define BUF_SIZE 1024

struct request {
    long type;
    char content[BUF_SIZE];
};


int main(int argc, char ** argv){
    struct request *msg, *msg_out;

    /* Server variables */
    sem_t *sem_msgread, *sem_msgwrite;
    int shm;
    char st_msgread[ST_SIZE], st_msgwrite[ST_SIZE], st_shm[ST_SIZE];

    /* Clients variables*/
    char *clients[MAX_CLIENTS];
    int shm_client[MAX_CLIENTS];
    sem_t *sem_write_client[MAX_CLIENTS], *sem_read_client[MAX_CLIENTS];
    char st_read[ST_SIZE], st_write[ST_SIZE], st_shm_client[ST_SIZE];

    if(argc != 2 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage : %s server_id\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Create connection strings  */
    sprintf(st_msgread,"/%s_sem:0",argv[1]);
    sprintf(st_msgwrite,"/%s_sem:1",argv[1]);
    sprintf(st_shm,"/%s_shm:0",argv[1]);

    /* Create shareds ressources : 2 SEM & 1 SHM */
    sem_msgread = sem_open(st_msgread,O_RDWR|O_CREAT,0666,0);
    if(sem_msgread == SEM_FAILED){
        fprintf(stderr,"sem_open %s",st_msgread);
        return EXIT_FAILURE;
    }
    sem_msgwrite = sem_open(st_msgwrite,O_RDWR|O_CREAT,0666,1);
    if(sem_msgwrite == SEM_FAILED){
        fprintf(stderr,"sem_open %s",st_msgwrite);
        return EXIT_FAILURE;
    }
    shm = shm_open(st_shm,O_RDWR|O_CREAT,0666);
    if(shm == -1){
        fprintf(stderr,"shm_open %s",st_shm);
        return EXIT_FAILURE;
    }

    /* Truncate and map SHM*/
    if(ftruncate(shm,sizeof(struct request)) == -1){
        perror("ftruncate");
        return EXIT_FAILURE;
    }

    msg = mmap(NULL,sizeof(struct request),
            PROT_READ|PROT_WRITE,MAP_SHARED,shm,0);
    if(msg == MAP_FAILED){
        perror("mmap");
        return EXIT_FAILURE;
    }

    /* Main loop*/
    int i;
    for(i=0;i<MAX_CLIENTS;i++){
        clients[i] = NULL;
    }

    for(;;){
        sem_wait(sem_msgread);
        /* Connection */
        if(msg->type == 0){
            for(i = 0;i<MAX_CLIENTS;i++){
                if(clients[i] == NULL)
                {
                    clients[i] = strdup(msg->content);

                    /* Create connection strings for client */
                    sprintf(st_read,"/%s_sem:0",clients[i]);
                    sprintf(st_write,"/%s_sem:1",clients[i]);
                    sprintf(st_shm_client,"/%s_shm:0",clients[i]);

                    /* Access client shared ressources : 2 SEM & 1 SHM  */
                    sem_read_client[i] = sem_open(st_read,O_RDWR);
                    if(sem_read_client[i] == SEM_FAILED){
                        fprintf(stderr,"sem_open %s",st_read);
                        return EXIT_FAILURE;
                    }
                    sem_write_client[i] = sem_open(st_write,O_RDWR);
                    if(sem_write_client[i] == SEM_FAILED){
                        fprintf(stderr,"sem_open %s",st_write);
                        return EXIT_FAILURE;
                    }
                    shm_client[i] = shm_open(st_shm_client,O_RDWR,0600);
                    if(shm_client[i] == -1){
                        fprintf(stderr,"shm_open %s",st_shm_client);
                        return EXIT_FAILURE;
                    }

                    printf("Connected : %s\n",clients[i]);
                    printf("  sem_read : %s\n",st_read);
                    printf("  sem_write : %s\n",st_write);
                    printf("  shm : %s\n",st_shm_client);
                    break;
                }
            }
        }
        /* Disconnection */
        else if(msg->type == 2){
            for(i=0;i<MAX_CLIENTS;i++){
                if(clients[i] != NULL){
                    if (strcmp(clients[i],msg->content)==0)
                    {
                        printf("Disconnecting %s...\n",clients[i]);
                        free(clients[i]);
                        sem_close(sem_write_client[i]);
                        sem_close(sem_read_client[i]);
                        clients[i] = NULL;
                        printf("Disconnected.\n");
                        break;
                    }
                }
            }

        }
        /* Message */
        else if(msg->type == 1){
            for(i = 0;i<MAX_CLIENTS;i++){
                if(clients[i] != NULL)
                {
                    msg_out = mmap(NULL,sizeof(struct request),
                            PROT_READ|PROT_WRITE,MAP_SHARED,shm_client[i],0);
                    if(msg_out == MAP_FAILED){
                        perror("mmap");
                        return EXIT_FAILURE;
                    }
                    sem_wait(sem_write_client[i]);
                    strncpy(msg_out->content,msg->content,BUF_SIZE);
                    sem_post(sem_read_client[i]);
                    munmap(msg_out, sizeof(struct request));
                }
            }

        }
        sem_post(sem_msgwrite);
    }

    /* Closing and unlinking shared ressources*/
    sem_close(sem_msgread);
    sem_close(sem_msgwrite);
    munmap(msg,sizeof(struct request));

    sem_unlink(st_msgread);
    sem_unlink(st_msgwrite);
    shm_unlink(st_shm);

    return EXIT_SUCCESS;
}
