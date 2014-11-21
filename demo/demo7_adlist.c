/*
 * Description: 
 *     History: junle, 2014/11/21, create
 */

#include "adlist.h"
#include <stdio.h>


int main()
{
    list* alist = listCreate();
    int v[10] = {1,2,3,4,5,6,7,8,9,10};

    for(int i=0; i<10; i++)
    {
        listAddNodeHead(alist, &v[i]);
    }


    listIter* it = listGetIterator(alist, AL_START_TAIL);
    listNode* node;
    while((node = listNext(it)) != NULL) {
        printf("val=%d\n", *((int*)node->value));
    }


    listRelease(alist);

    return 0;
}
