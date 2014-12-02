#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>

#define BROADCAST_IP "255.255.255.255"
#define BC_PORT 9999
#define MSG_SIZE 256
#define USR_SIZE 32

struct msg_chat {
    char buf[MSG_SIZE];
    char usr[USR_SIZE];
};

int sock_bc;

void PrintHandler()
{

    struct sockaddr_in exp;
    unsigned int len;
    struct msg_chat msg;

    len = sizeof(exp);
    if (recvfrom(sock_bc, &msg, sizeof(msg), 0,
                (struct sockaddr *) &exp, &len) <0)
        perror("recvfrom");
    else
        printf("%s : %s", msg.usr, msg.buf);
}



int main(int argc, char *argv[]) {

    struct sigaction handler;
    struct sockaddr_in servad;
    struct sockaddr_in dest;
    struct msg_chat msg;
    int mode, port;
    int on = 1, running = 1, ir = 1, i = 0;

    if(argc != 4 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s ADDR PORT USERNAME\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Read port */
    if((port = atoi(argv[2])) < 0) {
        fprintf(stderr,"invalid port number");
        return EXIT_FAILURE;
    }

    if(strlen(argv[3]) > USR_SIZE ){
        fprintf(stderr,"username size must be less than %d\n",
                USR_SIZE);
        return EXIT_FAILURE;
    }
    strcpy(msg.usr,argv[3]);

    if ((sock_bc = socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("creation socket");
        exit(1);
    }

    setsockopt(sock_bc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    setsockopt(sock_bc, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    /* Bind socket */
    memset((void *)&servad,0, sizeof(servad));
    servad.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    servad.sin_family = AF_INET;
    servad.sin_port = htons(port);
    if (bind(sock_bc,(struct sockaddr *)&servad,sizeof(servad)) < 0) {
        perror("bind");
        exit(2);
    }

    /* We need two processes otherwise signal reception interrupts read */
    if(fork()){
        /* Set reception Handler */
        handler.sa_handler = PrintHandler;
        sigfillset(&handler.sa_mask) ;
        handler.sa_flags = 0;

        if (sigaction(SIGIO, &handler, 0) < 0) {
            perror("sigaction");
            return EXIT_FAILURE ;
        }
        if (fcntl(sock_bc, F_SETOWN, getpid()) < 0) {
            perror ("fcntl");
            return EXIT_FAILURE;
        }
        mode = fcntl ( sock_bc, F_GETFL);
        if (fcntl(sock_bc, F_SETFL, mode| O_NONBLOCK |
                    O_ASYNC) < 0){
            perror ("fcntl");
            return EXIT_FAILURE;
        }
        while(running)
            pause();
    }
    else {
        dest.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        dest.sin_family = AF_INET;
        dest.sin_port = htons(BC_PORT);

        /* Main loop */
        while(running && ir > 0) {

            for (i = 0; i < MSG_SIZE ; i++)
                msg.buf[i] = '\0';
            if ((ir = read(STDIN_FILENO, msg.buf, MSG_SIZE)) == -1)
                perror("read");

            if (sendto(sock_bc, &msg, sizeof(msg), 0, (struct sockaddr *)&dest, sizeof(dest)) == -1) {
                perror("sendto");
                exit(1);
            }
        }
    }

    close(sock_bc);

    return 0;
}
