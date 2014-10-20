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

void fin_cb(struct aeEventLoop *l,void *data)
{
    puts("unknow final function \n");
}


/*
 * main
 */
int main(int argc,char *argv[])
{
    aeEventLoop* loop; 

    // step 1: init buf.
    char* msg = "std input:";
    char* user_data = malloc(50*sizeof(char));
    if(!user_data)
    {
        assert( ("user_data malloc error",user_data) );
    }
    memset(user_data, 0, 50*sizeof(char));
    memcpy(user_data, msg, sizeof(msg));


    // step 2: create event loop. 
    loop = aeCreateEventLoop(MAXFD);
    aeSetBeforeSleepProc(loop, before_sleep);
    
    // step 3: std echo. 
    int ret;
    ret = aeCreateFileEvent(loop, STDIN_FILENO, AE_READABLE, 
                    file_cb, user_data);
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

    puts("Everything is ok !!!\n");
    return 0;
}
