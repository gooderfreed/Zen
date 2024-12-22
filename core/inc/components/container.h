#ifndef CONTAINER_H
#define CONTAINER_H

#include "components.h"

/*
 * CardsContainer - Container for card operations
 * Used for moving cards between objects and tracking their source
 */
#ifndef CONTAINER_DYNAMIC
#ifndef CONTAINER_SIZE
    #define CONTAINER_SIZE 1
#endif
#endif

typedef struct Container {
    int size;
    void *source;
    #ifndef CONTAINER_DYNAMIC
        void *container[CONTAINER_SIZE];
    #else
        int length;
        void **container;
    #endif
} Container;

#ifndef CUSTOM_CONTAINER_IMPL
    void container_add_element(Container *container, void *element);
    void container_clear_container(Container *container);
    bool container_is_empty(const Container *container);
    void container_set_source(Container *container, void *source);
    void *container_pop_element(Container *container);
    void *container_get_element(const Container *container, const int index);
    #ifndef CONTAINER_DYNAMIC
        Container container_init(void);
    #else
        Container container_init(int length);
        void container_free(Container *container);
    #endif
#endif

#endif
