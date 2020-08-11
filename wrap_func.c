#define WRAPFUNC
#include "wrap_func.h"




int err_flag = 0;

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

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){ // don`t right if erron == EINTR close then connect
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
    static char str[512];
    char potrSrt[8];
    struct sockaddr_in *ipv4 = NULL;
    struct sockaddr_in6 *ipv6 = NULL;
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
        case AF_INET6:
            ipv6 = (struct sockaddr_in6 *)sockaddrIn;
            if(inet_ntop(ipv6->sin6_family, &ipv6->sin6_addr, str, 128) == NULL)
                return NULL;
            if(ntohs(ipv6->sin6_port) != 0){
                snprintf(potrSrt, sizeof(potrSrt), ":%d", ntohs(ipv6->sin6_port));
                strcat(str, potrSrt);
            }
            return str;
        default:
            break;
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
    
    //printf("signal end\n");
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
    const int on = 1;
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
            Setsockopt(clientFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
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
    err_flag = 1;
    openlog(pname, LOG_PID, faclity);

    return 0;
}   

static void connect_alarm(){
    return;
}

int connect_timeo(int socketFd, struct sockaddr *addr, socklen_t alen, int nsec){
    Signal_func *sigFunc;
    int ret;

    sigFunc = Signal(SIGALRM, connect_alarm);
    
    if(alarm(nsec) != 0){
        err_msg("alarm aleady set\n");
    }

    if((ret = connect(socketFd, addr, alen)) < 0){
        close(socketFd);
        if(errno == EINTR)
            errno = ETIMEDOUT;
    }
    alarm(0);
    Signal(SIGALRM, sigFunc);

    return ret;
}

int Readable_timeo(int fd, int sec){
    fd_set rset;
    struct timeval tv;
    int ret;

    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    tv.tv_sec = sec;
    tv.tv_usec = 0;
    if((ret = select(fd + 1, &rset, NULL, NULL, &tv)) > 0)
        return 0;
    else if (ret == 0)
        return 1;
    else
        return ret;
}

int Socketpair(int domain, int type, int protocol, int sv[2]){
    while(1){
        if(socketpair(domain, type, protocol, sv) == 0)
            break;
        else{
            if(errno == EINTR){
                continue;
            }
            else{
                err("socketpair error");
            }
        }
    }
    return 0;
}

ssize_t Recvmsg(int sockfd, struct msghdr *msg, int flags){
    int ret;
    while(1){
        if((ret = recvmsg(sockfd, msg, flags)) >= 0)
            break;
        else{
            if(errno == EINTR){
                continue;
            }
            else{
                err("socketpair error");
            }
        }
    }
    return ret;
}

ssize_t Sendmsg(int sockfd, const struct msghdr *msg, int flags){
    int ret;
    while(1){
        
        if((ret = sendmsg(sockfd, msg, flags)) >= 0)
            break;
        else{
            if(errno == EINTR){
                continue;
            }
            else{
                err("socketpair error");
            }
        }
        
    }
    return ret;
}



int Connect_nonb(int socketFd, struct sockaddr *addr, socklen_t alen, int nsec){
    fd_set read, write;
    struct timeval tval;
    int n = 0;
    int flags = fcntl(socketFd, F_GETFL, 0);
    int error = 0;
    
    fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);

    FD_ZERO(&read);
    FD_ZERO(&write);
    FD_SET(socketFd, &read);
    FD_SET(socketFd, &write);

    tval.tv_sec = nsec;
    tval.tv_usec = 0;

    if((n = connect(socketFd, addr, alen)) == 0){
        fcntl(socketFd, F_SETFL, flags);
        return 0;
    }else if(n < 0){
        if( errno != EINPROGRESS)
            return -1;
    }
    
    if((n = Select(socketFd + 1, &read, &write, NULL, nsec == 0 ? NULL : &tval)) == 0){
        close(socketFd);
        errno = ETIMEDOUT;
        return -1;
    }
    if (FD_ISSET(socketFd, &read) || FD_ISSET(socketFd, &write)) {
        int len = sizeof(error);
        if (getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
            return -1; 
    } else{
        err("select error: sockfd not set");
    }
    
    if(error){
        close(socketFd);
        errno = error;
        return -1;
    }

    fcntl(socketFd, F_SETFL, flags);
    return 0;
}

int Min(int a, int b){
    return a > b ? b : a;
}


struct IfiInfo* Get_ifi_info(int family, int doaliases){
    int sockfd;
    int len = 0, lastlen;
    char *buf, lastname[IFI_NAME_LEN], *sdlname = NULL, *ptr, *haddr, *cptr;
    struct ifconf ifc;
    struct IfiInfo *ifi, *ifihead, **ifipnext;
    struct ifreq *ifr, ifrcopy;
    int flags, myflags, idx, hlen;

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    len = 100 * sizeof(struct ifreq);
    lastlen = 0;
    while(1){
        buf = malloc(len);
        if(!buf)
            err("malloc err");
        ifc.ifc_len = len;
        ifc.ifc_buf = buf;
        if(ioctl(sockfd, SIOCGIFCONF, &ifc) < 0){
            if(errno != EINVAL || lastlen != 0)
                err("ioctl error");
        }else{
            if(ifc.ifc_len == lastlen){
                break;
            }
            lastlen = ifc.ifc_len;        
        }
        len += 10 * sizeof(struct ifreq);
        free(buf);
    }
    ifihead = NULL;
    ifipnext = &ifihead;
    lastname[0] = 0;
    sdlname = NULL;
    
    for(ptr = buf; ptr < buf + ifc.ifc_len;){
        ifr = (struct ifreq*)ptr;
        ptr += sizeof(struct ifreq);
       
        if(ifr->ifr_addr.sa_family != family){
            continue;
        }
           
        myflags = 0;
        if((cptr = strchr(ifr->ifr_name,':')) != NULL){
            *cptr = 0;
        }
            
        if(strncmp(lastname, ifr->ifr_name, IF_NAMESIZE) == 0){
            if(doaliases == 0)
                continue;
            myflags = IFI_ALIAS;
        }
        memcpy(lastname, ifr->ifr_name, IF_NAMESIZE);
        ifrcopy = *ifr;
        ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);
        flags = ifrcopy.ifr_flags;
        if((flags & IFF_UP) == 0)
            continue;
        ifi = calloc(1, sizeof(struct IfiInfo));
        if(ifi == NULL)
            err("calloc error");
        *ifipnext = ifi;
        ifipnext = &ifi->next;

        ifi->flags = flags;
        ifi->myFlags = myflags;

        
        ioctl(sockfd, SIOCGIFMTU, &ifrcopy);
        ifi->mtu = ifrcopy.ifr_mtu;
        
        
        memcpy(ifi->name, ifr->ifr_name, IFI_NAME_LEN);
        
        ifi->name[IFI_NAME_LEN - 1] = '\0';
        if(sdlname == NULL || strcmp(sdlname, ifr->ifr_name) != 0){
            idx = hlen = 0;
        }
        ifi->index = idx;
        ifi->hlen = hlen;
        if(ifi->hlen > IFI_HADDR_LEN)
            ifi->hlen = IFI_HADDR_LEN;
        if(hlen){
            memcpy(ifi->haddr, haddr, ifi->hlen);
        }

        switch(ifr->ifr_addr.sa_family){
            case AF_INET:
                ifi->addr = calloc(1, sizeof(struct sockaddr_in));
                if(ifi->addr == NULL)
                    err("calloc error");
                memcpy(ifi->addr, &ifr->ifr_addr, sizeof(struct sockaddr_in));

                
                if(flags & IFF_BROADCAST){
                    ioctl(sockfd, SIOCGIFBRDADDR, &ifrcopy);
                    ifi->baddr = calloc(1, sizeof(struct sockaddr_in));
                    if(ifi->addr == NULL)
                        err("calloc error");
                    memcpy(ifi->baddr, &ifrcopy.ifr_addr, sizeof(struct sockaddr_in));
                }
                
                if(flags & IFF_POINTOPOINT){
                    ioctl(sockfd, SIOCGIFDSTADDR, &ifrcopy);
                    ifi->detaddr = calloc(1, sizeof(struct sockaddr_in));
                    if(ifi->addr == NULL)
                        err("calloc error");
                    memcpy(ifi->detaddr, &ifrcopy.ifr_addr, sizeof(struct sockaddr_in));
                }
               
                break;

            case AF_INET6:
                ifi->addr = calloc(1, sizeof(struct sockaddr_in6));
                if(ifi->addr == NULL)
                    err("calloc error");
                memcpy(ifi->addr, &ifr->ifr_addr, sizeof(struct sockaddr_in6));
                if(flags & IFF_POINTOPOINT){
                    ioctl(sockfd, SIOCGIFDSTADDR, &ifrcopy);
                    ifi->detaddr = calloc(1, sizeof(struct sockaddr_in6));
                    if(ifi->addr == NULL)
                        err("calloc error");
                    memcpy(ifi->detaddr, &ifrcopy.ifr_addr, sizeof(struct sockaddr_in6));
                }
                break;

            default:
                break;
        }

    }
    free(buf);
    return ifihead;
}

