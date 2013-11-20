#ifndef LIBCHIKEN_STRING_H
#define LIBCHIKEN_STRING_H

#include <stddef.h>

void *memset(void *, int, size_t);
void *memcpy(void *restrict, const void *restrict, size_t);
char *strncpy(char *restrict, const char *restrict, size_t);
char *strcpy(char *restrict, const char *restrict);
size_t strlen(const char *);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
char *strdup(const char *);
char *strndup(const char *, size_t);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
char *strtok(char *, const char *);

#endif

