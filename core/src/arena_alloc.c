/*
 * Button handling implementation
 * Manages button creation, removal, and click handling
 */
#include "../inc/core.h"

/*
 * Update the next_free pointer of the previous free block
 * Sets the next_free pointer to new_val or updates the arena's free_blocks if no previous free block exists
 */
static inline void update_next_of_prev_block(Arena *arena, Block *block, Block *new_val) {
    if (block->prev_free) {
        block->prev_free->next_free = new_val;
    } else {
        arena->free_blocks = new_val;
    }
}

/*
 * Update the prev_free pointer of the next free block
 * Sets the prev_free pointer to new_val if the next free block exists
 */
static inline void update_prev_of_next_block(Block *block, Block *new_val) {
    if (block->next_free) {
        block->next_free->prev_free = new_val;
    }
}

/*
 * Update the maximum free block size on allocation
 * Adjusts max_free_block_size and its count if a larger block is freed
 */
static inline void update_max_free_block_on_free(Arena *arena, size_t block_size) {
    if (block_size > arena->max_free_block_size) {
        arena->max_free_block_size = block_size;
        arena->max_free_block_size_count = 1;
    } else if (block_size == arena->max_free_block_size) {
        arena->max_free_block_size_count++;
    }
}

/*
 * Create an empty block at the end of the given block
 * Sets up a new block with size 0 and adjusts pointers
 */
static inline Block *create_empty_block(Block *prev_block) {
    // prep data for new block
    Block *prev_tail = prev_block;
    void *new_chunk = (char *)prev_tail->data + prev_tail->size;

    // create new block
    Block *block = (Block *)new_chunk;
    block->size = 0;
    block->data = (char *)new_chunk + sizeof(Block);
    block->is_free = true;
    block->next = NULL;
    block->prev = NULL;

    return block;
}

/*
 * Find the second largest free block size
 * Iterates through free blocks to find the largest size excluding the current max
 */
static inline size_t find_second_largest_size(Arena *arena) {
    Block *block = arena->free_blocks;
    if (!block) return 0;

    size_t second_largest = 0;
    while (block) {
        if (block->size > second_largest && block->size != arena->max_free_block_size) {
            second_largest = block->size;
        }
        block = block->next_free;
    }
    return second_largest;
}

/*
 * Update the maximum free block size on block detachment
 * Adjusts the max_free_block_size and its count when a block of the current max size is detached
 */
static inline void update_max_free_block_on_detach(Arena *arena, size_t block_size) {
    if (block_size == arena->max_free_block_size) {
        arena->max_free_block_size_count--;
        if (arena->max_free_block_size_count == 0) {
            arena->max_free_block_size = find_second_largest_size(arena);
            if (arena->max_free_block_size != 0) arena->max_free_block_size_count = 1;
        }
    }
}

/*
 * Wipe a free block and adjust arena metadata
 * Handles tail adjustments, updates max free block size, and cleans up the free block
 */
static void wipe_free_block(Arena *arena, Block *free_block, Block *tail) {
    if (tail->next == NULL && tail->size == 0) {
        free_block->next = NULL;

        arena->free_size_in_tail += free_block->size + sizeof(Block);
        arena->tail = free_block;

        update_max_free_block_on_detach(arena, free_block->size);
        update_next_of_prev_block(arena, free_block, free_block->next_free);
        update_prev_of_next_block(free_block, free_block->prev_free);

        free_block->size = 0;
    }
}

/*
 * Merge two adjacent blocks in the arena
 * Combines the sizes of two blocks and updates their pointers
 * If the second block is the last and empty, it gets wiped
 */
static void merge_blocks(Arena *arena, Block *block1, Block *block2) {
    // wipe free block if it's the last block
    if (block2->next == NULL && block2->size == 0) {
        wipe_free_block(arena, block1, block2);
        return;
    }

    // merge blocks
    block1->size += block2->size + sizeof(Block);
    Block *block_after = block2->next;
    block_after->prev = block1;
    block1->next = block_after;

    update_next_of_prev_block(arena, block2, block2->next_free);
    update_max_free_block_on_free(arena, block1->size);
}

/*
 * Split a block into two parts
 * Creates a new block with the remaining size and marks the first part as allocated
 */