void Free_ifi_info(struct IfiInfo *ifihead){
    struct IfiInfo *ifi, *ifinext;

    for (ifi = ifihead; ifi != NULL; ifi = ifinext){
        if(ifi->addr != NULL)
            free(ifi->addr);
        if(ifi->baddr != NULL)
            free(ifi->baddr);
        if(ifi->detaddr != NULL)
            free(ifi->detaddr);
        ifinext = ifi->next;
        free(ifi);
    }
}

int Family_to_level(int family){
    switch (family) {
        case AF_INET: {
            return (IPPROTO_IP);
        }
        case AF_INET6: {
            return (IPPROTO_IPV6);
        }
        default: {
            return (-1);
        }
    }
}

int Mcast_join(int sock, const struct sockaddr *grp, socklen_t grplen, const char *ifname, unsigned int ifindex){
    

    struct group_req req;
    if(ifindex > 0){
        req.gr_interface = ifindex;
    }else if(ifname){
        if((req.gr_interface = if_nametoindex(ifname)) == 0){
            errno = ENXIO;
            return -1;
        }
    }else{
        req.gr_interface = 0;
    }
    if(grplen > sizeof(req.gr_group)){
        errno = EINVAL;
        return -1;
    }
    memcpy(&req.gr_group, grp, grplen);
    return setsockopt(sock, Family_to_level(grp->sa_family), MCAST_JOIN_GROUP, &req, sizeof(req));
}

