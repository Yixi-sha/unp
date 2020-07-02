#include "../wrap_func.h"


int main(int argc, char* argv[]){
    int serverFd = -1;
    struct sockaddr clientAddr;
    socklen_t addrLen;
    char buf[MAXLINE];
    time_t ticks;
    int n = 0;

    if(argc == 3){
        serverFd = Udp_server(argv[1], argv[2], NULL);
    }else if(argc == 2){
        serverFd = Udp_server(NULL, argv[1], NULL);
    }else{
        printf("usage [hostname] <service>\n");
        exit(1);
    }

    if(serverFd == -1)
        err("Udp_server error\n");
    
    while(1){
        addrLen = sizeof(struct sockaddr);
        n = Recvfrom(serverFd, buf, MAXLINE, 0, &clientAddr, &addrLen);
        buf[n] = '\0';
        printf("%s\n", buf);
        printf("get from %s\n", Sock_ntop(&clientAddr, addrLen));
        snprintf(buf, MAXLINE, "%.24s\r\n", ctime(&ticks));
        Sendto(serverFd, buf, strlen(buf), 0, &clientAddr, addrLen);
        //printf("%s %ld\n", buf, strlen(buf));
        printf("send to %s\n", Sock_ntop(&clientAddr, addrLen));
    }
    
    exit(0);
}