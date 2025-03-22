/*
 * Button handling implementation
 * Manages button creation, removal, and click handling
 */
#include "../inc/core.h"

/*
 * Safe next block pointer
 * Checks if the next block exists and is not in the tail free space
 */
static inline bool has_next_block(Arena *arena, Block *block) {
    // Calculate the offset of the next block (as a number, not a pointer)
    size_t next_offset = (size_t)((char *)block - (char *)arena->data) + 
                         sizeof(Block) + block->size;
    
    // Verify that the next block offset is within valid limits
    // And not in the tail free space
    return (next_offset < arena->capacity) && 
           ((block != arena->tail) || (arena->free_size_in_tail == 0));
}

/*
 * Safe next block pointer
 * Checks if the next block exists and is not in the tail free space
 */
static inline Block *next_block(Arena *arena, Block *block) {
    if (!has_next_block(arena, block)) {
        return NULL;
    }
    
    // Only if the check passed, calculate the pointer
    return (Block *)(void *)((char *)block + sizeof(Block) + block->size);
}


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
    void *new_chunk = (char *)block_data(prev_tail) + prev_tail->size;

    // create new block
    Block *block = (Block *)new_chunk;
    block->size = 0;
    block->is_free = true;
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
    if (next_block(arena, tail) == NULL && tail->size == 0) {

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
    if (next_block(arena, block2) == NULL && block2->size == 0) {
        wipe_free_block(arena, block1, block2);
        return;
    }

    // merge blocks
    block1->size += block2->size + sizeof(Block);
    Block *block_after = next_block(arena, block2);
    block_after->prev = block1;

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

    if (next_block(arena, block)) {
        next_block(arena, block)->prev = new_block;
    }
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
        new_block->prev = block;
        // update arena
        arena->tail = new_block;
        arena->free_size_in_tail -= sizeof(Block);
    }
    else {
        block->size += arena->free_size_in_tail;
        arena->free_size_in_tail = 0;
    }

    block->arena = arena;
    // return allocated data pointer
    return block_data(block);
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

    block->arena = arena;
    // return allocated data pointer
    return block_data(block);
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

