#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "wrap_func.h"
#include <signal.h>



int Socket(int family, int type, int protocol){
    int n;
    if((n = socket(family, type, protocol)) < 0)
        err("scoket");
    return n;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    int n;

    if((n = bind(sockfd, addr,addrlen)) < 0 )
        err("bind");
    return n;

}

int Listen(int sockfd, int backlog){
    int n;
    char* backlogOfEnv = NULL;

    if((backlogOfEnv = getenv("LISTENQ")) != NULL)
        backlog = atoi(backlogOfEnv);

    if((n = listen(sockfd, backlog)) < 0)
        err("listen");

    return n;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    int n;
    while(1){
        if((n = accept(sockfd, addr, addrlen)) < 0){
            if(errno == EINTR){
                continue;
            }else{
                err("accept");
            }
        }
        break;
    }    
    return n;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    int n;
    if((n = connect(sockfd, addr, addrlen)) < 0)
        err("connect");
    
    return n;
}

void Pthread_mutex_lock(pthread_mutex_t *mptr){
    int n;
    if((n = pthread_mutex_lock(mptr)) == 0)
        return;
    errno = n;
    err("Pthread_mutex_lock");
}

int Inet_pton(int af, const char *src, void *dst){
    int n;
    if((n = inet_pton(af, src, dst)) < 0)
        err("Inet_pton");
    
    return n;
}

char *Sock_ntop(const struct sockaddr *sockaddrIn, socklen_t len){
    static char str[128];
    char potrSrt[8];
    struct sockaddr_in *ipv4 = NULL;

    switch(sockaddrIn->sa_family){
        case AF_INET:
            ipv4 = (struct sockaddr_in *)sockaddrIn;
            if(inet_ntop(ipv4->sin_family, &ipv4->sin_addr, str, 128) == NULL)
                return NULL;
            if(ntohs(ipv4->sin_port) != 0){
                snprintf(potrSrt, sizeof(potrSrt), ":%d", ntohs(ipv4->sin_port));
                strcat(str, potrSrt);
                
            }
            return str;
    }

    return str;
}

size_t Readn(int fd, void *vptr, size_t len){
    int ret = 0;

    size_t nread = 0;
    size_t rest = len;
    char *ptr = vptr;
    while(rest > 0){
        if((nread = read(fd, ptr, rest)) < 0){
            if(errno == EINTR){
                nread = 0;
            }else {
                return -1;
            }
        }else if(nread == 0){
            break;
        }

        ret += nread;
        ptr += nread;
        rest -= nread;
    }

    return ret;
}

ssize_t Read(int fd, void *buf, size_t count){
    ssize_t ret = 0;
    while(1){
        if((ret = read(fd, buf, count)) < 0){
            if(errno == EINTR){
                continue;
            }
            else{
                err("Read error");
            }
        }else{
            break;
        }
    }
    return ret;
}

ssize_t Write(int fd, void *buf, size_t count){
    ssize_t ret = 0;
    while(1){
        if( (ret = write(fd, buf, count)) < 0){
            if(errno == EINTR)
                continue;
            else{
                err("Read error");
            }
        }
        break;
    }
    return ret;
}

size_t Writen(int fd,const void *vptr, size_t len){
    int ret = 0;

    size_t nwrite = 0;
    size_t rest = len;
    const char *ptr = vptr;
    while(rest > 0){
        if((nwrite = write(fd, ptr, rest)) < 0){
            if(errno == EINTR){
                nwrite = 0;
            }else {
                return -1;
            }
        }else if(nwrite == 0){
            break;
        }

        ret += nwrite;
        ptr += nwrite;
        rest -= nwrite;
    }

    return ret;
}

static int readCnt = 0;
static char *readPtr = NULL;
static char readBuf[MAXLINE];

static size_t my_read(int fd, char *vptr){
    while(readCnt <= 0){
        if((readCnt = read(fd, readBuf, sizeof(readBuf))) < 0){
            if(errno == EINTR){
                continue;
            }
            else{
                printf("readCnt is %d \n", -1);
                return -1;
            }
        }else if(0 == readCnt){
            return 0;
        } 
        readPtr = readBuf;
        
    }
   
    --readCnt;
    *vptr = *(readPtr++);
    return 1;
}

size_t Read_line_buf(void **vptr){
    if(readCnt){
        *vptr = readPtr;
        //readCnt = 0; //
    }
    return readCnt;
}

size_t Readline(int fd,void *vptr, size_t maxLen){
    size_t ret = 0;
    size_t readn = 0;
    char *ptr = vptr;
    char c;
    while(ret < maxLen){
        //if((readn = read(fd, &c, 1)) == 1){
        if((readn = my_read(fd, &c)) == 1){
            *(ptr++) = c;
            ++ret;
            if(c == '\n')
                break;
        }else if(0 == readn){
            *ptr = 0;
            return ret;
        }else if(errno != EINTR){
            return -1;
        }
    }
    *ptr = 0;
    return ret;
}

