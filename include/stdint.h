#ifndef LIBCHIKEN_STDINT_H
#define LIBCHIKEN_STDINT_H

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
typedef unsigned long size_t;
#endif

typedef unsigned long uintptr_t;
typedef long intptr_t;

typedef long ptrdiff_t;

#define SIZE_MAX 4294967295UL

#define INTMAX_MAX 9223372036854775807LL
#define INTMAX_MIN -9223372036854775807LL

#define UINTMAX_MAX 18446744073709551615LL
#define UINTMAX_MIN -18446744073709551615LL

#endif

