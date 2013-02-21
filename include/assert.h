// NO include guards here! assert() must be redefined on each 
// #include <assert.h>!!

#ifdef assert
#undef assert
#endif

#ifndef NDEBUG
#define assert(ignore) ((void) 0)
#else
#include <stdio.h>
#define XSTR(x) STR(x)
#define STR(x) #x
#define assert(cond) \
    do { \
        if(!(cond)) { \
            printf("Assertion " #cond " failed at " \
                __FILE__ ":" XSTR(__LINE__) "\n"); \
            _exit(1); \
        } \
    } while(0)
#endif

