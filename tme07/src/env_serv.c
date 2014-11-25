#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ARG_SIZE 64

struct msg_env {
    char cmd;
    char id[ARG_SIZE];
    char val[ARG_SIZE];
    int ack;
};

int main(int argc, char **argv)
{
    struct sockaddr_in serv;
    struct sockaddr_in client;
    struct msg_env msg;
    int addr_len = sizeof(client);
    int running = 1;
    int port;
    int sc;

    if(argc != 2 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s PORT\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Read port */
    if((port = atoi(argv[1])) < 0) {
        perror("invalid port number");
        return EXIT_FAILURE;
    }

    /* Open and bind socket */
    if((sc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("unable to open socket");
        return EXIT_FAILURE;
    }

    memset((char *)&serv, 0, sizeof(serv));
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);
    serv.sin_family = AF_INET;

    if(bind(sc, (struct sockaddr *)&serv, sizeof(serv)) < 0 ){
        perror("bind");
        return EXIT_FAILURE;
    }


    printf("addr : %s\n", inet_ntoa(serv.sin_addr));
    printf("port : %d / %d\n", port, ntohs(serv.sin_port));

    /* Main loop */
    char *val;
    while(running){
        /* Wait for message*/
        if(recvfrom(sc, (void *)&msg, sizeof(msg), 0,
                    (struct sockaddr *)&client,
                    (socklen_t *)&addr_len) == -1 ){
            perror("recvfrom");
            return EXIT_FAILURE;
        }

        /* Parse and handle message */
        /* Same message will be sent back with updated
         * msg.val and msg.ack */
        printf("message received: %c %s %s\n", msg.cmd, msg.id, msg.val);
        msg.ack = 0;
        if(msg.cmd == 'S')
            msg.ack = setenv(msg.id,msg.val,1);
        else if(msg.cmd == 'G'){
            val = getenv(msg.id);
            if(val != NULL)
                strcpy(msg.val,val);
            else
                msg.ack = -1;
        }
        else
            msg.ack = -1;

        /* Send message back */
        if (sendto(sc, (void *)&msg, sizeof(msg), 0,
                    (struct sockaddr *)&client, (socklen_t)addr_len)  == -1) {
            perror("sendto"); 
            return EXIT_FAILURE;
        }
    }
    return 0;
}
