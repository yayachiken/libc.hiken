#ifndef LIBCHIKEN_UNISTD_H
#define LIBCHIKEN_UNISTD_H

#include <errno.h>
#include <stdint.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>

void _exit(int);
int close(int);
int execve(const char*, char *const[], char *const[]);
int fork(void);
int getpid(void);
int isatty(int);
int kill(int, int);
int link(char*, char*);
int lseek(int, int, int);
int open(const char*, int, int);
int read(int, void*, int);
void* sbrk(int);
int unlink(char*);
int wait(int*);
int write(int, const void*, int);

#endif

