#ifndef LIBCHIKEN_MALLOC_COMMON_H
#define LIBCHIKEN_MALLOC_COMMON_H

#include <stddef.h>

#define MALLOC_PAGE_SIZE 4096

typedef struct
{
    size_t size;   // Chunk size in bytes
    void *next; // The first chunk in the bin
} MallocBin;

typedef struct _ChunkInfo
{
    size_t prev_size;   // size of previous chunk
    size_t size;    // size of this chunk, because of double word alignment
                    // the last bits aren't used, these hold information whether
                    // the *previous* chunk is in use (1) or not (0)
    struct _ChunkInfo *prev;     // overwritten by user data when used
    struct _ChunkInfo *next;     // dito

    // Here begins user data
} ChunkInfo;

#endif

