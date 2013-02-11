#ifndef LIBCHIKEN_ALLOCA_H
#define LIBCHIKEN_ALLOCA_H

#include <stddef.h>

#ifdef __GNUC__
#define alloca(x) __builtin_alloca(x)
#else
void *alloca(size_t);
#endif

#endif

