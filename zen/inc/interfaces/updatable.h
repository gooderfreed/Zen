#ifndef UPDATABLE_H
#define UPDATABLE_H

#include "object_interfaces.h"

/*
 * Updateable - Interface for objects that can be updated
 * Provides update method
 */
typedef struct Updateable {
    void *context;
    void (*update)(void *self, void *context);
} Updateable;

// Updateable macros
static inline Updateable *UPDATEABLE_HANDLER(const void *object) {
    return GET_INTERFACES(object)->updateable;
}

static inline bool IS_UPDATEABLE(const void *object) {
    return GET_INTERFACES(object)->capabilities.requires_update;
}

static inline void UPDATE(void *object) {
    UPDATEABLE_HANDLER(object)->update(object, UPDATEABLE_HANDLER(object)->context);
}

static inline void SET_UPDATE_CONTEXT(void *object, void *context) {
    UPDATEABLE_HANDLER(object)->context = context;
}

#define UPDATEABLE_FULL(arena, object, _update)                                                                            \
    do {                                                                                                                   \
        if (!IS_UPDATEABLE(object))                                                                                        \
            GET_INTERFACES(object)->capabilities.requires_update = true;                                                   \
        Updateable *updateable = UPDATEABLE_HANDLER(object);                                                               \
        if (updateable == NULL) {                                                                                          \
            GET_INTERFACES(object)->updateable = (Updateable *)arena_alloc(arena, sizeof(Updateable));                     \
            updateable = UPDATEABLE_HANDLER(object);                                                                       \
            updateable->context = NULL;                                                                                    \
        }                                                                                                                  \
        updateable->update = _update;                                                                                      \
        if (!_update)                                                                                                      \
            wprintf(L"Error in '%s': Updateable interface is missing 'update' function\n", GET_INTERFACES(object)->name);  \
    } while (0)

#define UPDATEABLE(_update) \
    UPDATEABLE_FULL(cur_arena, cur_object, _update)


#endif
