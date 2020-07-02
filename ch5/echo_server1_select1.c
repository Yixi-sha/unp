#include "../wrap_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define PORT 9877
#define LISTENQ 20
#define MAXFDNUM 1024


int main(){
    int listenFd = 0, accpetFd = 0;
    struct sockaddr_in localAddr, acceptAddr;
    socklen_t localLen = 0, acceptLen = 0;
    pid_t cPid;
    int clientFd[MAXFDNUM];
    int maxIndex = 0;
    int maxFd = 0;
    int i = 0;
    int selectRet = 0;
    fd_set readFdSet;
    char buf[MAXLINE];
    int receiveNUM = 0;

    for(; i < MAXFDNUM; ++i)
        clientFd[i] = -1;
    
    FD_ZERO(&readFdSet);

    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(PORT);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenFd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenFd, (struct sockaddr*)(&localAddr), sizeof(localAddr));

    Listen(listenFd, LISTENQ);
    
    while(1){
        FD_SET(listenFd, &readFdSet);
        maxFd = listenFd;
        for(i = 0; i < maxIndex; ++i){
            if(clientFd[i] != -1){
                FD_SET(clientFd[i], &readFdSet);
                if(clientFd[i] > maxFd)
                    maxFd = clientFd[i];
            }
        }
        selectRet = Select(maxFd + 1, &readFdSet, NULL, NULL, NULL);
        if(FD_ISSET(listenFd, &readFdSet)){
            socklen_t cLen = sizeof(acceptAddr);
            accpetFd = Accept(listenFd, (struct sockaddr*)(&acceptAddr), &cLen);
            selectRet -= 1;
            printf("accept\n");
            for(i = 0; i < MAXFDNUM; ++i){
                if(clientFd[i] == -1){
                    clientFd[i] = accpetFd;
                    if((i + 1)> maxIndex)
                        maxIndex = i + 1;
                    break;
                }
            }
            if(i == MAXFDNUM)
                close(accpetFd);
        }
        for(i = 0; i < maxIndex && selectRet > 0; ++i){
            if(clientFd[i] != -1 && FD_ISSET(clientFd[i], &readFdSet)){
                if((receiveNUM = Read(clientFd[i], buf, MAXLINE)) > 0){
                    Writen(clientFd[i], buf, receiveNUM);
                }else if(receiveNUM == 0){
                    close(clientFd[i]);
                    FD_CLR(clientFd[i], &readFdSet);
                    clientFd[i] = -1;
                    printf("close\n");
                    if((i + 1) == maxIndex){
                        while(i >= 0 &&clientFd[i] == -1){
                            maxIndex--;
                            --i;
                        }
                        break;
                    }
                }else{
                    err("read error");
                }
                --selectRet;
            }
        }
    }

    close(listenFd);


    exit(0);
}
