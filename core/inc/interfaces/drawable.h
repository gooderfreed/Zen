#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "object_interfaces.h"

/*
 * Drawable - Interface for objects that can be drawn on screen
 * Implementations must provide print method
 */
typedef struct Drawable {
    bool is_active;
    void (*print)(const void *, Screen *, const Cursor *);
} Drawable;

// Drawable macros
static inline Drawable *DRAW_HANDLER(const void *object) {
    return GET_INTERFACES(object)->drawable;
}

static inline bool IS_DRAWABLE(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_drawable;
}

static inline void DRAW(const void *object, Screen *screen, Cursor *cursor) {
    DRAW_HANDLER(object)->print(object, screen, cursor);
}

static inline bool IS_ACTIVE_DRAWABLE(const void *object) {
    return IS_DRAWABLE(object) && DRAW_HANDLER(object)->is_active;
}

static inline void SET_DRAWABLE_ACTIVE(void *object, bool active) {
    DRAW_HANDLER(object)->is_active = active;
}

static inline void SET_DRAWABLE_INACTIVE(void *object) {
    DRAW_HANDLER(object)->is_active = false;
}

#define DRAWABLE_FULL(arena, object, active, _print)                                                                        \
    do {                                                                                                                    \
        if (!IS_DRAWABLE(object))                                                                                           \
            GET_INTERFACES(object)->capabilities.is_drawable = true;                                                        \
        Drawable *drawable = DRAW_HANDLER(object);                                                                          \
        if (drawable == NULL) {                                                                                             \
            GET_INTERFACES(object)->drawable = (Drawable *)arena_alloc(arena, sizeof(Drawable));                            \
            drawable = DRAW_HANDLER(object);                                                                                \
            drawable->is_active = active;                                                                                   \
        }                                                                                                                   \
        drawable->print = _print;                                                                                           \
        if (!_print)                                                                                                        \
            wprintf(L"Error in '%s': Drawable interface is missing 'print' function\n", GET_INTERFACES(object)->name);      \
    } while (0)

#define DRAWABLE(_print)                              \
    DRAWABLE_FULL(cur_arena, cur_object, true, _print)

#define DRAWABLE_INACTIVE(_print) \
    DRAWABLE_FULL(cur_arena, cur_object, false, _print)

#endif
