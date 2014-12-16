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

    fdr = open("../src/async.c", O_RDONLY, 0600);
    fdw = open("../wacky", O_WRONLY,0600);

    for(i = 0; i < NBCHAR; i ++){
        a[i].aio_fildes = fdw;
        a[i].aio_nbytes = 1;
        a[i].aio_reqprio = 0;
        a[i].aio_lio_opcode = LIO_WRITE;
        list[i] = &(a[i]);
    }


    while ( (rd = read(fdr,buf,10)) > 0 ){
        for(i = 0; i < rd ; i++){
            a[i].aio_offset = offset++ ;
            a[i].aio_buf = &buf[rd-i-1] ;
        }

        lio_listio(LIO_WAIT,list,rd,NULL);
    }

    return 0;
}
