#include "../wrap_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>


#define PORT 9877



void echo_client(int socketFd, FILE* fp, struct sockaddr *desAddr, socklen_t desAddrlen ){
    char sendLine[MAXLINE], receiveLine[MAXLINE];
    fd_set readSet;
    FD_ZERO(&readSet);
    int maxFD = 0;
    int selectRet = 0;

    sendLine[0] = '\0';
    while(1){
        
        FD_SET(STDIN_FILENO, &readSet);
        if(STDIN_FILENO > maxFD)
            maxFD = STDIN_FILENO;
        
        FD_SET(socketFd, &readSet);
        if(socketFd > maxFD)
            maxFD = socketFd;
        
        selectRet =  Select(maxFD + 1, &readSet, NULL, NULL, NULL);
        if(selectRet < 0 ){
            printf("select error");
            break;
        }
        if(FD_ISSET(STDIN_FILENO ,&readSet)){
            if(fgets(sendLine, MAXLINE, fp) != NULL){ 
                printf("input  %s\n", sendLine);
                if(Sendto(socketFd, sendLine, strlen(sendLine), 0, desAddr, desAddrlen ) < 0){
                    printf("write end\n");
                    break;
                }
            }else{
                printf("fgets end\n");
                break;
            }
        }
        if(FD_ISSET(socketFd ,&readSet)){
            int ret = Read(socketFd, receiveLine, MAXLINE);
            if(ret < 0){ 
                err("Readline error");
            }
            receiveLine[ret] = 0;
            printf("output %s\n", receiveLine);
        }

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