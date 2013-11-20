#include <string.h>
#include <stdlib.h>

char *strndup(const char *str, size_t n)
{
    char *ret = (char *) malloc(n);
    strncpy(ret, str, n);
    return ret;
}

