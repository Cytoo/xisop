#define _XOPEN_SOURCE 700

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
    struct sockaddr_in client;
    char buf[BUFSIZE];
    int addr_len = sizeof(client);
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

    recvfrom(sc, (void*) buf, BUFSIZE, 0, (struct sockaddr*) &client, (socklen_t*) &addr_len);
    printf("opening file %s\n", buf);

    int fd;
   
    if((fd = open(buf, O_WRONLY | O_CREAT | O_EXCL)) == -1) {
        perror("couldn't open output file for writing");
        return EXIT_FAILURE;
    }

    struct msg m;

    for(;;) {
        /* Wait for message*/
        if(recvfrom(sc, (void*) &m, sizeof(m), 0, (struct sockaddr*) &client,
                    (socklen_t*) &addr_len) != sizeof(m)) {
            break;
        }

        write(fd, (void*) m.buf, m.len);
    }

    return 0;
}
