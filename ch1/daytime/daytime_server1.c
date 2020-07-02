#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

extern int Socket(int family, int type, int protocol);
extern void Pthread_mutex_lock(pthread_mutex_t *mptr);
extern int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int main(int argc, char* argv[]){

    int listenfd, connfd;
    struct sockaddr_in serveraddr;
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
        connfd = Accept(listenfd, NULL, NULL);
        ticks = time(NULL);

        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }


    exit(0);
}