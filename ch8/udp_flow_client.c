#include "../wrap_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>


#define PORT 9877
#define NDG 2000
#define DGLEN 1400


void echo_client(int socketFd, FILE* fp, struct sockaddr *desAddr, socklen_t desAddrlen ){
    char sendLine[DGLEN];
    int i;

    for(i = 0; i < NDG; ++i)
        Sendto(socketFd, sendLine, DGLEN, 0, desAddr, desAddrlen);
}

int main(int argc, char* argv[]){
    struct sockaddr_in deslAddr;
    int socketFd = 0; 

    if(argc != 2){
        printf("input ip\n");
        exit(0);
    }

    bzero(&deslAddr, sizeof(deslAddr));
    socketFd = Socket(AF_INET, SOCK_DGRAM, 0);

    Inet_pton(AF_INET, argv[1], &deslAddr);
    deslAddr.sin_port = htons(PORT);
    deslAddr.sin_family = AF_INET;
    
   
    echo_client(socketFd, stdin, (struct sockaddr *)(&deslAddr), sizeof(deslAddr));

    close(socketFd);
    
}