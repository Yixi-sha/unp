#include "../wrap_func.h"
#include <signal.h>
#include <sys/wait.h>


void sig_child(int sig){
    int state;
    pid_t pid;

    printf("entry sig_chld\n");
    while ( (pid = waitpid(-1, &state, WNOHANG)) > 0)
        printf("pid is %d state is %d\n", pid, state);
}

void echo_server(int clientFd){
    char buf[MAXLINE];
    int n;
    while(1){
        n = Read(clientFd, buf, MAXLINE);
        if(n == 0){
            shutdown(clientFd, SHUT_RD);
            break;
        }
        Writen(clientFd, buf, n);
    }
}

int main(int argc, char* argv[]){
    int socketFd;
    int clientFd;
    pid_t pid;
    
    if(argc == 2){
        socketFd = Tcp_listen(NULL, argv[1], NULL, AF_INET);
    }else if(argc == 3){
        socketFd = Tcp_listen(argv[1], argv[2], NULL, AF_INET);
    }else{
        printf("usage [hostname] <service>\n");
        exit(1);
    }
    Signal(SIGCHLD, sig_child);

    while(1){
        clientFd = Accept(socketFd, NULL, NULL);
        if((pid = fork()) == 0){
            close(socketFd);
            echo_server(clientFd);
            exit(0);
        }else if(pid > 0){
            close(clientFd);
        }else{
            err("fork error\n");
        }
    }

    exit(0);
}