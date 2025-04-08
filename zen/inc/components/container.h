#ifndef CONTAINER_H
#define CONTAINER_H

#include "components.h"

/*
 * Container - Universal container
 * Used for moving data between objects and tracking their source
 */
typedef struct Container {
    int size;
    int length;
    void *source;
    void **container;
} Container;

/*
* Container functions
* Container manipulation
*/
#ifndef CUSTOM_CONTAINER_IMPL
    Container *container_init(Arena *arena, int length);
    void container_free(Container *container);
    void container_add_element(Container *container, void *element);
    void container_clear_container(Container *container);
    bool container_is_empty(const Container *container);
    void container_set_source(Container *container, void *source);
    void *container_pop_element(Container *container);
    void *container_get_element(const Container *container, const int index);
#endif

#endif
