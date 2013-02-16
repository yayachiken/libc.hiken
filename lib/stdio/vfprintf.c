#include <stdio.h>
#include <unistd.h>
#include <printf_common.h>

void vfprintf_output(Output op, int *chars_printed, size_t size, char character)
{
    write(op.file->handle, &character, 1);
    return;
}

int vfprintf(FILE *stream, const char *format, va_list ap)
{
    Output op;
    op.file = stream;
    return printf_common(op, format, ap, vfprintf_output, 0);
}

