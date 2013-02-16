#ifndef LIBCHIKEN_STDIO_H
#define LIBCHIKEN_STDIO_H

#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
typedef int size_t;
#endif

#ifndef va_list
#define va_list __builtin_va_list
#endif

typedef struct
{
    off_t offset;
    int whence;
} fpos_t;

typedef struct
{
    int handle;
    fpos_t current_position;
    char *byte_buffer;
    int error_indicator;
    int eof_indicator;
} FILE;

#define _IOFBF 1
#define _IOLBF 2
#define _IONBF 3

#define BUFSIZ 1024

#define EOF (-1)

#define FOPEN_MAX 8
#define FILENAME_MAX 255
#define L_tmpnam 8

#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 3

#define TMP_MAX 255

FILE *stdin, *stdout, *stderr;

int printf(const char*, ...);
int fprintf(FILE*, const char*, ...);
int sprintf(char*, const char*, ...);
int snprintf(char*, size_t, const char*, ...);

int vprintf(const char*, va_list);
int vfprintf(FILE*, const char*, va_list);
int vsprintf(char*, const char*, va_list);
int vsnprintf(char*, size_t, const char*, va_list);

#endif

