// NO include guards here! assert() must be redefined on each 
// #include <assert.h>!!

#ifndef NDEBUG
#define assert(ignore) ((void) 0)
#else
#define assert(cond) \
    do { \
        if(cond) { \
            printf("Assertion " ##cond " failed at " ##__FILE__ ":" ##__LINE__); \
            _exit(); \
        } \
    while(0)
#endif

