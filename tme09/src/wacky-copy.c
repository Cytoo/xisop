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

#define NBCHAR 10

int main(int argc, char **argv){

    int fdr, fdw;
    int rd = 0, offset = 0;
    int i;
    char buf[NBCHAR];
    struct aiocb a[NBCHAR];
    struct aiocb * list[NBCHAR];


    if(argc != 3 || !strcmp(argv[1], "-h")) {
        fprintf(stderr, "usage: %s SRC_FILE DEST_FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Opening passed files  */
    fdr = open(argv[1], O_RDONLY, 0600);
    fdw = open(argv[2], O_WRONLY | O_CREAT,0600);


    /* Initialisation of unchanging aiocb values   */
    for(i = 0; i < NBCHAR; i ++){
        a[i].aio_fildes = fdw;
        a[i].aio_nbytes = 1;
        a[i].aio_reqprio = 0;
        a[i].aio_lio_opcode = LIO_WRITE;
        //a[i].aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        //a[i].aio_sigevent.sigev_signo = SIGRTMIN;
        list[i] = &(a[i]);
    }

    /* Main loop */
    while ( (rd = read(fdr,buf,10)) > 0 ){
        printf("%s\n",buf);
        /* Fill the aiocbs with read values */
        for(i = 0; i < rd ; i++){
            a[i].aio_offset = offset++ ;
            a[i].aio_buf = &buf[rd-i-1] ;
        }

        /* Send list */
        lio_listio(LIO_WAIT,list,rd,NULL);
    }

    /* We could use LIO_NOWAIT to gain some time, checking if all past actions
     * are completed after the read. That would only take an extra buffer to
     * ensure no value is overwritten before it has been copied.*/

    close(fdr);
    close(fdw);
    return EXIT_SUCCESS;
}
