#include "../wrap_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define PORT 9877
#define RECVBUFLEN 4096

long long count = 0;

static void sig_int(int signo){
    printf("count is %lld\n", count);
    exit(0);
}

void echo_server(int socketFd,struct sockaddr *remoteAddr, socklen_t remoteAddrlen ){
    socklen_t len;
    char msg[RECVBUFLEN];

    while(1){
        len = remoteAddrlen;
        Recvfrom(socketFd , msg, RECVBUFLEN, 0, remoteAddr, &remoteAddrlen);
        ++count;
    }
}

int main(){
    int serverFd = 0;
    struct sockaddr_in localAddr, remoteAddr;
    socklen_t localLen = 0, remoteLen = 0;
    pid_t cPid;
    ssize_t recvLen = 0;

    
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(PORT);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    serverFd = Socket(AF_INET, SOCK_DGRAM, 0);
    Bind(serverFd, (struct sockaddr*)(&localAddr), sizeof(localAddr));

    char buf[RECVBUFLEN];
    Signal(SIGINT, sig_int);
    echo_server(serverFd, (struct sockaddr*)(&remoteAddr), sizeof(remoteAddr));

    close(serverFd);

    exit(0);
}
