#include <string.h>

void *memset(void *dest, int byte, size_t size)
{
    for(size_t i=0; i<size; i++)
    {
        *((char*)dest+i) = (char)byte;
    }

    return dest;
}
