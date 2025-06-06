#ifndef CORE_DEPENDENT_H
#define CORE_DEPENDENT_H

#include "object_interfaces.h"

/*
 * CoreDependent - Interface for objects that depend on the core
 * Provides core dependent capabilities
 */
typedef struct CoreDependent {
    Zen *zen;

    void (*change_layer) (Zen *zen, int layer);
    void (*action)       (Zen *zen);
    void (*global_move)  (Zen *zen, Coords move);
    void (*local_move)   (Zen *zen, Coords move);
    void (*shutdown)     (Zen *zen);

    Screen *(*get_screen)(Zen *zen);
} CoreDependent;

// CoreDependent macros
static inline const CoreDependent *CORE_DEPENDENT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->core_dependent;
}

static inline bool IS_CORE_DEPENDENT(const void *object) {
    return GET_INTERFACES(object)->capabilities.requires_core;
}

static inline Zen *GET_CORE(const void *object) {
    if (IS_CORE_DEPENDENT(object)) {
        return CORE_DEPENDENT_HANDLER(object)->zen;
    }
    return NULL;
}

static inline Screen *CORE_GET_SCREEN(const void *object) {
    if (IS_CORE_DEPENDENT(object)) {
        return CORE_DEPENDENT_HANDLER(object)->get_screen(GET_CORE(object));
    }
    return NULL;
}

static inline void CORE_CHANGE_LAYER(const void *object, int layer) {
    if (IS_CORE_DEPENDENT(object)) {
        CORE_DEPENDENT_HANDLER(object)->change_layer(GET_CORE(object), layer);
    }
}

static inline void CORE_ACTION(const void *object) {
    if (IS_CORE_DEPENDENT(object)) {
        CORE_DEPENDENT_HANDLER(object)->action(GET_CORE(object));
    }
}

static inline void CORE_GLOBAL_MOVE(const void *object, Coords move) {
    if (IS_CORE_DEPENDENT(object)) {
        CORE_DEPENDENT_HANDLER(object)->global_move(GET_CORE(object), move);
    }
}

static inline void CORE_LOCAL_MOVE(const void *object, Coords move) {
    if (IS_CORE_DEPENDENT(object)) {
        CORE_DEPENDENT_HANDLER(object)->local_move(GET_CORE(object), move);
    }
}

static inline void CORE_SHUTDOWN(const void *object) {
    if (IS_CORE_DEPENDENT(object)) {
        CORE_DEPENDENT_HANDLER(object)->shutdown(GET_CORE(object));
    }
}

#define CORE_DEPENDENT_FULL(arena, object)                               \
    do {                                                                 \
        if (!IS_CORE_DEPENDENT(object))                                  \
            GET_INTERFACES(object)->capabilities.requires_core = true;   \
    } while (0)

#define CORE_DEPENDENT() \
    CORE_DEPENDENT_FULL(cur_arena, cur_object)

#endif
