#include "../wrap_func.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>




#define PORT 9877
#define RECVBUFLEN 4096
#define LISTENQ 20

void sig_chld(int signo){
    int state;
    pid_t pid;

    printf("entry sig_chld\n");
    while ( (pid = waitpid(-1, &state, WNOHANG)) > 0)
        printf("pid is %d state is %d\n", pid, state);

}

void echo_server(int sockfd){
    int n = 0;
    char buf[MAXLINE];
    while(1){
        while((n = read(sockfd, buf, MAXLINE)) > 0){
            printf("tcp  %s\n", buf);
            n = Writen(sockfd, buf, n);
        }
        if(n < 0 && errno == EINTR){
            continue;
        }else{
            break;
        }
    } 
}

int main(){
    struct sockaddr_in localAddrUDP, remoteAddr, localAddrTCP;
    int tcpFd, udpFd;
    int MAXFD = 0;
    int selectRet = 0;
    socklen_t remoteLen = 0;
    char buf[RECVBUFLEN];

    bzero(&localAddrUDP, sizeof(localAddrUDP));
    localAddrUDP.sin_family = AF_INET;
    localAddrUDP.sin_port = htons(PORT);
    localAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
    udpFd = Socket(AF_INET, SOCK_DGRAM, 0);
    Bind(udpFd, (struct sockaddr*)&localAddrUDP, sizeof(localAddrUDP));

    bzero(&localAddrTCP, sizeof(localAddrTCP));
    localAddrTCP.sin_family = AF_INET;
    localAddrTCP.sin_port = htons(PORT);
    localAddrTCP.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpFd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(tcpFd, (struct sockaddr*)&localAddrTCP, sizeof(localAddrTCP));

    Signal(SIGCHLD, sig_chld);

    fd_set readFdSet;
    FD_ZERO(&readFdSet);

    Listen(tcpFd, LISTENQ);

    while(1){
        FD_SET(tcpFd, &readFdSet);
        MAXFD = tcpFd;

        FD_SET(udpFd, &readFdSet);
        if(udpFd > MAXFD)
            MAXFD = udpFd;
        selectRet = Select(MAXFD + 1, &readFdSet, NULL, NULL, NULL);

        if(FD_ISSET(udpFd, &readFdSet)){
            remoteLen = sizeof(remoteAddr);
            int recvLen = Recvfrom(udpFd, buf, RECVBUFLEN, 0, (struct sockaddr*)(&remoteAddr), &remoteLen);
            if(recvLen >= 0){
                puts(buf);
                Sendto(udpFd, buf, recvLen, 0,(struct sockaddr*)(&remoteAddr), remoteLen);
            }
        }
        if(FD_ISSET(tcpFd, &readFdSet)){
            int accpetFd;
            pid_t cPid;
            remoteLen = sizeof(remoteAddr);
            if((accpetFd = accept(tcpFd, (struct sockaddr*)(&remoteAddr), &remoteLen)) < 0){
                if(errno == EINTR){
                    continue;
                }else{
                    err("Accept error");
                }
            }
            if((cPid = fork()) == 0){
                printf("child %d\n", getpid());
                close(tcpFd);
                echo_server(accpetFd);
                close(accpetFd);
                printf("child end\n");
                exit(0);
            }else if(cPid > 0){
                close(accpetFd);
            }else{
                printf("fork error\n");
            }
        }
    }



    exit(0);
}