#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "object_interfaces.h"

/*
 * CursorInteractable - Interface for objects that can be interacted with via cursor
 * Provides cursor placement and movement capabilities
 */
typedef struct CursorInteractable {
    void (*place_cursor)(const void *self, const Coords, Coords *);
    void (*move_cursor)(const void *self, Coords *, const Coords);
    Coords (*get_default_coords)(const void *self);
    void (*custom_draw)(const void *self, const Cursor *, Screen *, const Coords);
    CursorConfig (*get_cursor_config)(const void *self, const Coords);
} CursorInteractable;

// Interactable macros
static inline const CursorInteractable *CURSOR_INTERACT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->cursor_interactable;
}

static inline bool IS_CURSOR_INTERACTABLE(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_cursor_interactable;
}

static inline void MOVE_CURSOR(const void *object, Coords *coords, const Coords delta) {
    CURSOR_INTERACT_HANDLER(object)->move_cursor(object, coords, delta);
}

static inline void PLACE_CURSOR(const void *object, const Coords coords, Coords *base_coords) {
    CURSOR_INTERACT_HANDLER(object)->place_cursor(object, coords, base_coords);
}

static inline Coords GET_DEFAULT_COORDS(const void *object) {
    return CURSOR_INTERACT_HANDLER(object)->get_default_coords(object);
}

static inline CursorConfig GET_CURSOR_CONFIG(const void *object, const Coords coords) {
    return CURSOR_INTERACT_HANDLER(object)->get_cursor_config(object, coords);
}

static inline void CUSTOM_DRAW(const void *object, const Cursor *cursor, Screen *screen, const Coords coords) {
    CURSOR_INTERACT_HANDLER(object)->custom_draw(object, cursor, screen, coords);
}

#endif