static void split_block(Arena *arena, Block *block, size_t size) {
    size_t new_block_size = block->size - size - sizeof(Block);
    size_t block_size = block->size;
    block->size = size;
    block->is_free = false;

    Block *new_block = create_empty_block(block);
    new_block->size = new_block_size;
    new_block->is_free = true;

    if (block->next) {
        new_block->next = block->next;
        block->next->prev = new_block;
    }
    else {
        new_block->next = NULL;
    }
    block->next = new_block;
    new_block->prev = block;

    update_next_of_prev_block(arena, block, new_block);
    update_prev_of_next_block(block, new_block);

    new_block->next_free = block->next_free;
    new_block->prev_free = block->prev_free;

    update_max_free_block_on_detach(arena, block_size);
}

/*
 * Allocate memory from the tail of the arena
 * Updates the tail block and creates a new block if there is enough space
 */
static void *alloc_in_tail(Arena *arena, size_t size) {
    // get a tail block
    Block *block = arena->tail;
    // update block
    block->size = size;
    block->is_free = false;
    arena->free_size_in_tail -= size;

    // create new block
    if (arena->free_size_in_tail >= sizeof(Block)) {
        Block *new_block = create_empty_block(arena->tail);
        block->next = new_block;
        new_block->prev = block;
        // update arena
        arena->tail = new_block;
        arena->free_size_in_tail -= sizeof(Block);
    }
    else {
        block->size += arena->free_size_in_tail;
        arena->free_size_in_tail = 0;
    }

    // return allocated data pointer
    return block->data;
}

/*
 * Allocate memory from a free block in the arena
 * Finds a suitable block, splits it if necessary, and marks it as allocated
 */
static void *alloc_in_free_block(Arena *arena, size_t size) {
    Block *block = arena->free_blocks;

    // find block with enough space
    while (block->size < size) {
        block = block->next_free;
    }

    // split block if it's too big
    if (block->size > size + sizeof(Block) + MIN_BUFFER_SIZE) {
        split_block(arena, block, size);
    }
    else {
        block->is_free = false;

        update_next_of_prev_block(arena, block, block->next_free);
        update_prev_of_next_block(block, block->prev_free);
        update_max_free_block_on_detach(arena, block->size);
    }

    return block->data;
}

/*
 * Allocate memory in the arena
 * Tries to allocate memory in the tail or from free blocks
 * Returns NULL if there is not enough space
 */
void *arena_alloc(Arena *arena, size_t size) {
    if (size == 0) return NULL;

    // check if area has enough space in the end
    if (arena->free_size_in_tail >= size) {
        return alloc_in_tail(arena, size);
    }

    // check if there are free blocks with enough space
    if (arena->max_free_block_size >= size) {
        return alloc_in_free_block(arena, size);
    }
    return NULL;
}

/*
 * Free a block of memory in the arena
 * Marks the block as free, merges it with adjacent free blocks if possible,
 * and updates the free block list
 */
void arena_free_block(Arena *arena, void *data) {
    Block *block = (Block *)((void *)((char *)data - sizeof(Block)));
    block->is_free = true;

    Block *prev = block->prev;
    Block *next = block->next;

    update_max_free_block_on_free(arena, block->size);

    if (!next) {
        block->is_free = true;
        arena->free_size_in_tail += block->size;
        block->size = 0;
    }
    else if (next->is_free) {
        merge_blocks(arena, block, next);
    }

    if (prev && prev->is_free) {
        merge_blocks(arena, prev, block);
        block = prev;
    }

    if (block->size != 0) {
        if (!arena->free_blocks) {
            arena->free_blocks = block;
        } else {
            Block *prev_free = arena->free_blocks;
            block->next_free = prev_free;
            prev_free->prev_free = block;
            arena->free_blocks = block;
        }
    }
}

/*
 * Create a static arena
 * Initializes an arena using preallocated memory and sets up the first block
 * Returns NULL if the provided size is too small
 */
Arena *arena_new_static(void *memory, size_t size) {
    if (size < sizeof(Arena) + sizeof(Block) + MIN_BUFFER_SIZE) return NULL;

    Arena *arena = (Arena *)memory;
    arena->capacity = size - sizeof(Arena);
    arena->data = (char *)memory + sizeof(Arena);
    arena->max_free_block_size = 0;
    arena->max_free_block_size_count = 0;
    
    Block *block = (Block *)arena->data;
    block->size = 0;
    block->data = (char *)arena->data + sizeof(Block);
    block->is_free = true;
    block->next = NULL;
    block->prev = NULL;

    arena->tail = block;
    arena->free_size_in_tail = arena->capacity - sizeof(Block);

    arena->is_dynamic = false;

    return arena;
}

