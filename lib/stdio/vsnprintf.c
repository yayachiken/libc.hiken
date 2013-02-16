#include <stdio.h>
#include <printf_common.h>

void snprintf_output(Output op, int *chars_printed, size_t size, char character)
{
    if(*chars_printed > size)
    {
        if(character == '\0')
        {
            op.buffer[size] = '\0';
        }
        (*chars_printed)++;
        return;
    }
    else
    {
        op.buffer[(*chars_printed)++] = character;
    }
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    Output op;
    op.buffer = str;
    return printf_common(op, format, ap, snprintf_output, size);
}

