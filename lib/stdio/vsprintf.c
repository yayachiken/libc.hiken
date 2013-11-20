#include <stdio.h>
#include <printf_common.h>

void sprintf_output(Output op, size_t *chars_printed, size_t size, char character)
{
    op.buffer[(*chars_printed)++] = character;
}

int vsprintf(char *str, const char *format, va_list ap)
{
    Output op;
    op.buffer = str;
    return printf_common(op, format, ap, sprintf_output, 0);
}

