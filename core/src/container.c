/*
 * Copyright 2024 Cheban Sergiy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef CUSTOM_CONTAINER_IMPL

/*
 * Container implementation
 * Handles container operations for cards
 */
#include "../inc/core.h"

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
inline bool container_is_empty(Container *container) {
    return container->size == 0;
}

/*
 * Set source of container
 * Sets source of container to specified pointer
 */
inline void container_set_source(Container *container, void *source) {
    container->source = source;
}

#ifdef CONTAINER_DYNAMIC
    /*
     * Initialize container of cards
     * Allocates memory for container and sets initial size and source
     */
    Container container_init(int length) {
        Container container = {
            .size = 0,
            .source = NULL,
            .length = length,
            .container = (void **)malloc(length * sizeof(void *))
        };
        return container;
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
     * Free container of cards
     * Frees memory allocated for container
     */
    void container_free(Container *container) {
        free(container->container);
    }
#else
    /*
     * Initialize container of cards
     * Sets initial size and source
     */
    Container container_init(void) {
        Container container = {
            .size = 0,
            .source = NULL,
            .container = {0}
        };
        return container;
    }

    /*
     * Add element to container
     * Adds element to container if there is enough space
     */
    void container_add_element(Container *container, void *element) {
        if (container->size >= CONTAINER_SIZE) return;
        container->container[container->size++] = element;
    }

    /*
     * Get element from container
     * Returns element at specified index
     */
    void *container_get_element(Container *container, int index) {
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

#endif
