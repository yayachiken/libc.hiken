#include <string.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t size)
{
    for(size_t i=0; i<size; i++)
    {
        *((char*)dest+i) = *((char*)src+i);
    }

    return dest;
}

void *memset(void *dest, int byte, size_t size)
{
    for(size_t i=0; i<size; i++)
    {
        *((char*)dest+i) = (char)byte;
    }

    return dest;
}

char *strcpy(char *restrict dest, const char *restrict src)
{
    size_t i = 0;
    while(src[i] != '\0')
    {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';

    return dest;
}

char *strncpy(char *restrict dest, const char *restrict src, size_t size)
{
    size_t i = 0;

    // Copy string up to the limit of size
    while(i < size && src[i] != '\0')
    {
        dest[i] = src[i];
        ++i;
    }

    // Fill rest of string with zeroes
    while(i < size)
    {
        dest[i] = '\0';
        ++i;
    }

    return dest;
}

