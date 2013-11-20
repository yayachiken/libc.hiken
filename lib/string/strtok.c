#include <string.h>

char *strtok(char *str, const char *delim)
{
    static char *last_ptr;
    if(!str)
    {
        last_ptr = str;
    }

    size_t delim_len = strlen(delim);
    while(*last_ptr != '\0' && !strncmp(last_ptr, delim, delim_len))
    {
        last_ptr++;
    }

    if(*last_ptr != '\0')
    {
        last_ptr++;
        return last_ptr - 1;
    }
    else
    {
        return NULL;
    }
}


