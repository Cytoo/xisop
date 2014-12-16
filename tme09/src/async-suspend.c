#define _POSIX_C_SOURCE 199309L
#define _POSIX_SOURCE 1

#include <aio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
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

    struct aiocb const* const aios = &aio;
    aio_suspend(&aios, 1, NULL);

    char* buf = malloc(len);
    aio.aio_fildes = fd;
    aio.aio_buf = buf;
    aio.aio_nbytes = len;
    aio_read(&aio);
    aio_suspend(&aios, 1, NULL);

    printf("read: %s\n", buf);

    return 0;
}
