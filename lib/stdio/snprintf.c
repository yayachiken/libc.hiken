#include <stdio.h>

int snprintf(char *str, size_t size, const char *format, ...)
{
    int ret;
    va_list va;
    va_start(va, format);
    ret = vsnprintf(str, size, format, va);
    va_end(va);
    return ret;
}
