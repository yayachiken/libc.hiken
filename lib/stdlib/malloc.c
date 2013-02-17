#include <stdlib.h>
#include <malloc_common.h>

#include <stdint.h>
#include <unistd.h>

#include <assert.h>

// This malloc tries to reimplement the algorithms used in Doug Lea's public 
// domain malloc (dlmalloc) as explained at
// <http://gee.cs.oswego.edu/dl/html/malloc.html>

void *malloc(size_t size)
{
    // These bins hold the memory chunks our malloc may use.
    static MallocBin bins[128];

    // The wilderness chunk is the nearest chunk to the program break which may
    // be extended by the sbrk syscall.
    // This one is cached for special treatment.
    static ChunkInfo *wilderness;

    // Initialize bins
    if(bins[2].size == 0)
    {
        // The lower bins are of exact size and 8 bytes apart for double word
        // alignment.
        for(int i=2; i <= 64; i++)
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
        wilderness = (ChunkInfo*) sbrk(MALLOC_PAGE_SIZE);
        if(wilderness < 0)
        {
            return NULL;
        }

        // Provide boundary tag info at the start.
        wilderness->prev_size = 0;
        wilderness->size = SIZE_MAX; // The last chunk can be extended 
                                    // arbitratily long by sbrk().
        wilderness->prev = NULL;
        wilderness->next = NULL;

        // Apply boundary tag to the end and mark as unused.
        ((ChunkInfo*)(((uintptr_t)wilderness)
                + MALLOC_PAGE_SIZE - sizeof(size_t)))->prev_size
            = SIZE_MAX & ~((size_t)(0x1));

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
    for(int i=2; i <= 127; i++)
    {
        // Bin is too small.
        // Since MallocBin.size gives the minimum bin size, we need to check
        // the next higher bin as well.
        if(bins[i].size < size && bins[i+1].size < size)
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
    // maximum size
    assert(designated_chunk != NULL);

    // The chunk fits exactly, just mark it as used, remove it from the bin, and
    // return a pointer to the usable memory area
    if(designated_chunk != wilderness && designated_chunk->size == size)
    {
        // Mark as used
        *((size_t*)((uintptr_t)designated_chunk)
                + 2*sizeof(size_t) + designated_chunk->size)
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

        // Return user memory area
        return (void*)((uintptr_t)designated_chunk + 2 * sizeof(size_t));
    }
    
    return NULL;
}

