#include <stdlib.h>
#include <malloc_common.h>

#include <stdint.h>
#include <unistd.h>

#include <assert.h>

// This malloc tries to reimplement the algorithms used in Doug Lea's public 
// domain malloc (dlmalloc) as explained at
// <http://gee.cs.oswego.edu/dl/html/malloc.html>


// Helper functions
int find_bin_for_chunk(MallocBin *bins, ChunkInfo *chunk)
{
    for(int designated_bin=1; designated_bin<=127; designated_bin++)
    {
        // The right bin is the largest bin which has a minimum size below
        // the allocated size.
        if(designated_bin == 127 || (bins[designated_bin].size <= chunk->size &&
           bins[designated_bin+1].size > chunk->size))
        {
            return designated_bin;
        }
    }

    assert(0);
    return -1;
}

void insert_chunk_into_bin(ChunkInfo *chunk, MallocBin *bins, 
        int designated_bin)
{
    // Please insert the new chunk.
    ChunkInfo *curr = bins[designated_bin].next;

    // Bin is still empty
    if(curr == NULL)
    {
        bins[designated_bin].next = chunk;
        chunk->prev = NULL;
        chunk->next = NULL;
    }
    else
    {
        for(; curr; curr = curr->next)
        {
            // To insert, we must either have reached the end or found the place
            // where the chunk belongs.
            if(curr->next && (curr->size <= chunk->size 
                        && curr->next->size >= chunk->size))
            {
                chunk->prev = curr;
                chunk->next = curr->next;
                curr->next->prev = chunk;
                curr->next = chunk;
                break;
            }
            // This special case can only happen if we are still at the start
            // of the bin.
            if(curr->size > chunk->size)
            {
                assert(curr->prev == NULL);

                chunk->prev = NULL;
                chunk->next = curr;
                curr->prev = chunk;
                break;
            }
        }
    }
    
}


