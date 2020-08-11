#ifndef WRAP_FUNC_H__
#define WRAP_FUNC_H__
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <linux/pfkeyv2.h>
#include <sys/param.h>
#include <linux/in.h>
#include <pthread.h>



#define MAXLINE 4096
#define MAXFD 64

#ifndef WRAPFUNC
extern int err_flag;
#endif

#define err(s) do{  \
    if(err_flag == 1){ \
        syslog(LOG_ERR,s); \
        syslog(LOG_ERR," %s %s %d\n", hstrerror(errno), __FILE__, __LINE__); \
    }else{ \
        printf(s); \
        printf(" %s %s %d\n", hstrerror(errno), __FILE__, __LINE__); \
    } \
    exit(1); \
}while(0)

#define err_msg(s) do{  \
    if(err_flag == 1){ \
        syslog(LOG_ERR,s); \
        syslog(LOG_ERR," %s %s %d\n", hstrerror(errno), __FILE__, __LINE__); \
    }else{ \
        printf(s); \
        printf(" %s %s %d\n", hstrerror(errno), __FILE__, __LINE__); \
    } \
}while(0)

struct UnpInPktinfo{
    struct in_addr addr;
    int ifindex;
};

typedef struct sockaddr SA;

typedef void (Signal_func)(int);

#define IFI_NAME_LEN 16
#define IFI_HADDR_LEN 8


#define IFI_NAME 16
#define IFI_HADDR 8
#define IFI_ALIAS 1

struct IfiInfo{
    char name[IFI_NAME_LEN];
    unsigned int index;
    unsigned int mtu;
    unsigned char haddr[IFI_HADDR_LEN];
    unsigned short hlen;

    short flags;

    short myFlags;
    struct sockaddr *addr;
    struct sockaddr *baddr;
    struct sockaddr *detaddr;
    struct IfiInfo *next;
};


int Min(int a, int b);
void Pthread_mutex_lock(pthread_mutex_t *mptr);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Listen(int sockfd, int backlog);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Socket(int family, int type, int protocol);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Inet_pton(int af, const char *src, void *dst);
char *Sock_ntop(const struct sockaddr *sockaddrIn, socklen_t len);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, void *buf, size_t count);
size_t Readn(int fd, void *ptr, size_t len);
size_t Writen(int fd,const void *vptr, size_t len);
static size_t my_read(int fd, char *vptr);
size_t Readline(int fd,void *vptr, size_t maxLen);
int Socket_to_family(int sockfd);
Signal_func* Signal(int signalNum, Signal_func* signal_func);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
struct addrinfo *Host_serv(const char *hostname, const char *service, \
int family, int socktype, int flags);
int Tcp_connect(const char *hostname, const char *service, int family);
int Getsockopt(int sockfd, int level, int optname,void *optval, socklen_t *optlen);
int Setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
int Tcp_listen(const char *hostname, const char *service, socklen_t *addrlenp, int family);
int Udp_client(const char *hostname, const char *service, struct sockaddr **saptr, socklen_t *lenp, int family);
int Udp_client_connect(const char *hostname, const char *service, int family);
int Udp_server(const char *hostname, const char *service, socklen_t *plen, int family);
int daemon_init(const char *pname, int faclity);
int Readable_timeo(int fd, int sec);
int Socketpair(int domain, int type, int protocol, int sv[2]);
ssize_t Recvmsg(int sockfd, struct msghdr *msg, int flags);
ssize_t Sendmsg(int sockfd, const struct msghdr *msg, int flags);
int Connect_nonb(int socketFd, struct sockaddr *addr, socklen_t alen, int nsec);
struct IfiInfo* Get_ifi_info(int family, int doaliases);
void Free_ifi_info(struct IfiInfo *ifihead);
int Family_to_level(int family);
int Mcast_join(int sock, const struct sockaddr *grp, socklen_t grplen, const char *ifname, unsigned int ifindex);
int Sockfd_to_family(int sockfd);
int Mcast_set_loop(int sock, int onoff);
ssize_t Recv_des(int fd, void *ptr, size_t nbytes,SA *sa, socklen_t *alen,struct in_pktinfo *pktp);


typedef struct Rline_s{
    int cnt;
    char *bufptr;
    char buf[MAXLINE];
}Rline;

int Pthread_key_create(pthread_key_t *keyptr, void (*destructor)(void*));
int Pthread_once(pthread_once_t *onceptr, void (*init)(void));
static size_t My_read_r(Rline *rline,int fd, char *vptr);
size_t Readline_r(int fd,void *vptr, size_t maxLen);
#endif