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




#define MAXLINE 4096
#define MAXFD 64

#define err(s) do{  \
    printf(s); \
    printf(" %s %s %d\n", strerror(errno), __FILE__, __LINE__); \
    exit(1); \
}while(0)

#define err_msg(s) do{  \
    printf(s); \
    printf(" %s %s %d\n", hstrerror(errno), __FILE__, __LINE__); \
    exit(1); \
}while(0)

typedef void (Signal_func)(int);

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
#endif