int Socket_to_family(int sockfd){
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
     
    if(getsockname(sockfd, (struct sockaddr*)(&addr), &len) < 0)
        return -1;
    return addr.ss_family;
}

Signal_func* Signal(int signalNum, Signal_func* signal_func){
    struct sigaction newAtion, oldAction;
    
    newAtion.sa_handler = signal_func;
    sigemptyset(&newAtion.sa_mask);
    newAtion.sa_flags = 0;
    if (signalNum == SIGALRM){
        #ifdef SA_INTERRUPT
            newAtion.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
        #endif
    }else{
        #ifdef SA_RESTART
            newAtion.sa_flags |= SA_RESTART;
        #endif
    }


    if(sigaction(signalNum, &newAtion, &oldAction) < 0)
        err("sigaction");
    
    printf("signal end\n");
    return oldAction.sa_handler;

}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){
    int n;
    while(1){
        if((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0){
            if(errno == EINTR){
                continue;
            }else{
                err("select");
            }
        }
        break;
    }
    
    return n;

}


ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    ssize_t ret;

    while(1){
        if((ret = recvfrom(sockfd, buf, len, flags, src_addr, addrlen)) < 0){
            if(errno == EINTR){
                continue;
            }else{
                err("recvfrom");
            }
        }
        break; 
    }

    return ret;
}

ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen){
    ssize_t ret;

    while(1){
        if((ret = sendto(sockfd, buf, len, flags, dest_addr, addrlen)) < 0){
            if(errno == EINTR){
                continue;
            }else{
                err("sendto");
            }
        }
        break; 
    }

    return ret;
}

struct addrinfo *Host_serv(const char *hostname, const char *service, \
int family, int socktype,int flags){
    struct addrinfo *ret;
    struct addrinfo hints;

    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = flags;
    hints.ai_family = family;
    hints.ai_socktype = socktype;
    //printf("%s %s\n", hostname, service);
    if(getaddrinfo(hostname, service, &hints, &ret) != 0){
        //printf("return NULL\n");
        return NULL;
    }
        
    return ret;
}

int Setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen){
    while(1){
        if(setsockopt(sockfd, level, optname,optval, optlen) == 0)
            break;
        else{
            if(errno == EINTR){
                continue;
            }
            else{
                err("setsockopt error");
            }
        }
    }
    return 0;
}

int Getsockopt(int sockfd, int level, int optname,void *optval, socklen_t *optlen){
    while(1){
        if(getsockopt(sockfd, level, optname, optval, optlen) == 0)
            break;
        else{
            if(errno == EINTR){
                continue;
            }
            else{
                err("getsockopt error");
            }
        }
    }
    return 0;
}

int Tcp_connect(const char *hostname, const char *service, int family){
    int socketFd = -1;
    struct addrinfo *addrinfoRet = NULL, *addrinfoHead = NULL;

    if(family == AF_INET6){
        addrinfoRet = Host_serv(hostname, service, family, SOCK_STREAM, AI_CANONNAME | AI_V4MAPPED);
        
    }else
        addrinfoRet = Host_serv(hostname, service, family, SOCK_STREAM, AI_CANONNAME);
    if(addrinfoRet == NULL){
        err("host_serv error\n");
    }
    addrinfoHead = addrinfoRet;
    
    while(addrinfoRet){
        socketFd = socket(addrinfoRet->ai_family, addrinfoRet->ai_socktype, addrinfoRet->ai_protocol);
        
        if(socketFd < 0)
            continue;
        if(Connect(socketFd, addrinfoRet->ai_addr, addrinfoRet->ai_addrlen) == 0)
            break;
        
        close(socketFd);
        socketFd = -1;
        addrinfoRet = addrinfoRet->ai_next;
    }
    freeaddrinfo(addrinfoHead);
    return socketFd;
}

