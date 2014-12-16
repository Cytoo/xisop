#define _XOPEN_SOURCE 700

#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void on_sig(int sig) { (void) sig; }

int main(int argc, char **argv)
{
    if(argc != 3 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s FILE TEXT...\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* file = argv[1];
    char* text = argv[2];
    int len = strlen(text);

    int fd = open(file, O_WRONLY | O_CREAT, 0600);
    struct aiocb aio;
    aio.aio_fildes = fd;
    aio.aio_buf = text;
    aio.aio_nbytes = len;
    aio.aio_offset = 0;
    aio.aio_reqprio = 0;
    aio.aio_sigevent.sigev_notify = SIGEV_NONE;
    aio_write(&aio);

    fd = open(file, O_RDONLY, 0600);

    struct timespec tspec;
    tspec.tv_sec = 0;
    tspec.tv_nsec = 50;

    while(aio_error(&aio) == EINPROGRESS)
        clock_nanosleep(CLOCK_REALTIME, 0, &tspec, NULL);

    char* buf = malloc(len);
    read(fd, buf, len);

    printf("read: %s\n", buf);

    return 0;
}
