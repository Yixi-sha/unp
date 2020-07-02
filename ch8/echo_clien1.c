#include "../wrap_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>


#define PORT 9877



void echo_client(int socketFd, FILE* fp, struct sockaddr *desAddr, socklen_t desAddrlen ){
    char sendLine[MAXLINE], receiveLine[MAXLINE];
    int ret = 0;
    socklen_t receiveLen;
    sendLine[0] = '\0';

    Connect(socketFd, desAddr, desAddrlen);
    getsockname(socketFd, desAddr, &desAddrlen);
    printf("local address %s\n", Sock_ntop(desAddr, desAddrlen));

    while(fgets(sendLine, MAXLINE, fp) != NULL){
        if(Write(socketFd, sendLine, strlen(sendLine)) < 0){
            printf("write end\n");
            break;
        }
        
        ret = Read(socketFd, receiveLine, MAXLINE);
        if(ret < 0){ 
            err("Readline error");
        }
        receiveLine[ret] = 0;
        
        puts(receiveLine);
    }
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