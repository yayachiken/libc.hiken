#include <string.h>
#include <stdlib.h>

char *strcat(char *dest, const char *src)
{
    size_t i;
    for(i = strlen(dest); src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return dest;
}

