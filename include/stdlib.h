#ifndef LIBCHIKEN_STDLIB_H
#define LIBCHIKEN_STDLIB_H

#include <stddef.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#ifndef NULL
#define NULL ((void*)0)
#endif

void *malloc(size_t);
void *realloc(void*, size_t);
void free(void*);

#endif

