#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "object_interfaces.h"

/*
 * CursorInteractable - Interface for objects that can be interacted with via cursor
 * Provides cursor placement and movement capabilities
 */
typedef struct CursorInteractable {
    Coords (*place_cursor)(const void *self, const Coords);
    void (*move_cursor)(const void *self, Coords *, const Coords);
    Coords (*get_default_coords)(const void *self);
    void (*custom_draw)(const void *self, const Cursor *, Screen *, const Coords);
    CursorConfig (*get_cursor_config)(const void *self, const Coords);
} CursorInteractable;

// Interactable macros
static inline CursorInteractable *CURSOR_INTERACT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->cursor_interactable;
}

static inline bool IS_CURSOR_INTERACTABLE(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_cursor_interactable;
}

static inline void MOVE_CURSOR(const void *object, Coords *coords, const Coords delta) {
    CURSOR_INTERACT_HANDLER(object)->move_cursor(object, coords, delta);
}

static inline Coords PLACE_CURSOR(const void *object, const Coords coords) {
    return CURSOR_INTERACT_HANDLER(object)->place_cursor(object, coords);
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

// #define CURSOR_INTERACTABLE_FULL(arena, object, _place_cursor, _move_cursor, _get_default_coords, _get_cursor_config)                                \
//     do {                                                                                                                                             \
//         if (!IS_CURSOR_INTERACTABLE(object))                                                                                                         \
//             GET_INTERFACES(object)->capabilities.is_cursor_interactable = true;                                                                      \
//         CursorInteractable *cursor_interactable = CURSOR_INTERACT_HANDLER(object);                                                                   \
//         if (cursor_interactable == NULL) {                                                                                                           \
//             GET_INTERFACES(object)->cursor_interactable = (CursorInteractable *)arena_alloc(arena, sizeof(CursorInteractable));                      \
//             cursor_interactable = CURSOR_INTERACT_HANDLER(object);                                                                                   \
//         }                                                                                                                                            \
//         cursor_interactable->place_cursor = _place_cursor;                                                                                           \
//         cursor_interactable->move_cursor = _move_cursor;                                                                                             \
//         cursor_interactable->get_default_coords = _get_default_coords;                                                                               \
//         cursor_interactable->get_cursor_config = _get_cursor_config;                                                                                 \
//         if (!_place_cursor)                                                                                                                          \
//             wprintf(L"Error in '%s': CursorInteractable interface is missing 'place_cursor' function\n", GET_INTERFACES(object)->name);              \
//         if (!_move_cursor)                                                                                                                           \
//             wprintf(L"Error in '%s': CursorInteractable interface is missing 'move_cursor' function\n", GET_INTERFACES(object)->name);               \
//         if (!_get_default_coords)                                                                                                                    \
//             wprintf(L"Error in '%s': CursorInteractable interface is missing 'get_default_coords' function\n", GET_INTERFACES(object)->name);        \
//         if (!_get_cursor_config)                                                                                                                     \
//             wprintf(L"Error in '%s': CursorInteractable interface is missing 'get_cursor_config' function\n", GET_INTERFACES(object)->name);         \
//     } while (0)

// #define CURSOR_INTERACTABLE(_place_cursor, _move_cursor, _get_default_coords, _get_cursor_config) \
//     CURSOR_INTERACTABLE_FULL(cur_arena, cur_object, _place_cursor, _move_cursor, _get_default_coords, _get_cursor_config)

#define CURSOR_INTERACTABLE_FULL(arena, object, _place_cursor, _move_cursor, _get_default_coords, _get_cursor_config, _custom_draw)                                \
    do {                                                                                                                                             \
        if (!IS_CURSOR_INTERACTABLE(object))                                                                                                         \
            GET_INTERFACES(object)->capabilities.is_cursor_interactable = true;                                                                      \
        CursorInteractable *cursor_interactable = CURSOR_INTERACT_HANDLER(object);                                                                   \
        if (cursor_interactable == NULL) {                                                                                                           \
            GET_INTERFACES(object)->cursor_interactable = (CursorInteractable *)arena_alloc(arena, sizeof(CursorInteractable));                      \
            cursor_interactable = CURSOR_INTERACT_HANDLER(object);                                                                                   \
        }                                                                                                                                            \
        cursor_interactable->place_cursor = _place_cursor;                                                                                           \
        cursor_interactable->move_cursor = _move_cursor;                                                                                             \
        cursor_interactable->get_default_coords = _get_default_coords;                                                                               \
        cursor_interactable->get_cursor_config = _get_cursor_config;                                                                                 \
        cursor_interactable->custom_draw = _custom_draw;                                                                                             \
        if (!_place_cursor)                                                                                                                          \
            wprintf(L"Error in '%s': CursorInteractable interface is missing 'place_cursor' function\n", GET_INTERFACES(object)->name);              \
        if (!_move_cursor)                                                                                                                           \
            wprintf(L"Error in '%s': CursorInteractable interface is missing 'move_cursor' function\n", GET_INTERFACES(object)->name);               \
        if (!_get_default_coords)                                                                                                                    \
            wprintf(L"Error in '%s': CursorInteractable interface is missing 'get_default_coords' function\n", GET_INTERFACES(object)->name);        \
        if (!_get_cursor_config)                                                                                                                     \
            wprintf(L"Error in '%s': CursorInteractable interface is missing 'get_cursor_config' function\n", GET_INTERFACES(object)->name);         \
    } while (0)

#define CURSOR_INTERACTABLE(_place_cursor, _move_cursor, _get_default_coords, _get_cursor_config) \
    CURSOR_INTERACTABLE_FULL(cur_arena, cur_object, _place_cursor, _move_cursor, _get_default_coords, _get_cursor_config, NULL)

#define CURSOR_INTERACTABLE_CUSTOM(_place_cursor, _move_cursor, _get_default_coords, _get_cursor_config, _custom_draw) \
    CURSOR_INTERACTABLE_FULL(cur_arena, cur_object, _place_cursor, _move_cursor, _get_default_coords, _get_cursor_config, _custom_draw)

#endif
