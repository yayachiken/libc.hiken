#include <string.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t size)
{
    for(size_t i=0; i<size; i++)
    {
        *((char*)dest+i) = *((char*)src+i);
    }

    return dest;
}
