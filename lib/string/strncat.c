#include <string.h>
#include <stdlib.h>

char *strncat(char *dest, const char *src, size_t n)
{
    size_t len = strlen(dest);
    size_t i;
    for(i = 0; src[len + i] != '\0' &&  i < n; i++)
    {
        dest[len + i] = src[len + i];
    }
    dest[len + i] = '\0';

    return dest;
}

