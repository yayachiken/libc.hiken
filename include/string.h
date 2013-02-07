#ifndef LIBCHIKEN_STRING_H
#define LIBCHIKEN_STRING_H

#include <stddef.h>

void *memset(void*, int, size_t);
void *memcpy(void *restrict, const void *restrict, size_t);
char *strncpy(char *restrict, const char *restrict, size_t);
char *strcpy(char *restrict, const char* restrict);

#endif