static void arena_free_block_full(Arena *arena, void *data) {
    Block *block = (Block *)((void *)((char *)data - sizeof(Block)));
    block->is_free = true;

    Block *prev = block->prev;
    Block *next = next_block(arena, block);

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
 * Free a block of memory in the arena
 * Marks the block as free, merges it with adjacent free blocks if possible,
 * and updates the free block list
 */
void arena_free_block(void *data) {
    Block *block = (Block *)((void *)((char *)data - sizeof(Block)));
    Arena *arena = block->arena;
    arena_free_block_full(arena, data);
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
    block->is_free = true;
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

    occupied_meta = sizeof(Arena);

    Block *block = (Block *)arena->data;
    while (block != NULL) {
        occupied_data += block->size;
        occupied_meta += sizeof(Block);
        len++;
        wprintf(L"  Block: %p\n", block);
        wprintf(L"  Block Full Size: %lu\n", block->size + sizeof(Block));
        wprintf(L"  Block Data Size: %lu\n", block->size);
        wprintf(L"  Is Free: %d\n", block->is_free);
        wprintf(L"  Data Pointer: %p\n", block_data(block));
        wprintf(L"  Next: %p\n", next_block(arena, block));
        wprintf(L"  Prev: %p\n", block->prev);
        wprintf(L"\n");
        block = next_block(arena, block);
    }

    wprintf(L"Arena Free Blocks\n");

    Block *free_block = arena->free_blocks;
    if (free_block == NULL) wprintf(L"  None\n");
    while (free_block != NULL) {
        wprintf(L"  Block: %p\n", free_block);
        wprintf(L"  Block Full Size: %lu\n", free_block->size + sizeof(Block));
        wprintf(L"  Block Data Size: %lu\n", free_block->size);
        wprintf(L"  Is Free: %d\n", free_block->is_free);
        wprintf(L"  Data Pointer: %p\n", block_data(free_block));
        wprintf(L"  Next: %p\n", next_block(arena, free_block));
        wprintf(L"  Prev: %p\n", free_block->prev);
        wprintf(L"  Next Free: %p\n", free_block->next_free);
        wprintf(L"  Prev Free: %p\n", free_block->prev_free);
        wprintf(L"\n");
        free_block = next_block(arena, free_block);
    }
    wprintf(L"\n");

    wprintf(L"Arena occupied data size: %lu\n", occupied_data);
    wprintf(L"Arena occupied meta size: %lu\n", occupied_meta);
    wprintf(L"Arena occupied full size: %lu\n", occupied_data + occupied_meta);
    wprintf(L"Arena block count: %lu\n", len);
}

/*
 * Print a fancy visualization of the arena memory
 * Displays a bar chart of the arena's memory usage, including free blocks, occupied data, and metadata
 * Uses ANSI escape codes to colorize the visualization
 */
void print_fancy(Arena *arena, size_t bar_size) {
    if (!arena) return;
    
    size_t total_size = arena->capacity;

    wprintf(L"\nArena Memory Visualization [%zu bytes]\n", total_size + sizeof(Arena));
    wprintf(L"┌");
    for (int i = 0; i < (int)bar_size; i++) wprintf(L"─");
    wprintf(L"┐\n│");
    
    // Size of one segment of visualization in bytes
    double segment_size = (double)(total_size / bar_size);
    
    // Iterate through each segment of visualization
    for (int i = 0; i < (int)bar_size; i++) {
        // Calculate the start and end positions of the segment in memory
        size_t segment_start = (size_t)(i * segment_size);
        size_t segment_end = (size_t)((i + 1) * segment_size);
        
        // Determine which data type prevails in this segment
        char segment_type = ' '; // Empty by default
        size_t max_overlap = 0;
        
        // Check arena metadata
        size_t arena_meta_end = sizeof(Arena);
        if (segment_start < arena_meta_end) {
            size_t overlap = segment_start < arena_meta_end ? 
                (arena_meta_end > segment_end ? segment_end - segment_start : arena_meta_end - segment_start) : 0;
            if (overlap > max_overlap) {
                max_overlap = overlap;
                segment_type = '@'; // Arena metadata
            }
        }
        
        // Check each block
        size_t current_pos = 0;
        Block *current = arena->data;
        
        while (current) {
            // Position of block metadata
            size_t block_meta_start = current_pos;
            size_t block_meta_end = block_meta_start + sizeof(Block);
            
            // Check intersection with block metadata
            if (segment_start < block_meta_end && segment_end > block_meta_start) {
                size_t overlap = (segment_end < block_meta_end ? segment_end : block_meta_end) - 
                             (segment_start > block_meta_start ? segment_start : block_meta_start);
                if (overlap > max_overlap) {
                    max_overlap = overlap;
                    segment_type = '@'; // Block metadata
                }
            }
            
            // Position of block data
            size_t block_data_start = block_meta_end;
            size_t block_data_end = block_data_start + current->size;
            
            // Check intersection with block data
            if (segment_start < block_data_end && segment_end > block_data_start) {
                size_t overlap = (segment_end < block_data_end ? segment_end : block_data_end) - 
                             (segment_start > block_data_start ? segment_start : block_data_start);
                if (overlap > max_overlap) {
                    max_overlap = overlap;
                    segment_type = current->is_free ? ' ' : '#'; // Free or occupied block
                }
            }
            
            current_pos = block_data_end;
            current = next_block(arena, current);
        }

        // Check tail free memory
        if (arena->free_size_in_tail > 0) {
            size_t tail_start = total_size - arena->free_size_in_tail;
            if (segment_start < total_size && segment_end > tail_start) {
                size_t overlap = (segment_end < total_size ? segment_end : total_size) - 
                               (segment_start > tail_start ? segment_start : tail_start);
                if (overlap > max_overlap) {
                    max_overlap = overlap;
                    segment_type = '-'; // Free tail
                }
            }
        }
        
        // Display the corresponding symbol with color
        if (segment_type == '@') {
            wprintf(L"\033[43m@\033[0m"); // Yellow for metadata
        } else if (segment_type == '#') {
            wprintf(L"\033[41m#\033[0m"); // Red for occupied blocks
        } else if (segment_type == ' ') {
            wprintf(L"\033[42m \033[0m"); // Green for free blocks
        } else if (segment_type == '-') {
            wprintf(L"\033[40m \033[0m"); // Black for empty space
        }
        
    }
    
    wprintf(L"│\n└");
    for (int i = 0; i < (int)bar_size; i++) wprintf(L"─");
    wprintf(L"┘\n");

    wprintf(L"Legend: ");
    wprintf(L"\033[43m @ \033[0m - Used Meta blocks, ");
    wprintf(L"\033[41m # \033[0m - Used Data blocks, ");
    wprintf(L"\033[42m   \033[0m - Free blocks, ");
    wprintf(L"\033[40m   \033[0m - Empty space\n\n");
}