int Sockfd_to_family(int sockfd){
    struct sockaddr_storage ss;
    socklen_t    len;

    len = sizeof(ss);
    if (getsockname(sockfd, (struct sockaddr *)&ss, &len) < 0)
        return(-1);
    return(ss.ss_family);
}

int Mcast_set_loop(int sock, int onoff){
    switch (Sockfd_to_family(sock)){
        case AF_INET:{
            unsigned char flag = onoff;
            return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &flag, sizeof(flag));
        }
        case AF_INET6:{
            unsigned int flag = onoff;
            return setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &flag, sizeof(flag));
        }
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

ssize_t Recv_des(int fd, void *ptr, size_t nbytes,SA *sa, socklen_t *alen,struct in_pktinfo *pktp){
    struct msghdr msg;
    struct iovec iov[1];
    size_t n;
    int opt = 1;

    struct cmsghdr *cmptr;
    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(struct in_pktinfo)) + CMSG_SPACE(sizeof(int))];
    }control_un;
    n = setsockopt(fd, IPPROTO_IP, IP_PKTINFO, &opt, sizeof (opt));
    if(n < 0)
        err("setsockopt error");
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    msg.msg_flags = 0;
    msg.msg_name = sa;
    msg.msg_namelen = *alen;
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    //msg.msg_control = NULL;
    //msg.msg_controllen = 0;
    
    if((n = recvmsg(fd, &msg, 0)) < 0){
        printf("!!!\n");
        return n;
    }
    for(cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL; cmptr = CMSG_NXTHDR(&msg, cmptr)){
        printf("1\n");
        if ( cmptr->cmsg_level != IPPROTO_IP || cmptr->cmsg_type != IP_PKTINFO ){
            continue;
        }
        struct in_pktinfo *pi = (struct in_pktinfo *)CMSG_DATA(cmptr);
        if(pktp){
            printf("copy\n");
            memcpy(pktp, pi, sizeof(*pi));
        }
    }
    opt = 0;
    setsockopt(fd, IPPROTO_IP, IP_PKTINFO, &opt, sizeof (opt));
    return n;
}



static pthread_key_t readLineKey;
static pthread_once_t readLineOnce = PTHREAD_ONCE_INIT;

static void Readline_destructor(void *ptr){
    free(ptr);
}

int Pthread_key_create(pthread_key_t *keyptr, void (*destructor)(void*)){
    if(pthread_key_create(keyptr, destructor) != 0)
        err("Pthread_key_create");
}

int Pthread_once(pthread_once_t *onceptr, void (*init)(void)){
    if(pthread_once(onceptr, init) != 0)
        err("Pthread_key_create");
}

static void Readline_once(){
    Pthread_key_create(&readLineKey, Readline_destructor);
}

static size_t My_read_r(Rline *rline,int fd, char *vptr){
    while(readCnt <= 0){
        if((readCnt = read(fd, rline->buf, MAXLINE)) < 0){
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
        rline->bufptr = readBuf;
    }
    --readCnt;
    *vptr = *(readPtr++);
    return 1;
}

size_t Readline_r(int fd,void *vptr, size_t maxLen){
    size_t ret = 0;
    size_t readn = 0;
    char *ptr = vptr;
    char c;
    Rline *rline;
    Pthread_once(&readLineOnce, Readline_once);
    if((rline = pthread_getspecific(readLineKey)) == NULL){
        rline = calloc(1, sizeof(Rline));
        pthread_setspecific(readLineKey, rline);
    }

    while(ret < maxLen){
        if((readn = My_read_r(rline, fd, &c)) == 1){
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

