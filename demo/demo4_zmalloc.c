/*
 * Description: 
 *     History: junle, 2014/11/17, create
 */

#include "zmalloc.h"
#include <stdio.h>
#include <stdlib.h>


void redisPanic(char* msg, char* file, int line) 
{
    printf("bug: %s #%s:%d\n", msg, file, line);
    *((char*)-1) = 'x';
}
    


void redisOutOfMemoryHandler(size_t allocation_size)
{
    printf("Out Of Memory allocating %zu bytes!\n"), allocation_size;
    redisPanic("redis out of memory", __FILE__, __LINE__);
}


int main()
{
    // step 1: init (optional).
    zmalloc_enable_thread_safeness();
    zmalloc_set_oom_handler(redisOutOfMemoryHandler);

    // step 2: malloc.
    char* pMem = zmalloc(sizeof(char)*100);
    strcpy(pMem, "hello world.");
    printf("str=%s\n", pMem);
    printf("size=%zu\n", zmalloc_size(pMem));
    zlibc_free(pMem);

    return 0;
}
