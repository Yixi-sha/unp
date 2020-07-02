#include "../wrap_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9877

#define err(s) do{  \
    printf(s); \
    printf(" %s %s %d\n", strerror(errno), __FILE__, __LINE__); \
    exit(1); \
}while(0)

void echo_client(int socketFd, FILE* fp){
    char sendLine[MAXLINE], receiveLine[MAXLINE];

    while(fgets(sendLine, MAXLINE, fp) != NULL){
        Writen(socketFd, sendLine, 2);

        if(Readline(socketFd, receiveLine, MAXLINE) == 0){
            
            err("server end\n");
        }
        //printf("%s\n", receiveLine);
        puts(receiveLine);
    }
}

int main(int argc, char* argv[]){
    struct sockaddr_in deslAddr[5];
    int socketFd = 0, firstScoketFd = 0; 

    if(argc != 2){
        printf("input ip\n");
        exit(0);
    }

    for(int i = 0; i < 5; ++i){
        bzero(&deslAddr[i], sizeof(deslAddr[i]));
        socketFd = Socket(AF_INET, SOCK_STREAM, 0);

        Inet_pton(AF_INET, argv[1], &deslAddr[i]);
        deslAddr[i].sin_port = htons(PORT);
        deslAddr[i].sin_family = AF_INET;

        Connect(socketFd, (struct sockaddr*)(&deslAddr[i]), sizeof(deslAddr[i]));
        if(i == 0){
            firstScoketFd = socketFd;
        }
    }


    echo_client(firstScoketFd, stdin);

    close(firstScoketFd);
    exit(0);
}