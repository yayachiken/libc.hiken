#ifndef LIBCHIKEN_CONSOLE_H
#define LIBCHIKEN_CONSOLE_H

#ifdef LIBCHIKEN_FOR_LINUX
#   include <unistd.h>
#   define CONSOLE_WRITE(x) \
    do { \
        char c = (x); \
        write(1, &c, 1); \
    } while(0)
#else
#   define CONSOLE_WRITE(x)
#endif

#endif

