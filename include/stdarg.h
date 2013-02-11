#ifndef LIBCHIKEN_STDARG_H
#define LIBCHIKEN_STDARG_H

#define va_list __builtin_va_list

#define va_start(vl, lastarg) __builtin_va_start(vl, lastarg)
#define va_end(vl) __builtin_va_end(vl)
#define va_arg(vl, type) __builtin_va_arg(vl, type)

#endif