// The actual malloc
void *malloc(size_t size)
{
    // These bins hold the memory chunks our malloc may use.
    static MallocBin bins[128];

    // The wilderness chunk is the nearest chunk to the program break which may
    // be extended by the sbrk syscall.
    // This one is cached for special treatment as well as the current brk
    // position.
    static ChunkInfo *wilderness;
    static uintptr_t current_brk;

    // Initialize bins
    if(bins[1].size == 0)
    {
        // The lower bins are of exact size and 8 bytes apart for double word
        // alignment.
        for(int i=1; i <= 64; i++)
        {
            bins[i].size = 8*i;
        }

            // The upper bins are supposed to grow exponentially.
        // This algorithm comes out at about 2^31-1, give or take an order
        // of magnitude :p
        for(int i=65, exp=64; i<= 127; i++)
        {
            bins[i].size = bins[i-1].size + exp;
            if(i % 3 == 0)
            {
                exp *= 2;
            }
        }
        bins[127].size = SIZE_MAX >> 1;

        // Initialize memory, we take about a page (we assume that 4 KiB pages
        // are used, if they aren't that large, it's just bad for space usage).
        void *new_mem = sbrk(MALLOC_PAGE_SIZE);
        if(new_mem == (void*)-1)
        {
            return NULL;
        }
        current_brk = ((uintptr_t)new_mem) + MALLOC_PAGE_SIZE;

        // We don't shift the wilderness down by sizeof(size_t) the first time
        // since then .prev_size would point before the heap, but it might be
        // accessed later.
        wilderness = (ChunkInfo*) new_mem;

        // Provide boundary tag info at the start.
        wilderness->prev_size = 0;
        wilderness->size = SIZE_MAX & ~((size_t)(0x1));
                                    // The last chunk can be extended 
                                    // arbitratily long by sbrk().
        wilderness->prev = NULL;
        wilderness->next = NULL;

        // Save real length of the wilderness block at the end.
        // This is not done with normal blocks, usually the end of the block
        // is delimited by a copy of the .size info of the ChunkInfo
        *((size_t*)current_brk - 1) = MALLOC_PAGE_SIZE - 2*sizeof(size_t);

        bins[127].next = wilderness;
    }

    // Now we know that we have set up our management structures and at least
    // one chunk at our disposal. Now every malloc() works the same: Do a
    // best-fit search on our bins, split and reinitialize the found chunk as
    // needed and return memory to the caller.
    
    // Pad request for double word alignment
    while(size % 8)
    {
        size++;
    }

    ChunkInfo *designated_chunk = NULL;

    // Linear search (best-fit) for the chunk
    // The search always finishes since the wilderness chunk was assigned the
    // highest possible size.
    for(int i=1; i <= 127; i++)
    {
        // Bin is too small.
        // Since MallocBin.size gives the minimum bin size, we need to check
        // the next higher bin as well.
        if(i != 127 && bins[i].size < size && bins[i+1].size < size)
        {
            continue;
        }
        else
        {
            // Bin is empty
            if(bins[i].next == NULL)
            {
                continue;
            }
            else
            {
                // Look for best chunk.
                // The chunks in each bin are ordered by size, so we do a linear
                // search again.
                for(ChunkInfo *curr = bins[i].next; curr; curr = curr->next)
                {
                    if(curr->size > size)
                    {
                        designated_chunk = curr;
                        break;
                    }
                }
                // If we found a candidate it is automatically the best fit and
                // we finish the search.
                if(designated_chunk != NULL)
                {
                    break;
                }
            }
        }
    }

    // It is guaranteed that a chunk fits because the wilderness chunk has
    // maximum size.
    assert(designated_chunk != NULL);

    // Acquire real size of the wilderness
    size_t wilderness_size = *((size_t*)current_brk - 1);

    // The chunk fits exactly, just mark it as used, remove it from the bin, and
    // return a pointer to the usable memory area.
    if((designated_chunk == wilderness && wilderness_size == size) ||
        designated_chunk->size == size)
    {
        // if the chunk is the wilderness chunk, transform it into a regular
        // chunk by setting the proper size
        if(designated_chunk == wilderness)
        {
            designated_chunk->size = wilderness_size;
        }

        // Mark as used
        *((size_t*)((uintptr_t)designated_chunk + 2*sizeof(size_t) 
                + designated_chunk->size))
            |= 0x01;

        // Unlink
        if(designated_chunk->prev)
        {
            designated_chunk->prev->next = designated_chunk->next;
        }
        if(designated_chunk->next)
        {
            designated_chunk->next->prev = designated_chunk->prev;
        }

        // If the chunk was the wilderness chunk, then call sbrk to get a new
        // page. The first part of the struct is already there, so we need to 
        // remember to move the pointer before it.
        if(designated_chunk == wilderness)
        {
            void *new_mem = sbrk(MALLOC_PAGE_SIZE);
            if(new_mem == (void*)-1)
            {
                return NULL;
            }
            current_brk = ((uintptr_t) new_mem) + MALLOC_PAGE_SIZE;
            wilderness = (ChunkInfo*)((uintptr_t)new_mem - sizeof(size_t));

            // Maximum size and mark as unused
            wilderness->size = SIZE_MAX & ~((size_t)(0x1));

            // Get the last chunk in the last bin and insert the wilderness
            // behind it.
            ChunkInfo *last_chunk;
            for(last_chunk = bins[127].next;
                last_chunk;
                last_chunk = last_chunk->next)
            {
            }
            last_chunk->next = wilderness;
            wilderness->prev = last_chunk;
            wilderness->next = NULL;

            // Save the real size at the end of the wilderness
            *((size_t*)current_brk - 1) = MALLOC_PAGE_SIZE - 2*sizeof(size_t);
        }
        // Return user memory area.
        return (void*)((uintptr_t)designated_chunk + 2 * sizeof(size_t));
    }


    // The chunk is larger, we have to split it up, reinitialize the remaining
    // part and put it into a bin.
    assert(designated_chunk->size > size);
    
    // If the designated chunk is the wilderness chunk, check whether we have to
    // allocate new memory via sbrk() and initialize proper size values since
    // the top part of the wilderness chunk will now become a regular chunk
    if(designated_chunk == wilderness)
    {
        if(wilderness_size < size)
        {
            // The last line makes sure that we will have some memory left, even
            // if the request coincidentally is aligned with the pages.
            intptr_t request_size = size + MALLOC_PAGE_SIZE -
                (size % MALLOC_PAGE_SIZE) + 
                (!!(size % MALLOC_PAGE_SIZE == 0))*MALLOC_PAGE_SIZE;

            // We use this pointer just to check the return value and to update
            // the static values.
            void *new_mem = (ChunkInfo*) sbrk(request_size);
            if(new_mem == (void*)-1)
            {
                return NULL;
            }
            current_brk = (uintptr_t)new_mem + request_size;
            wilderness_size += request_size;

            // Make sure to update the hidden proper length of the wilderness
            // chunk.
            *((size_t*)current_brk - 1) = wilderness_size;
        }
        // Now we made sure that we have enough space and can set the proper
        // value of the chunk size
        designated_chunk->size = wilderness_size;
    }

    // Initialize new chunk at the end of the currently allocated user memory.
    ChunkInfo *new_chunk = 
        (ChunkInfo*)((uintptr_t)designated_chunk + 2*sizeof(size_t) + size);
    
    // Initalize new chunk and mark it as unused.
    // The new chunk has the size of the designated chunk minus the user
    // memory and the overhead for a new chunk header.
    new_chunk->prev_size = size & ~((size_t)0x1);
    new_chunk->size = designated_chunk->size - size - 2*sizeof(size_t);
    designated_chunk->size = size & ~((size_t)0x1);

     // Unlink the designated chunk.
    if(designated_chunk->prev)
    {
        designated_chunk->prev->next = designated_chunk->next;
    }
    if(designated_chunk->next)
    {
        designated_chunk->next->prev = designated_chunk->prev;
    }

    int new_chunk_bin;
    // Sort the new chunk into an appropriate bin.
    new_chunk_bin = find_bin_for_chunk(bins, new_chunk);
    insert_chunk_into_bin(new_chunk, bins, new_chunk_bin);

    // In addition put designated chunk in new bin
    new_chunk_bin = find_bin_for_chunk(bins, designated_chunk);
    insert_chunk_into_bin(designated_chunk, bins, new_chunk_bin);
    
    // All organization has happened now, we can gladly return the
    // designated chunk payload to the caller.
    return (void*)((uintptr_t)designated_chunk + 2 * sizeof(size_t));
}