int Tcp_listen(const char *hostname, const char *service, socklen_t *addrlenp, int family){
    int serverFd = -1;
    const int on = 1;
    const int no = 0;
    struct addrinfo *addrinfoRet = NULL, *addrinfoHead = NULL;

    addrinfoRet = Host_serv(hostname, service, family, SOCK_STREAM, AI_PASSIVE);
    if(addrinfoRet == NULL){
        err("host_serv error\n");
    }
    addrinfoHead = addrinfoRet;
    while(addrinfoRet){
       
        serverFd = socket(addrinfoRet->ai_family, addrinfoRet->ai_socktype, addrinfoRet->ai_protocol);
        Setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if(family == AF_INET6){
            if(setsockopt(serverFd, IPPROTO_IPV6, IPV6_V6ONLY, &no, sizeof(no)) != 0){
                printf("set error\n");
            }
        }
        if(serverFd < 0)
            continue;
        
        if(bind(serverFd, addrinfoRet->ai_addr, addrinfoRet->ai_addrlen) == 0){
            if(listen(serverFd, 20) == 0){
                break;
            }
                
        }
        
        close(serverFd);
        serverFd = -1;
        addrinfoRet = addrinfoRet->ai_next;
    }
    if(addrinfoRet && addrlenp)
        *addrlenp = addrinfoRet->ai_addrlen;
    freeaddrinfo(addrinfoHead);
    
    return serverFd;
}

int Udp_client(const char *hostname, const char *service, struct sockaddr **saptr, socklen_t *lenp, int family){
    int clientFd = -1;

    struct addrinfo *addrinfoRet = NULL, *addrinfoHead = NULL;

    addrinfoRet = Host_serv(hostname, service, family, SOCK_DGRAM, AI_CANONNAME);
    if(addrinfoRet == NULL){
        err("host_serv error\n");
    }
    addrinfoHead = addrinfoRet;

    while(addrinfoRet){
        clientFd = socket(addrinfoRet->ai_family, addrinfoRet->ai_socktype, addrinfoRet->ai_protocol);
        if(clientFd > 0){
            *saptr = malloc(addrinfoRet->ai_addrlen);
            if(*saptr == NULL){
                err("Udp_client malloc error");
            }
            memcpy(*saptr, addrinfoRet->ai_addr, addrinfoRet->ai_addrlen);
            *lenp = addrinfoRet->ai_addrlen;
            break;
        }
            
        
        clientFd = -1;
        addrinfoRet = addrinfoRet->ai_next;
    }
    freeaddrinfo(addrinfoHead);
    return clientFd;
}

int Udp_client_connect(const char *hostname, const char *service, int family){
    int clientFd = -1;

    struct addrinfo *addrinfoRet = NULL, *addrinfoHead = NULL;

    addrinfoRet = Host_serv(hostname, service, family, SOCK_DGRAM, AI_CANONNAME);
    if(addrinfoRet == NULL){
        err("host_serv error\n");
    }
    addrinfoHead = addrinfoRet;

    while(addrinfoRet){
        clientFd = socket(addrinfoRet->ai_family, addrinfoRet->ai_socktype, addrinfoRet->ai_protocol);
        if(clientFd < 0){
            continue;
        }
        if(connect(clientFd, addrinfoRet->ai_addr, addrinfoRet->ai_addrlen) == 0){
            printf("connect %s\n", Sock_ntop(addrinfoRet->ai_addr, addrinfoRet->ai_addrlen));
            break;
        } 
        close(clientFd);
        clientFd = -1;
        addrinfoRet = addrinfoRet->ai_next;
    }
    freeaddrinfo(addrinfoHead);
    return clientFd;
}

int Udp_server(const char *hostname, const char *service, socklen_t *plen, int family){
    int clientFd = -1;

    struct addrinfo *addrinfoRet = NULL, *addrinfoHead = NULL;

    addrinfoRet = Host_serv(hostname, service, family, SOCK_DGRAM, AI_PASSIVE);
    if(addrinfoRet == NULL){
        err("host_serv error\n");
    }
    addrinfoHead = addrinfoRet;

    while(addrinfoRet){
        clientFd = socket(addrinfoRet->ai_family, addrinfoRet->ai_socktype, addrinfoRet->ai_protocol);
        if(clientFd < 0){
            continue;
        }
        if(bind(clientFd, addrinfoRet->ai_addr, addrinfoRet->ai_addrlen) == 0){
            printf("bind %s\n", Sock_ntop(addrinfoRet->ai_addr, addrinfoRet->ai_addrlen));
            break;
        } 
        
        clientFd = -1;
        addrinfoRet = addrinfoRet->ai_next;
    }
    if(addrinfoRet && plen)
        *plen = addrinfoRet->ai_addrlen;
    freeaddrinfo(addrinfoHead);
    return clientFd;
}

int daemon_init(const char *pname, int faclity){
    pid_t pid;
    int i = 0;

    if((pid = fork()) < 0 ){
        return -1;
    }else if(pid > 0){
        exit(0);
    }

    if(setsid() < 0)
        return -1;

    Signal(SIGHUP, SIG_IGN);
    if((pid = fork()) < 0)
        return -1;
    else if(pid > 0)
        exit(0);
    
    chdir("/");

    for(i = 0; i < MAXFD; i++){
        close(i);
    }

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    openlog(pname, LOG_PID, faclity);

    return 0;
}   
