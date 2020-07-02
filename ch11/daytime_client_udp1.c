#include "../wrap_func.h"

int main(int argc, char *argv[]){
    int socketfd = -1;
    struct sockaddr *remoteAddr = NULL;
    socklen_t remoteAddrLen;
    char buf[MAXLINE];
    int n = 0;

    if(argc != 3){
        printf("usage a.out <hostname> <service>");
        exit(1);
    }
    //socketfd =  Udp_client(argv[1], argv[2], &remoteAddr, &remoteAddrLen);
    socketfd =  Udp_client_connect(argv[1], argv[2]);
    //printf("sending to %s\n",Sock_ntop(remoteAddr, remoteAddrLen));
    //Sendto(socketfd, " ",1, 0, remoteAddr, remoteAddrLen);
    Write(socketfd, " ",1);
    n = Read(socketfd, buf, MAXLINE - 1);
    printf("get\n");
    buf[n] = '\0';
    fputs(buf, stdout);

    exit(0);
}