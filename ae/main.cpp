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

#include <stdio.h>
#include <string.h>
#include <memory.h>

extern "C"{
#include "anet.h"
#include "ae.h"
}




typedef struct {
    unsigned int    maxclients; 
    aeEventLoop     *el;

    unsigned int    port;
    int ipfd_count;             /* Used slots in ipfd[] */
    int ipfd[1000]; 
    int sofd;                   /* Unix socket file descriptor */

    char *unixsocket;           /* UNIX socket path */
    char neterr[256];   /* Error buffer for anet.c */
    mode_t unixsocketperm;      /* UNIX socket permission */
    int tcp_backlog;            /* TCP listen() backlog */

    char *bindaddr[1000]; /* Addresses we should bind to */
    int bindaddr_count;         
} Server;


Server server;


void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void acceptUnixHandler(aeEventLoop *el, int fd, void *privdata, int mask);

void beforeSleep(struct aeEventLoop *eventLoop) {
}


/*
 * 定时器触发函数
 */
int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData) 
{
    printf("serverCron\n");
    return 0;
}

int listenToPort(int port, int *fds, int *count) {
    int j;

    /* Force binding of 0.0.0.0 if no bind address is specified, always
     *      * entering the loop if j == 0. */
    if (server.bindaddr_count == 0) server.bindaddr[0] = NULL;
    for (j = 0; j < server.bindaddr_count || j == 0; j++) {
        if (server.bindaddr[j] == NULL) {
               fds[*count] = anetTcpServer(server.neterr,port,NULL,
                    server.tcp_backlog);
            if (fds[*count] != ANET_ERR) {
                anetNonBlock(NULL,fds[*count]);
                (*count)++;
            }
            
            if (*count) break;
        } else if (strchr(server.bindaddr[j],':')) {
            
        } else {
            /* Bind IPv4 address. */
            fds[*count] = anetTcpServer(server.neterr,port,server.bindaddr[j],
                    server.tcp_backlog);
        }
        if (fds[*count] == ANET_ERR) {
            return -1;
        }
        anetNonBlock(NULL,fds[*count]);
        (*count)++;
    }

    return 0; 
}


static void sigShutdownHandler(int sig) {
    char *msg;

    switch (sig) {
    case SIGINT:
        msg = "Received SIGINT scheduling shutdown...";
        break;
    case SIGTERM:
        msg = "Received SIGTERM scheduling shutdown...";
        break;
    default:
        msg = "Received shutdown signal, scheduling shutdown...";
    };
}

void setupSignalHandlers(void) {
    struct sigaction act;

    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
     *      * Otherwise, sa_handler is used. */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sigShutdownHandler;
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);

#ifdef HAVE_BACKTRACE
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = sigsegvHandler;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGILL, &act, NULL);
#endif
    return;
}

/*
 * 
 */
int main()
{
    // 1. ignore this.  
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    setupSignalHandlers();

    // 2.


    server.port = 32832;
    server.maxclients = 100;


    server.el = aeCreateEventLoop(server.maxclients+1);

    // 3.
    listenToPort(server.port,server.ipfd,&server.ipfd_count);

    server.sofd = anetUnixServer(server.neterr,server.unixsocket,
            server.unixsocketperm, server.tcp_backlog);


    anetNonBlock(NULL,server.sofd);

    if(aeCreateTimeEvent(server.el, 1, serverCron, NULL, NULL) == AE_ERR) {
        exit(1);
    }

    // 4.
    /* Create an event handler for accepting new connections in TCP and Unix
     *      * domain sockets. */
    for (int j = 0; j < server.ipfd_count; j++)
    {
        if (aeCreateFileEvent(server.el, server.ipfd[j], AE_READABLE,
                    acceptTcpHandler,NULL) == AE_ERR)
        {
            printf("Unrecoverable error creating server.ipfd file event.\n");
        }
    }

    if (server.sofd > 0 
            && aeCreateFileEvent(server.el,server.sofd,AE_READABLE,
                acceptUnixHandler,NULL) == AE_ERR) 
    {
        printf("Unrecoverable error creating server.sofd file event.\n");
    }


    // 5.  
    aeSetBeforeSleepProc(server.el,beforeSleep);
    aeMain(server.el);
    aeDeleteEventLoop(server.el);
    return 0;
}
