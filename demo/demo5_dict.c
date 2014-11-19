/*
 * Description: a simple hash implements. use redis dict. 
 *
 *     History: junle, 2014/11/17, create
 */

#include "dict.h"
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

/*
 * compare key.
 * return 1: true/false.
 */
int myCompare(void *privdata, const void *key1,
        const void *key2)
{
    if(strcmp(key1, key2) == 0)
    {
        return 1;
    } 

    return 0;
}

/*
 * murmurhash2.
 */
unsigned int myHash(const void *key) {
    unsigned int ret = dictGenHashFunction(key, strlen((char*)key));
    printf("key = %u\n", ret);
    return ret;
}

/*
 * destructor.
 */
void myDestructor(void* privdata, void* val) {
    printf("desctruct: val:%s\n", (char*)val);
}


void printState(dict* d)
{
    printf("rehashindex=%d, [0] size=%d, used=%d [1] size=%d, used=%d \n", 
            d->rehashidx, d->ht[0].size, d->ht[0].used,
            d->ht[1].size, d->ht[1].used);
}


int main()
{
    struct timeval tv;

    srand(time(NULL)^getpid());
    gettimeofday(&tv,NULL);


    // rand seed.
    unsigned int mod = tv.tv_sec^tv.tv_usec^getpid(); 
    printf("%u\n", mod);
    dictSetHashFunctionSeed(mod);


    // create <k,v> = <str, str>.
    dictType myType = {
        myHash,                 /* hash function */
        NULL,                   /* key dup */
        NULL,                   /* val dup */
        myCompare,              /* key compare */
        myDestructor,           /* key destructor */
        NULL                    /* val destructor */
    };


    // step 1: create.
    dict* myDict = dictCreate(&myType, NULL);
    assert(myDict != NULL);

    printf("-------------------\n");
    printState(myDict);

    char* key1 = "hello";
    char* key2 = "hello2";
    char* key3 = "hello3";


    // step 2: add
    printf("add 1\n");
    int ret = dictAdd(myDict, key1, "h1");
    printState(myDict);
    assert(ret==0);

    printf("add 2\n");
    ret = dictAdd(myDict, key2, "h2");
    printState(myDict);
    assert(ret==0); 

  
    printf("add 3\n");
    ret = dictAdd(myDict, key3, "h3");
    printState(myDict);
    assert(ret==0);

    printf("add 4\n");
    ret = dictAdd(myDict, "hello4", "h4");
    printState(myDict);
    assert(ret==0);

    printf("add 5\n");
    ret = dictAdd(myDict, "hello5", "h5");
    printState(myDict);
    assert(ret==0);

    printf("add 6\n");
    ret = dictAdd(myDict, "hello6", "h6");
    printState(myDict);
    assert(ret==0);




    // step 3: index.

    // step 4: search / foreach.

    // release. 
    dictRelease(myDict);

    return 0;
}
