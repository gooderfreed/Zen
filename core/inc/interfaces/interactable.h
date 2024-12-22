#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "object_interfaces.h"

/*
 * Interactable - Interface for objects that can be interacted with
 * Provides cursor placement and movement capabilities
 */
typedef struct Interactable {
    void (*place_cursor)(const void *self, const Coords, Coords *);
    void (*move)(const void *self, Coords *, const Coords);
    Coords (*get_default_coords)(const void *self);
    void (*custom_draw)(const void *self, const Cursor *, Screen *, const Coords);
    CursorConfig (*get_cursor_config)(const void *self, const Coords);
} Interactable;

// Interactable macros
static inline const Interactable *INTERACT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->interactable;
}

static inline bool IS_INTERACTABLE(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_interactable;
}

static inline void MOVE(const void *object, Coords *coords, const Coords delta) {
    INTERACT_HANDLER(object)->move(object, coords, delta);
}

static inline void PLACE_CURSOR(const void *object, const Coords coords, Coords *base_coords) {
    INTERACT_HANDLER(object)->place_cursor(object, coords, base_coords);
}

static inline Coords GET_DEFAULT_COORDS(const void *object) {
    return INTERACT_HANDLER(object)->get_default_coords(object);
}

static inline CursorConfig GET_CURSOR_CONFIG(const void *object, const Coords coords) {
    return INTERACT_HANDLER(object)->get_cursor_config(object, coords);
}

static inline void CUSTOM_DRAW(const void *object, const Cursor *cursor, Screen *screen, const Coords coords) {
    INTERACT_HANDLER(object)->custom_draw(object, cursor, screen, coords);
}

#endif
