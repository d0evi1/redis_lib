#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "ae.h"
#define MAXFD 5


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
void read_cb(struct aeEventLoop *l,int fd,void *data,int mask)
{
    char buf[4096];
    int  len;
    unsigned int size = sizeof(struct sockaddr);
    struct sockaddr_in  client_addr;

    printf("read_cb ok!!!\n");

    memset(buf, 0, sizeof(buf));
    len = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &size);
    if (len == -1) {
        perror("recvfrom()");
    } else if (len == 0) {
        printf("Connection Closed\n");
    } else {
        printf("Read: len [%d] - content [%s]\n", len, buf);

        /* Echo */
        //sendto(fd, buf, len, 0, (struct sockaddr *)&client_addr, size);
    }

}

/*
 * timer callback, if return AE_NOMORE, end.
 */
int time_cb(struct aeEventLoop *l, long long id, void *data)
{
    printf("now = %ld\n", time(NULL));
    printf("time_cb: [id : %lld],[data: %p] \n", id, data);
    // return 5*1000;
    return AE_NOMORE;
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
    struct sockaddr_in  sin;

    /* Create endpoint */
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("socket()");
        return -1;
    }

    //
    //
   int flags = fcntl(sock_fd, F_GETFL, 0);
   if (flags < 0) 
       return -1;

    flags |= O_NONBLOCK;
    if(fcntl(sock_fd, F_SETFL, flags) != 0)
        return -2;

    // 
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) 
    {
        perror("setsockopt()");
        return 1;
    }

    printf("create socket ok\n");

    /* Set IP, port */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(SVR_IP);
    sin.sin_port = htons(SVR_PORT);

    unsigned int iLen = sizeof(struct sockaddr);
    bind(sock_fd, (struct sockaddr*)&sin, iLen);
    printf("bind ok: sock=%d.\n", sock_fd);

    aeEventLoop* loop; 

    // step 1: init buf.
    char* msg = "std input:";
    char* user_data =(char*) malloc(50*sizeof(char));
    if(!user_data)
    {
        printf("user_data malloc error:%s\n", user_data) ;
        return -2;
    }
    memset(user_data, 0, 50*sizeof(char));
    memcpy(user_data, msg, strlen(msg));


    // step 2: create event loop. 
    loop = aeCreateEventLoop(MAXFD);
    aeSetBeforeSleepProc(loop, before_sleep);
    
    // step 3: std echo. 
    int ret;
    ret = aeCreateFileEvent(loop, sock_fd, AE_READABLE, 
                    read_cb, user_data);
    if(ret != 0)
    { 
        printf("err ret=%d\n", ret);
        return 0;
    }

    // step 4: create timer (5s). 
    ret = aeCreateTimeEvent(loop, 5*1000, time_cb, NULL, fin_cb);
    if(ret != 0)
    {
        printf("err ret=%d\n", ret);
        return 0;
    }

    // step 5: main loop
    aeMain(loop);

    return 0;
}
