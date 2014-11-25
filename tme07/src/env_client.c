#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 128
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
    struct addrinfo *info;
    struct msg_env msg;
    char buf[BUF_SIZE];
    int addr_len = sizeof(serv);
    int running = 1;
    int sc;
    int port;

    if(argc != 3 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s ADDR PORT\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Read port */
    if((port = atoi(argv[2])) < 0) {
        perror("invalid port number");
        return EXIT_FAILURE;
    }

    /* Open socket */
    if((sc = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("unable to open socket");
        return EXIT_FAILURE;
    }

    /* Get server address and set up server sockaddr */
    if (getaddrinfo(argv[1], NULL, NULL, &info) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    memset((char *)&serv, 0, sizeof(serv));
    memcpy((void*)&serv.sin_addr,
            (void*)&((struct sockaddr_in*)(info->ai_addr))->sin_addr,
            sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);

    printf("addr : %s / %s\n", argv[1], inet_ntoa(serv.sin_addr));
    printf("port : %d / %d\n", port, ntohs(serv.sin_port));



    /* Main loop */
    int ir = 1;
    int i;
    while(ir > 0 && running){
        /* Clear buffers*/
        msg.cmd = 0;
        for(i=0; i<ARG_SIZE; i++){ 
            msg.id[i] = '\0';
            msg.val[i] = '\0';
        }

        for (i = 0; i < BUF_SIZE ; i++)
            buf[i] = '\0';


        /* Read user input*/
        if((ir = read(STDIN_FILENO,buf,BUF_SIZE)) == -1 ){
            perror("read");
        }

        sscanf(buf,"%c %s %s", &msg.cmd, msg.id, msg.val);

        /* Check user input validity */
        if((msg.cmd != 'S' && msg.cmd != 'G') ||
                (strlen(msg.id) <= 0) ||
                (msg.cmd == 'S' && (strlen(msg.val) <= 0))){
            printf("commands :\n");
            printf("\tS <identificateur> <valeur>\n");
            printf("\tG <identificateur>\n");
            continue;
        } 
    
        /* Send to server*/
        if (sendto(sc,(void *)&msg,sizeof(msg),0,
                    (struct sockaddr *)&serv,(socklen_t)addr_len)  == -1) {
            perror("sendto"); 
            return EXIT_FAILURE;
        }

        /* Wait for response */
        if(recvfrom(sc, (void *)&msg, sizeof(msg), 0,
                    (struct sockaddr *)&serv, (socklen_t *)&addr_len) == -1 ){
            perror("recvfrom");
            return EXIT_FAILURE;
        }

        /* Print response */
        if(msg.cmd == 'S' && msg.ack == 0)
                printf("Success !\n");
        else if (msg.cmd == 'G' && msg.ack == 0)
            printf("%s = %s\n", msg.id, msg.val);
        else 
            printf("Failure.\n");
    }

    return 0;
}
