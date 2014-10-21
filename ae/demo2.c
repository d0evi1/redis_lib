/*
 * Description: 
 *     History: junle, 2014/10/20, create
 */


#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>


#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>


#include "ae.h"
#include "anet.h"

#define MAXFD 5


void before_sleep(struct aeEventLoop *l) 
{
    puts("before_sleep !!! \n");
}

/*
 *
 */
void file_cb(struct aeEventLoop *l,int fd,void *data,int mask)
{
    char buf[51] ={0};
    read(fd,buf,51);
    printf("I'm file_cb ,here [EventLoop: %p],[fd : %d],[data: %p],[mask: %d] \n",l,fd,data,mask);
    printf("get %s",buf);
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

/*
 */
void fin_cb(struct aeEventLoop *l,void *data)
{
    puts("unknow final function \n");
}




/*
 * 
 */
static void handleAccept(int fd)
{
    printf("handleAccept: fd=%d\n", fd);
}



/*
 * handle accept.
 */
void acceptTcpHandler(aeEventLoop *l, int fd, void* data, int mask)
{
    char errStr[256];
    char clientIp[256];
    int clientPort = -1;
    int clientFd = anetTcpAccept(errStr, fd, clientIp, sizeof(clientIp), &clientPort);
    if(clientFd == ANET_ERR) 
    {
        if(errno != EWOULDBLOCK)
        {
            printf("accepting client connection: %s\n", errStr);
        }
        
        return;
    }

    handleAccept(clientFd);
}

/*
 *
 */
int initServer(aeEventLoop* loop, int port, int backlog)
{
    char errStr[256];

    // step 1: create socket and listen.
    int fd = anetTcpServer(errStr, port, "localhost", backlog);
    if(fd == ANET_ERR)
    {
        printf("error: %s\n", errStr);
        return -2;
    }

    printf("create socket: %d\n", fd);

    // step 2: non block.
    anetNonBlock(NULL, fd);

    // step 3: add event.
    int ret = aeCreateFileEvent(loop, fd, AE_READABLE, acceptTcpHandler, NULL); 
    if(ret == AE_ERR)
    {
        printf("listen readable event.\n");
        return -2;
    }


    return 0;
}


/*
 * main
 */
int main(int argc,char *argv[])
{
    aeEventLoop* loop; 

    // step 1: create event loop. 
    loop = aeCreateEventLoop(MAXFD);
    aeSetBeforeSleepProc(loop, before_sleep);
    

    // step 2: create timer (5s). 
    int ret = aeCreateTimeEvent(loop, 5*1000, time_cb, NULL, fin_cb);
    if(ret != 0)
    {
        printf("err ret=%d\n", ret);
        return 0;
    }

    // step 3: initServer.
    ret = initServer(loop, 23932, 5);
    if(ret != 0)
    {
        printf("init Server fail: ret=%d \n", ret);
        return 0;
    }

    // step 4: main loop
    aeMain(loop);

    puts("Everything is ok !!!\n");
    return 0;
}
