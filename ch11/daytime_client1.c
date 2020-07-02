#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include "../wrap_func.h"

int main(int argc, char *argv[]){
    int socketFd = 0;
    
    char buf[4096];
    size_t n = 0;

    


    if(argc != 3)
        err("usage: a.out <hostname> <service>");
    
    
    socketFd = Tcp_connect(argv[1], argv[2]);
    if(socketFd == 1)
        err("tcp_connect error\n");
    

    while((n = Read(socketFd, buf,  4096)) > 0){
        buf[n] = '\0';    
        if(fputs(buf, stdout) == EOF)
            err("fput error");
    }
    if(n < 0)
        err("read error");

    exit(0);
}