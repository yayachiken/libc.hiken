#include <string.h>

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
