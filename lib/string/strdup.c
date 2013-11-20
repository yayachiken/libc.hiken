#include <string.h>
#include <stdlib.h>

char *strdup(const char *str)
{
    char *ret = (char *) malloc(strlen(str));
    strcpy(ret, str);
    return ret;
}

