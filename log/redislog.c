/*
 * Description: 
 *     History: junle, 2014/11/16, create
 */
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>

/* Log levels */
#define REDIS_DEBUG 0
#define REDIS_VERBOSE 1
#define REDIS_NOTICE 2
#define REDIS_WARNING 3
#define REDIS_LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define REDIS_DEFAULT_VERBOSITY REDIS_NOTICE

#define REDIS_MAX_LOGMSG_LEN    1024 /* Default maximum length of syslog messages */


typedef struct {
    char*   logfile;        // log path.
    int     verbosity;      // log level.   
    int     syslog_enabled; // syslog. 
} RedisServer;


RedisServer server;

/* Low level logging. To use only for very big messages, otherwise
 *  * redisLog() is to prefer. */
void redisLogRaw(int level, const char *msg) 
{
    const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmode = (level & REDIS_LOG_RAW);
    int log_to_stdout = server.logfile[0] == '\0';

    level &= 0xff; /* clear flags */
    if (level < server.verbosity) 
        return;

    fp = log_to_stdout ? stdout : fopen(server.logfile,"a");
    if (!fp) 
        return;

    if (rawmode) {
        fprintf(fp,"%s",msg);
    } else {
        int off;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        off = strftime(buf,sizeof(buf),"%d %b %H:%M:%S.",localtime(&tv.tv_sec));
        snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
        fprintf(fp,"[%d] %s %c %s\n",(int)getpid(),buf,c[level],msg);
    }
    fflush(fp);

    if (!log_to_stdout) 
        fclose(fp);
    
    if (server.syslog_enabled) 
        syslog(syslogLevelMap[level], "%s", msg);
}


/* Like redisLogRaw() but with printf-alike support. This is the function that
 * is used across the code. The raw version is only used in order to dump
 * the INFO output on crash. */
void redisLog(int level, const char *fmt, ...) {
    va_list ap;
    char msg[REDIS_MAX_LOGMSG_LEN];

    if ((level&0xff) < server.verbosity) return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    redisLogRaw(level,msg);
}

/*
 * main
 */
int main()
{
    server.logfile      = "./test";
    server.verbosity    = REDIS_DEBUG; //REDIS_DEFAULT_VERBOSITY;  
    server.syslog_enabled = 0;

    redisLog(REDIS_DEBUG, "this is a debug.");
    redisLog(REDIS_WARNING, "this is a warning.");
    redisLog(REDIS_NOTICE, "this is a notice.");
    redisLog(REDIS_VERBOSE, "this is a verbose");
    return 0;
}
