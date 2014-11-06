#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "ae.h"
#define MAXFD 1024


#define SVR_IP      "localhost"
#define SVR_PORT    29382
#define BUF_SIZE    4096


void before_sleep(struct aeEventLoop *l) 
{
    puts("before_sleep !!! \n");
}

/*
 *
 */
void read_cb(struct aeEventLoop *l, int fd, void *data, int mask)
{
    char buf[4096];
    int len;
    unsigned int size = sizeof(struct sockaddr);
    struct sockaddr_in client_addr;

    memset(buf, 0, sizeof(buf));
    len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &size);
    if (len == -1) 
    {
        perror("recvfrom()\n");
    } 
    else if (len == 0) 
    {
        printf("Connection Closed\n");
    } 
    else 
    {
        printf("Read: len [%d] - content [%s]\n", len, buf);

        /* Echo */
        //sendto(fd, buf, len, 0, (struct sockaddr *)&client_addr, size);
    }

}

/*
 * timer callback
 */
int time_cb(struct aeEventLoop *l, long long id, void *data)
{
    printf("now = %ld\n", time(NULL));
    printf("time_cb: [id : %lld],[data: %p] \n", id, data);
    return 5*1000;
}

void fin_cb(struct aeEventLoop *l,void *data)
{
    puts("unknow final function \n");
}

/*
 * main func.
 */
int main()
{
    int                 sock_fd;
    int                 flag = 1;

    // step 1: create socket.
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("socket()");
        return -1;
    }

    // step 2: set socket nonblock
   int flags = fcntl(sock_fd, F_GETFL, 0);
   if (flags < 0) 
       return -1;

    flags |= O_NONBLOCK;
    if(fcntl(sock_fd, F_SETFL, flags) != 0)
        return -2;

    // step 3: set socket reuse
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) 
    {
        perror("setsockopt()");
        return 1;
    }

    // step 4: set ip/port addr
    struct sockaddr_in  sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr =  INADDR_ANY;
    sin.sin_port = htons(SVR_PORT);

    // step 5: bind socket.
    unsigned int iLen = sizeof(struct sockaddr);
    if( bind(sock_fd, (struct sockaddr*)&sin, iLen) < 0)
    {
        printf("bind error: \n");
        return -1;
    }
    printf("bind ok: sock=%d. sin_port=%d \n", sock_fd, sin.sin_port);

    // step 6: create event loop. 
    aeEventLoop* loop; 
    loop = aeCreateEventLoop(MAXFD);
    aeSetBeforeSleepProc(loop, before_sleep);
    
    // step 7: monitor socket readable event.
    char user_data[1024];
    int ret = aeCreateFileEvent(loop, sock_fd, AE_READABLE, 
                    read_cb, user_data);
    if(ret != 0)
    { 
        printf("err ret=%d\n", ret);
        return 0;
    }

    // step 8: create timer (5s). 
    ret = aeCreateTimeEvent(loop, 5*1000, time_cb, NULL, fin_cb);
    if(ret != 0)
    {
        printf("err ret=%d\n", ret);
        return 0;
    }

    // step 9: main loop
    aeMain(loop);

    return 0;
}
