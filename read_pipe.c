#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 1024

int main(int argc, char **argv){


    int fd;
    char *my_fifo = "/tmp/my_fifo";
    char buf[MAX_BUF];

    /* create pipe */
    mkfifo(my_fifo, 0666);

    /* write to fifi */
    /* fd = open(my_fifo, O_WRONLY); */
    /* write(fd, "hi", sizeof("hi")); */
    fd = open(my_fifo, O_RDONLY);
    read(fd, buf, MAX_BUF);
    printf("Received %s\n", buf); 
    close(fd);

    return 0;
}
