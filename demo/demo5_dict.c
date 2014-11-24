/*
 * Description: a simple hash implements. use redis dict. 
 *
 *     History: junle, 2014/11/17, create
 */

#include "dict.h"
#include <stdlib.h>
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
    //printf("hash key = %u\n", ret);
    return ret;
}

/*
 * destructor.
 */
void myDestructor(void* privdata, void* val) {
    printf("desctruct: val:%s\n", (char*)val);
}

/*
 * scan func.
 */
void printState(dict* d)
{
    printf("rehashindex=%d, iterators=%d, [0] size=%d, used=%d [1] size=%d, used=%d \n", 
            d->rehashidx, d->iterators,
            d->ht[0].size, d->ht[0].used,
            d->ht[1].size, d->ht[1].used);
}

/*
 *
 */
void dictScanCallback(void* privdata, const dictEntry* de)
{
    printf("[scan] key=%s, val = %s\n", de->key, de->v.val);
}

/*
 * main function.
 */
int main()
{
    struct timeval tv;

    srand(time(NULL)^getpid());
    gettimeofday(&tv,NULL);


    // rand seed or fixed seed.
    unsigned int mod = tv.tv_sec^tv.tv_usec^getpid(); 
    printf("%u\n", mod);
    //dictSetHashFunctionSeed(mod);
    dictSetHashFunctionSeed(12391);



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

    char* key[10] = {"hello0", "hello1", "hello2", "hello3", "hello4", 
                "hello5", "hello6", "hello7", "hello8", "hello9"};

    char* val[10] = {"h0", "h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8", "h9"};


    for(int i=0; i<10; i++)
    {
        unsigned int hash = myHash(key[i]);
        unsigned int idx = hash & 3;
        printf("real key: %u, real idx=%d\n", hash, idx);
    }



    // step 2: add
    printf("----------add first 5-----------------\n");
    for(int i = 0; i<5; i++)
    {
        printf("add %d\n", i);
        int ret = dictAdd(myDict, key[i], val[i]);
        printState(myDict);
        assert(ret==0);
    }

    printf("----------start rehashing..------------\n");
    for(int i=0; i<5; i++)
    {
        dictRehash(myDict, 1);
        printState(myDict);
    }

    printf("----------add  last 5.-----------------\n");
    for(int i = 5; i<10; i++)
    {
        printf("add %d\n", i);
        int ret = dictAdd(myDict, key[i], val[i]);
        printState(myDict);
        assert(ret==0);
    }




    // index.
    printf("------------index---------------\n");
    for(int i = 0; i < 10; i++)
    {
        printf("i=%d\n", i);
        char* v = dictFetchValue(myDict, key[i]);
        int ret = strcmp(v, val[i]); 
        assert(ret == 0);
    }

    char* v = dictFetchValue(myDict, "hello world2");
    assert(v == NULL);
    

    // foreach dict.
    unsigned long cur = 0;
    while(1) 
    {
        cur = dictScan(myDict, cur, dictScanCallback, NULL); 
        if(cur == 0)
        {
            break;
        }
    }

    // release. 
    dictRelease(myDict);

    return 0;
}
