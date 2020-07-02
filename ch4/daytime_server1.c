#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "../wrap_func.h"

int main(int argc, char* argv[]){

    int listenfd, connfd;
    struct sockaddr_in serveraddr;
    socklen_t cLen;
    struct sockaddr Caddr;
    char buff[4096];
    time_t ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(13);

    Bind(listenfd, (struct sockaddr*)(&serveraddr), sizeof(serveraddr));

    listen(listenfd, 10);
    while(1){
        cLen = sizeof(Caddr);
        connfd = Accept(listenfd, &Caddr, &cLen);
        printf("connect from %s\n", Sock_ntop(&Caddr, cLen));
        ticks = time(NULL);

        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }


    exit(0);
}