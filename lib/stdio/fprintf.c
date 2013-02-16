#include <stdio.h>

int fprintf(FILE *stream, const char *format, ...)
{
    int ret;

    va_list args;
    va_start(args, format);
    ret =  vfprintf(stream, format, args);
    va_end(args);
    
    return ret;
}

