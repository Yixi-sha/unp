#include "../wrap_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9877

struct Args{
    long arg1;
    long arg2;
};

struct Result{
    long res;
};

void sum_client(int socketFd, FILE* fp){
    char sendLine[MAXLINE], receiveLine[MAXLINE];
    struct Args arg;
    struct Result res;
    while(fgets(sendLine, MAXLINE, fp) != NULL){
        
        sscanf(sendLine, "%ld%ld", &arg.arg1, &arg.arg2);
        Writen(socketFd, &arg, sizeof(arg));
        Writen(socketFd, "\n", 1);
        if(Readn(socketFd, &res, sizeof(res)) == 0){
            err("server end\n");
        }
        printf("%ld\n", res.res);
        
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
    socketFd = Socket(AF_INET, SOCK_STREAM, 0);

    Inet_pton(AF_INET, argv[1], &deslAddr);
    deslAddr.sin_port = htons(PORT);
    deslAddr.sin_family = AF_INET;

    Connect(socketFd, (struct sockaddr*)(&deslAddr), sizeof(deslAddr));
    sum_client(socketFd, stdin);

    close(socketFd);
    
}