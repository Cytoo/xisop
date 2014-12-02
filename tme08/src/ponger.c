#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if(argc != 1) {
        fprintf(stderr, "usage: %s ARGUMENTS...\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(9999);
    sockaddr.sin_addr.s_addr = INADDR_ANY;

    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    bind(sock, (struct sockaddr*) &sockaddr, sizeof(sockaddr));

    for(;;) {
        char buf[4];
        struct sockaddr_in sender;
        socklen_t socklen;
        recvfrom(sock, (void*) buf, 4, 0, (struct sockaddr*) &sender, &socklen);
        printf("received %c%c%c%c\n", buf[0], buf[1], buf[2], buf[3]);

        if(!strncmp(buf, "PING", 4)) {
            sendto(sock, "PONG", 4, 0, (struct sockaddr*) &sender, socklen);
        }
    }

    return 0;
}
