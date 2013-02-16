#include <stdio.h>
#include <unistd.h>
#include <printf_common.h>

void vprintf_output(Output op, int *chars_printed, size_t size, char character)
{
    // We always print to stdout: Ignore all limitations
    write(1, &character, 1);
    return;
}

int vprintf(const char *format, va_list ap)
{
    Output op;
    return printf_common(op, format, ap, vprintf_output, 0);
}

