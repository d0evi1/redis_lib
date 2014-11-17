/*
 * Description: 
 *     History: junle, 2014/11/17, create
 */

#include "zmalloc.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char* pMem = zmalloc(sizeof(char)*100);
    strcpy(pMem, "hello world.");
    printf("str=%s\n", pMem);
    printf("size=%d\n", zmalloc_size(pMem));
    zlibc_free(pMem);

    return 0;
}
