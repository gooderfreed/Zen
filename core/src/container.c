#ifndef CUSTOM_CONTAINER_IMPL

/*
 * Container implementation
 * Handles container operations for cards
 */
#include "../inc/core.h"

/*
 * Initialize container of cards
 * Allocates memory for container and sets initial size and source
 */
Container *container_init(Arena *arena, int length) {
    Container *container = (Container *)arena_alloc(arena, sizeof(Container));

    *container = (Container) {
        .size = 0,
        .length = length,
        .source = NULL,
        .container = (void **)arena_alloc(arena, length * sizeof(void *))
    };

    return container;
}

/*
 * Free container of cards
 * Frees memory allocated for container
 */
void container_free(Arena *arena, Container *container) {
    arena_free_block(arena, container);
}

/*
 * Clear container of cards
 * Sets all elements to NULL and resets size and source
 */
void container_clear_container(Container *container) {
    for (int i = 0; i < container->size; i++) {
        container->container[i] = NULL;
    }
    container->size = 0;
    container->source = NULL;
}

/*
 * Check if container is empty
 * Returns true if container has no elements
 */
inline bool container_is_empty(const Container *container) {
    return container->size == 0;
}

/*
 * Set source of container
 * Sets source of container to specified pointer
 */
inline void container_set_source(Container *container, void *source) {
    container->source = source;
}

/*
 * Add element to container
 * Adds element to container if there is enough space
 */
void container_add_element(Container *container, void *element) {
    if (container->size >= container->length) return;
    container->container[container->size++] = element;
}

/*
 * Get element from container
 * Returns element at specified index
 */
void *container_get_element(const Container *container, const int index) {
    if (index >= container->size) return NULL;
    return container->container[index];
}

/*
 * Pop element from container
 * Pops element from container and returns it
 */
void *container_pop_element(Container *container) {
    if (container->size == 0) return NULL;
    void *element = container->container[--container->size];
    container->container[container->size] = NULL;
    if (container->size == 0) container->source = NULL;
    return element;
}
#endif
