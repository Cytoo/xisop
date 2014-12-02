#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if(argc < 2 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s ARGUMENTS...\n", argv[0]);
        return EXIT_FAILURE;
    }

    int nsocks = argc - 1;
    int* sockets = malloc(nsocks * sizeof(int));
    int* ports = malloc(nsocks * sizeof(int));
    fd_set set;

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    socklen_t socklen;

    for(int i = 0; i < nsocks; ++i) {
        int port = atoi(argv[i + 1]);
        ports[i] = port;
        sockaddr.sin_port = htons(port);

        sockets[i] = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        bind(sockets[i], (struct sockaddr*) &sockaddr, sizeof(sockaddr));
        listen(sockets[i], 10);

        FD_SET(sockets[i], &set);
    }

    FILE* cxlog = fopen("cxlog", "w");

    for(;;) {
        fd_set s = set;
        select(sockets[nsocks - 1] + 1, &s, NULL, NULL, NULL);
    
        for(int i = 0; i < nsocks; ++i) {
            if(FD_ISSET(sockets[i], &s)) {
                if(accept(sockets[i], (struct sockaddr*) &sockaddr, &socklen) >= 0) {
                    fprintf(cxlog, "connection established with %s on port %d\n",
                        inet_ntoa(sockaddr.sin_addr),
                        ports[i]);
                    fflush(cxlog);
                } else {
                    perror("failure\n");
                    exit(1);
                }

                break;
            }
        }
    }

    return 0;
}
