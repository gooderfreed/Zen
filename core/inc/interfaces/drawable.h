#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "object_interfaces.h"

/*
 * Drawable - Interface for objects that can be drawn on screen
 * Implementations must provide print method
 */
typedef struct Drawable {
    void (*print)(const void *, Screen *, const Cursor *);
} Drawable;

// Drawable macros
static inline const Drawable *DRAW_HANDLER(const void *object) {
    return GET_INTERFACES(object)->drawable;
}

static inline bool IS_DRAWABLE(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_drawable;
}

static inline void DRAW(const void *object, Screen *screen, Cursor *cursor) {
    DRAW_HANDLER(object)->print(object, screen, cursor);
}

#endif
