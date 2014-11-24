/*
 * Description: 
 *     History: junle, 2014/11/23, create
 */

#include <stdio.h>
#include <inttypes.h>
#include "endianconv.h"

int main()
{
    // 16.
    uint16_t num16 =  0x253f;
    printf("intrev16(), before: 0x%x, after: 0x%x \n", num16, intrev16(num16));
    printf("intrev16ifbe(), before: 0x%x, after: 0x%x \n", num16, intrev16ifbe(num16));


    // 32.
    uint32_t num32 = 0x5432FECD;
    printf("intrev32(), before 0x%x, after: 0x%x \n", num32, intrev32(num32));
    printf("intrev32ifbe(), before 0x%x, after: 0x%x \n", num32, intrev32ifbe(num32));


    // 64.
    uint64_t num64 = 0x12345678A1B2C3D4;
    printf("intrev64(), before 0x%"PRIx64", after: 0x%"PRIx64" \n", num64, intrev64(num64));
    printf("intrev64ifbe(), before 0x%"PRIx64", after: 0x%"PRIx64" \n", num64, intrev64ifbe(num64));


    return 0;
}
