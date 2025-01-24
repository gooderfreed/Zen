
#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include "components.h"

#ifndef MIN_BUFFER_SIZE
    // Default minimum buffer size for the arena.
    #define MIN_BUFFER_SIZE 16
#endif

/*
 * Memory block structure.
 * Represents a chunk of memory and metadata for its management within the arena.
 */
struct Block {
    size_t size;          // Size of the data block.
    void *data;           // Pointer to the start of user data in this block.
    Block *next;          // Pointer to the next block in the global list.
    Block *prev;          // Pointer to the previous block in the global list.

    bool is_free;         // Flag indicating whether the block is free.
    Block *next_free;     // Pointer to the next free block in the free list.
    Block *prev_free;     // Pointer to the previous free block in the free list.
};

/*
 * Memory arena structure.
 * Manages a pool of memory, block allocation, and block states.
 */
struct Arena {
    size_t capacity;                     // Total capacity of the arena.
    void *data;                          // Pointer to the start of the memory managed by the arena.

    bool is_dynamic;                     // Flag indicating if the arena uses dynamic allocation.

    Block *tail;                         // Pointer to the last block in the global list.
    Block *free_blocks;                  // Pointer to the list of free blocks.

    size_t free_size_in_tail;            // Free space available in the tail block.
    size_t max_free_block_size;          // Size of the largest free block.
    size_t max_free_block_size_count;    // Number of blocks matching max_free_block_size.
};

/*
* Arena functions
* Arena manipulation and memory allocation
*/
Arena *arena_new_dynamic(size_t size);
Arena *arena_new_static(void *memory, size_t size);
void arena_reset(Arena *arena);
void *arena_alloc(Arena *arena, size_t size);
void arena_free_block(Arena *arena, void *data);
void arena_free(Arena *arena);
void print_arena(Arena *arena);

#endif
