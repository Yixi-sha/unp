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

struct Args{
    long arg1;
    long arg2;
};

struct Result{
    long res;
};


void sig_chld(int signo){
    int state;
    pid_t pid;

    printf("entry sig_chld\n");
    while ( (pid = waitpid(-1, &state, WNOHANG)) > 0)
        printf("pid is %d state is %d\n", pid, state);

}

void sum_server(int sockfd){
    int n = 0;
    char buf[MAXLINE];
    struct Args *arg;
    struct Result res;
    while(1){
        while((n = Readline(sockfd, buf, MAXLINE)) > 0){
            printf("%s\n", buf);
            if(n == (sizeof(struct Args) + 1)){
                arg = (struct Args *)buf;
                res.res = arg->arg1 + arg->arg2;
                printf("rece %ld %ld\n", arg->arg1, arg->arg2);
                n = Writen(sockfd, &res, sizeof(res));
            }else{
               snprintf(buf, MAXLINE,"input error\n"); 
               n = Writen(sockfd, buf, strlen(buf));
            }
            //printf("%d\n", n);
        }
        if(n < 0 && errno == EINTR){
            continue;
        }else{
            break;
        }
    }
    
    
}

int main(){
    int listenFd = 0, accpetFd = 0;
    struct sockaddr_in localAddr, acceptAddr;
    socklen_t localLen = 0, acceptLen = 0;
    pid_t cPid;

    
    bzero(&localAddr, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(PORT);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenFd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(listenFd, (struct sockaddr*)(&localAddr), sizeof(localAddr));

    Listen(listenFd, LISTENQ);
    Signal(SIGCHLD, sig_chld);
    while(1){
        socklen_t cLen = sizeof(acceptAddr);
        printf("parent %d\n", getpid());
        if((accpetFd = accept(listenFd, (struct sockaddr*)(&acceptAddr), &cLen)) < 0){
            if(errno == EINTR){
                continue;
            }else{
                err("Accept error");
            }
        }
        if((cPid = fork()) == 0){
            printf("child %d\n", getpid());
            close(listenFd);
            sum_server(accpetFd);
            close(accpetFd);
            printf("child end\n");
            exit(0);
        }else if(cPid > 0){
            close(accpetFd);
        }else{
            printf("fork error\n");
        }

    }


    exit(0);
}
