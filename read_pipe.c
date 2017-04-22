#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 1024

void parse_input(char *pkt, char **arg1, char **arg2); 

int main(int argc, char **argv){


    int fd;
    char *my_fifo = "/tmp/my_fifo";
    /* char *buf = malloc(MAX_BUF); */
    char buf[MAX_BUF];
    char *arg1 = malloc(32);
    char *arg2 = malloc(256);

    /* create pipe */
    mkfifo(my_fifo, 0666);

    /* write to fifi */
    /* fd = open(my_fifo, O_WRONLY); */
    /* write(fd, "hi", sizeof("hi")); */
    while(1){
        memset(&buf[0], 0, sizeof(buf));
        fd = open(my_fifo, O_RDONLY);
        read(fd, buf, MAX_BUF);
        printf("Received %s\n", buf); 
        close(fd);

        parse_input(buf, &arg1, &arg2);

        printf("Arg1: %s, len: %d\n", arg1, (int)strlen(arg1));
        printf("Arg2: %s, len: %d\n", arg2, (int)strlen(arg2));
    }

    return 0;
}

int word_count(char *s){
    int i;
    int count = 1;

    for (i = 0;s[i] != '\0';i++){
        if (s[i] == ' ')
            count++;    
    }

    return count;
}

void parse_input(char *pkt, char **arg1, char **arg2){
    int count = word_count(pkt);
    printf("%d\n", count);

    if(count > 1){
        *arg1 = strtok(pkt, " ");
        *arg2 = strtok(NULL, " ");
    }
    else {
        *arg1 = strtok(pkt, " ");
    }
}
