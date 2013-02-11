#include <stdio.h>

int printf(const char *format, ...)
{
    int ret;

    va_list args;
    va_start(args, format);
    ret =  vprintf(format, args);
    va_end(args);
    
    return ret;
}

