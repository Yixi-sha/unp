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
    while(1){
        remoteLen = sizeof(remoteAddr);
        recvLen = Recvfrom(serverFd, buf, RECVBUFLEN, 0, (struct sockaddr*)(&remoteAddr), &remoteLen);
        if(recvLen >= 0){
            puts(buf);
            Sendto(serverFd, buf, recvLen, 0,(struct sockaddr*)(&remoteAddr), remoteLen);
        }
            

    }

    close(serverFd);

    exit(0);
}
