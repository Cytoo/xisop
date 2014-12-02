#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void* receiver(void* arg) {
    int sock = *((int*) arg);
    
    for(;;) {
        char buf[4];
        struct sockaddr_in sender;
        socklen_t socklen;
        recvfrom(sock, (void*) buf, 4, 0, (struct sockaddr*) &sender, &socklen);

        if(!strncmp(buf, "PONG", 4)) {
            printf("received PONG from %s\n", inet_ntoa(sender.sin_addr));
        }
    }
}

void* sender(void* arg) {
    int sock = *((int*) arg);
    struct sockaddr_in sockaddr;

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(9999);
    sockaddr.sin_addr.s_addr = INADDR_BROADCAST;
    
    for(;;) {
        printf("PING?\n");
        sendto(sock, "PING", 4, 0, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
        sleep(3);
    }
}

int main(int argc, char **argv)
{
    if(argc != 1) {
        fprintf(stderr, "usage: %s ARGUMENTS...\n", argv[0]);
        return EXIT_FAILURE;
    }

    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    int perm = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &perm, sizeof(int));

    pthread_t t;
    pthread_create(&t, NULL, sender, (void*) &sock);
    receiver((void*) &sock);

    return 0;
}
