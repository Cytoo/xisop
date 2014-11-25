#define _XOPEN_SOURCE 700

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PORT 5000

int main(int argc, char** argv)
{
    int i, nb_proc, total, sock;
    struct sockaddr_in sockaddr;

    if(argc != 2 || !strcmp(argv[1], "-h"))
    {
        fprintf(stderr, "usage: %s NB_THREADS\n", argv[0]);
        return EXIT_FAILURE;
    }

    nb_proc = atoi(argv[1]);
    total = 0;

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    for(i = 0; i < nb_proc; ++i)
    {
        if(fork() == 0)
        {
            srand(time(NULL) ^ (getpid() << 16));
            int rand_val = (int) (10 * (float) rand() / RAND_MAX);

            sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
            
            printf("oh\n");
            sendto(sock, (void*) &rand_val, sizeof(int), 0, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
            close(sock);

            return EXIT_SUCCESS;
        }
    }

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bind(sock, (struct sockaddr*) &sockaddr, sizeof(sockaddr));
    printf("eh\n");

    for(i = 0; i < nb_proc; ++i)
    {
        int buf;
        socklen_t socklen;
        struct sockaddr_in emitter;
        recvfrom(sock, (void*) &buf, sizeof(int), 0, (struct sockaddr*) &emitter, &socklen);
        total += buf;
    }

    for(i = 0; i < nb_proc; ++i)
        wait(NULL);

    printf("total = %d\n", total);
    close(sock);

    return EXIT_SUCCESS;
}
