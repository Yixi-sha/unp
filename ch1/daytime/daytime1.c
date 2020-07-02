#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define err(s) do{  \
    printf(s); \
    printf(" %s %s %d\n",strerror(errno),__FILE__, __LINE__); \
    exit(1); \
}while(0)

int main(int argc, char *argv[]){
    int socketFd = 0;
    struct sockaddr_in serverAddr;
    char buf[4096];
    size_t n = 0;

    if(argc != 2)
        err("usage: a.out <IPaddress>");
    
    if((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err("sock");
    
    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port =  htons(13);
    if(inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) < 0)
        err("inet_pton");
    
    if(connect(socketFd, (struct sockaddr *)(&serverAddr), sizeof(serverAddr)) < 0)
        err("connect");
    
    while((n = read(socketFd, buf,  4096)) > 0){
        buf[n] = '\0';    
        if(fputs(buf, stdout) == EOF)
            err("fput error");
    }
    if(n < 0)
        err("read error");

    exit(0);
}