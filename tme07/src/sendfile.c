#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 512

struct msg {
    size_t len;
    char buf[BUFSIZE];
};

int main(int argc, char **argv)
{
    struct sockaddr_in serv;
    struct addrinfo *info;
    char buf[BUFSIZE];
    struct msg m;
    int addr_len = sizeof(serv);
    int sc;
    int port;

    if(argc != 4 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s ADDR PORT FILE\n", argv[0]);
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

    memset((char*) &serv, 0, sizeof(serv));
    memcpy((void*) &serv.sin_addr,
            (void*)&((struct sockaddr_in*)(info->ai_addr))->sin_addr,
            sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);

    printf("addr : %s / %s\n", argv[1], inet_ntoa(serv.sin_addr));
    printf("port : %d / %d\n", port, ntohs(serv.sin_port));

    int fd;
    if((fd = open(argv[3], O_RDONLY)) == -1) {
        perror("couldn't open file");
        return EXIT_FAILURE;
    }

    strcpy(buf, argv[3]);

    /* Send path */
    sendto(sc,(void*) buf, BUFSIZE, 0, (struct sockaddr*) &serv, (socklen_t) addr_len);

    while((m.len = read(fd, m.buf, BUFSIZE))) {
        sendto(sc, (void*) &m, sizeof(m), 0, (struct sockaddr*) &serv, (socklen_t) addr_len);
    }

    close(sc);
    close(fd);
    return 0;
}
