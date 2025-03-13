#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "object_interfaces.h"

/*
 * Dynamic - Interface for objects that can be dynamically allocated
 * Provides free method
 */
typedef struct Dynamic {
    void (*free)(void *self);
} Dynamic;

// Dynamic macros
static inline const Dynamic *DYNAMIC_HANDLER(const void *object) {
    return GET_INTERFACES(object)->dynamic;
}

static inline bool IS_DYNAMIC(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_dynamic;
}

static inline void FREE(void *object) {
    DYNAMIC_HANDLER(object)->free(object);
}

#define DYNAMIC_FULL(arena, object, _free)                                                      \
    do {                                                                                        \
        if (!IS_DYNAMIC(object))                                                                \
            GET_INTERFACES(object)->capabilities.is_dynamic = true;                             \
        Dynamic *dynamic = DYNAMIC_HANDLER(object);                                             \
        if (dynamic == NULL) {                                                                  \
            GET_INTERFACES(object)->dynamic = (Dynamic *)arena_alloc(arena, sizeof(Dynamic));   \
            dynamic = DYNAMIC_HANDLER(object);                                                  \
        }                                                                                       \
        dynamic->free = _free;                                                                  \
    } while (0)

#define DYNAMIC(_free) \
    DYNAMIC_FULL(cur_arena, cur_object, _free)

#endif
