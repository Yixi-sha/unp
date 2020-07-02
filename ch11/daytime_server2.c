#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "../wrap_func.h"



int main(int argc, char* argv[]){

    int listenfd, connfd;
    struct sockaddr_in serveraddr;
    char buff[4096];
    time_t ticks;

    if(argc == 2){
        listenfd = Tcp_listen(NULL, argv[1], NULL);
    }else if(argc == 3){
        listenfd = Tcp_listen(argv[1], argv[2], NULL);
    }else
        err("usage: a.out  <service>");
    
    
    while(1){
        connfd = Accept(listenfd, NULL, NULL);
        ticks = time(NULL);

        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }


    exit(0);
}