#include "../wrap_func.h"

void echo_client(int socketFd){
    char buf[MAXLINE];
    fd_set readSet;
    int MaxFd=0;
    int n = 0;
    int shut = 0;
    FD_ZERO(&readSet);
    

    while(1){
        FD_SET(STDIN_FILENO, &readSet);
        MaxFd = STDIN_FILENO;
        FD_SET(socketFd, &readSet);
        if(socketFd > MaxFd)
            MaxFd = socketFd;
        Select(MaxFd + 1, &readSet, NULL, NULL, NULL);

        if(FD_ISSET(STDIN_FILENO, &readSet)){
            n = Read(STDIN_FILENO, buf, MAXLINE);
            if(n == 0){
                shutdown(socketFd, SHUT_WR);
                shut = 1;
            }else{
                if(shut == 1){
                    err("socket shut\n");
                }
                Writen(socketFd, buf, n);
            }
        }
        if(FD_ISSET(socketFd, &readSet)){
            n = Read(socketFd, buf, MAXLINE);
            if(n == 0){
                printf("end\n");
                break;
            }
            buf[n] = '\0';
            puts(buf);
        }
    }
}

int main(int argc, char *argv[]){
    int socketFd;
    if(argc == 3){
        socketFd = Tcp_connect(argv[1], argv[2], AF_INET);
    }else{
        printf("usage <hostname> <service>\n");
        exit(1);
    }

    echo_client(socketFd);

    exit(0);
}
