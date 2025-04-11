#ifndef CURSOR_H
#define CURSOR_H

#include "../components.h"

/*
 * Cursor movement vectors
 * Predefined movement directions for cursor
 */
#define CURSOR_UP    (Coords){.x = 0,  .y = -1}
#define CURSOR_DOWN  (Coords){.x = 0,  .y = 1}
#define CURSOR_LEFT  (Coords){.x = -1, .y = 0}
#define CURSOR_RIGHT (Coords){.x = 1,  .y = 0}
#define CURSOR_STAY  (Coords){.x = 0,  .y = 0}

/*
 * Cursor structure
 * Represents cursor state including position, selected cards and current object
 */
struct Cursor {
    Coords coords;          // Current cursor position
    Container *cards;       // Currently selected cards
    void *subject;          // Object under cursor
};

/*
 * Cursor style definitions
 * Predefined cursor styles for different purposes
 */
enum CursorType {
    CURSOR_SLIM,

    CURSOR_LEFT_SLIM,
    CURSOR_RIGHT_SLIM,
    CURSOR_UP_SLIM,
    CURSOR_DOWN_SLIM,

    CURSOR_WIDE,

    CURSOR_LEFT_WIDE,
    CURSOR_RIGHT_WIDE,
    CURSOR_UP_WIDE,
    CURSOR_DOWN_WIDE,

    CURSOR_SELECT,
    CURSOR_CUSTOM
};

/*
 * Cursor configuration structure
 * Contains cursor type and custom symbol
 */
typedef struct CursorConfig {
    CursorType type;
    TextEffect effect;
    Color background;
    Color foreground;
    wchar_t *custom;
} CursorConfig;

/*
 * Cursor functions
 * Cursor manipulation and rendering
 */
Cursor *init_cursor(Arena *arena, Container *cursor_cards);
void cursor_set_subject(Cursor *cursor, void *subject);
void cursor_set_coords(Cursor *cursor, Coords coords);
void print_cursor(Cursor *cursor, Screen *screen);

#endif
