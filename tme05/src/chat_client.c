#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

#define ST_SIZE 32
#define BUF_SIZE 1024

struct request {
    long type;
    char content[BUF_SIZE];
};

/* Global variables */
bool run = true;
struct request *msg_in;
sem_t *sem_read,*sem_write;
char last[BUF_SIZE];

/* Display thread */
void* display_thread(){
    for(;;)
    {
        sem_wait(sem_read);
        if(strcmp(last,msg_in->content)!=0)
            if(fputs(msg_in->content, stdout) < 0)
                perror("print");
        sem_post(sem_write);
    }
}

/* SIGINT handler */
void stop(){
	run = false;
}

/* Main */
int main(int argc, char ** argv){
    struct request *msg;

    /* Server variables */
    sem_t *sem_msgread, *sem_msgwrite;
    int shm_server;
    char st_msgread[ST_SIZE], st_msgwrite[ST_SIZE], st_shm_server[ST_SIZE];

    /* Client variables */
    int shm_client;
    char buf[BUF_SIZE],st_read[ST_SIZE],st_write[ST_SIZE],st_shm_client[ST_SIZE];

    if(argc != 3 || !strcmp(argv[1], "-h") || !strcmp(argv[2], "-h")) {
        fprintf(stderr, "usage : %s server_id client_id",argv[0]);
        return EXIT_FAILURE;
    }
	
	/* Assign SIGINT hanlder*/
	sigset_t sig_proc;
	sigfillset(&sig_proc);

    struct sigaction act;
    act.sa_handler = stop;
    act.sa_mask = sig_proc;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    /* Create connection strings  */
    sprintf(st_msgread,"/%s_sem:0",argv[1]);
    sprintf(st_msgwrite,"/%s_sem:1",argv[1]);
    sprintf(st_shm_server,"/%s_shm:0",argv[1]);

    sprintf(st_read,"/%s_sem:0",argv[2]);
    sprintf(st_write,"/%s_sem:1",argv[2]);
    sprintf(st_shm_client,"/%s_shm:0",argv[2]);

    /* Access server shareds ressources : 2 SEM & 1 SHM */
    sem_msgread = sem_open(st_msgread,O_RDWR);
    if(sem_msgread == SEM_FAILED){
        fprintf(stderr,"sem_open %s",st_msgread);
        return EXIT_FAILURE;
    }
    sem_msgwrite = sem_open(st_msgwrite,O_RDWR);
    if(sem_msgwrite == SEM_FAILED){
        fprintf(stderr,"sem_open %s",st_msgwrite);
        return EXIT_FAILURE;
    }
    shm_server = shm_open(st_shm_server,O_RDWR,0600);
    if(shm_server == -1){
        fprintf(stderr,"shm_open %s",st_shm_server);
        return EXIT_FAILURE;
    }

    /* Map SHM*/
    msg = mmap(NULL,sizeof(struct request),
            PROT_READ|PROT_WRITE,MAP_SHARED,shm_server,0);
    if(msg == MAP_FAILED){
        perror("mmap");
        return EXIT_FAILURE;
    }

    /* Create client shareds ressources : 2 SEM & 1 SHM */
    sem_read = sem_open(st_read,O_RDWR|O_CREAT,0666,0);
    if(sem_read == SEM_FAILED){
        fprintf(stderr,"sem_open %s",st_read);
        return EXIT_FAILURE;
    }
    sem_write = sem_open(st_write,O_RDWR|O_CREAT,0666,1);
    if(sem_write == SEM_FAILED){
        fprintf(stderr,"sem_open %s",st_write);
        return EXIT_FAILURE;
    }
    shm_client = shm_open(st_shm_client,O_RDWR|O_CREAT,0600);
    if(shm_client == -1){
        fprintf(stderr,"shm_open %s",st_shm_client);
        return EXIT_FAILURE;
    }

    /* Truncate and map SHM*/
    if(ftruncate(shm_client,sizeof(struct request)) == -1){
        perror("ftruncate");
        return EXIT_FAILURE;
    }

    msg_in = mmap(NULL,sizeof(struct request),
            PROT_READ|PROT_WRITE,MAP_SHARED,shm_server,0);
    if(msg_in == MAP_FAILED){
        perror("mmap");
        return EXIT_FAILURE;
    }

    /* Connect */
    printf("Connecting to %s...\n",argv[1]);
    sem_wait(sem_msgwrite);
    msg->type = 0;
    sprintf(msg->content,"%s",argv[2]);
    sem_post(sem_msgread);
    printf("Connected.\n");

    /* Main Loop */
    int i;
    int ir = 1;

    pthread_t t;
    pthread_create(&t, NULL, display_thread, NULL);

    while (ir > 0 && run) 
    {
        for (i = 0; i < BUF_SIZE ; i++)
            buf[i] = '\0';
        if ((ir = read(STDIN_FILENO, buf, BUF_SIZE)) == -1)
            perror("read");
        if(strcmp(buf,"exit\n") == 0)
            break;
        sem_wait(sem_msgwrite);
        msg->type = 1;
        strcpy(msg->content, buf);
        sem_post(sem_msgread);
        strcpy(last, buf);
    }

    /* Disconnect */
    printf("Disconnecting from %s...\n",argv[1]);
    sem_wait(sem_msgwrite);
    msg->type = 2;
    sprintf(msg->content,"%s",argv[2]);
    sem_post(sem_msgread);
    printf("Disconnected.\n");
 

    /* Closing and unlinking shared ressources*/
    sem_close(sem_msgread);
    sem_close(sem_msgwrite);
    sem_close(sem_read);
	sem_close(sem_write);
	munmap(msg,sizeof(struct request));
	munmap(msg_in,sizeof(struct request));
 	
	sem_unlink(st_read);
 	sem_unlink(st_write);
	shm_unlink(st_shm_client);

    return EXIT_SUCCESS;
}