/*
 * Create a dynamic arena
 * Allocates memory for the arena and initializes it as a dynamic arena
 * Returns NULL if the requested size is too small
 */
Arena *arena_new_dynamic(size_t size) {
    if (size < sizeof(Arena) + sizeof(Block) + MIN_BUFFER_SIZE) return NULL;
    void *data = malloc(size);
    Arena *arena = arena_new_static(data, size);

    arena->is_dynamic = true;

    return arena;
}

/*
 * Reset the arena
 * Clears the arena's blocks and resets it to the initial state without freeing memory
 */
void arena_reset(Arena *arena) {
    Block *block = (Block *)arena->data;
    block->size = 0;
    block->is_free = true;
    block->next = NULL;
    block->prev = NULL;
    block->next_free = NULL;
    block->prev_free = NULL;

    arena->tail = block;
    arena->free_blocks = NULL;
    arena->free_size_in_tail = arena->capacity - sizeof(Block);
}

/*
 * Free a dynamic arena
 * Releases memory for dynamically allocated arenas
 */
void arena_free(Arena *arena) {
    if (arena->is_dynamic) {
        free(arena);
    }
}

/*
 * Print arena details
 * Outputs the current state of the arena and its blocks, including free blocks
 * Useful for debugging and understanding memory usage
 */
void print_arena(Arena *arena) {
    wprintf(L"Arena: %p\n", arena);
    wprintf(L"Arena Full Size: %lu\n", arena->capacity + sizeof(Arena));
    wprintf(L"Arena Data Size: %lu\n", arena->capacity);
    wprintf(L"Data: %p\n", arena->data);
    wprintf(L"Tail: %p\n", arena->tail);
    wprintf(L"Free Size in Tail: %lu\n", arena->free_size_in_tail);
    wprintf(L"Max Free Block Size: %lu\n", arena->max_free_block_size);
    wprintf(L"Max Free Block Size Count: %lu\n", arena->max_free_block_size_count);
    wprintf(L"\n");

    size_t occupied_data = 0;
    size_t occupied_meta = 0;
    size_t len = 0;


    Block *block = (Block *)arena->data;
    while (block != NULL) {
        occupied_data += block->size;
        occupied_meta += sizeof(Block);
        len++;
        wprintf(L"  Block: %p\n", block);
        wprintf(L"  Block Full Size: %lu\n", block->size + sizeof(Block));
        wprintf(L"  Block Data Size: %lu\n", block->size);
        wprintf(L"  Is Free: %d\n", block->is_free);
        wprintf(L"  Data Pointer: %p\n", block->data);
        wprintf(L"  Next: %p\n", block->next);
        wprintf(L"  Prev: %p\n", block->prev);
        wprintf(L"\n");
        block = block->next;
    }

    wprintf(L"Arena Free Blocks\n");

    Block *free_block = arena->free_blocks;
    if (free_block == NULL) wprintf(L"  None\n");
    while (free_block != NULL) {
        wprintf(L"  Block: %p\n", free_block);
        wprintf(L"  Block Full Size: %lu\n", free_block->size + sizeof(Block));
        wprintf(L"  Block Data Size: %lu\n", free_block->size);
        wprintf(L"  Is Free: %d\n", free_block->is_free);
        wprintf(L"  Data Pointer: %p\n", free_block->data);
        wprintf(L"  Next: %p\n", free_block->next);
        wprintf(L"  Prev: %p\n", free_block->prev);
        wprintf(L"  Next Free: %p\n", free_block->next_free);
        wprintf(L"  Prev Free: %p\n", free_block->prev_free);
        wprintf(L"\n");
        free_block = free_block->next_free;
    }
    wprintf(L"\n");

    wprintf(L"Arena occupied data size: %lu\n", occupied_data);
    wprintf(L"Arena occupied meta size: %lu\n", occupied_meta);
    wprintf(L"Arena occupied full size: %lu\n", occupied_data + occupied_meta);
    wprintf(L"Arena block count: %lu\n", len);
}
