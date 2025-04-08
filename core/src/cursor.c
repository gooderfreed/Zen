/*
 * Cursor implementation
 * Handles cursor state and visualization
 */
#include "../inc/core.h"


/*
 * Initialize cursor
 * Allocates memory for cursor and sets initial state
 */
Cursor *init_cursor(Arena *arena, Container *cursor_cards) {
    Cursor *cursor = (Cursor *)arena_alloc(arena, sizeof(Cursor));

    *cursor = (Cursor) {
        .coords = {0},
        .cards = cursor_cards,
        .subject = NULL,
    };

    return cursor;
}

/*
 * Assign subject to cursor
 * Links cursor to a subject and resets its coordinates
 */
void cursor_set_subject(Cursor *cursor, void *subject) {
    cursor->subject = subject;
    cursor->coords = GET_DEFAULT_COORDS(subject);
}

/*
 * Update cursor coordinates
 * Sets cursor position to given coordinates
 */
void cursor_set_coords(Cursor *cursor, Coords coords) {
    cursor->coords = coords;
}

/*
 * Print cursor on screen
 * Handles cursor visualization based on current state and position
 */
void print_cursor(Cursor *cursor, Screen *screen) {
    // If subject is interactable, let it place the cursor
    if (cursor && IS_CURSOR_INTERACTABLE(cursor->subject)) {
        Coords spot = PLACE_CURSOR(cursor->subject, cursor->coords);

        CursorConfig config = GET_CURSOR_CONFIG(cursor->subject, cursor->coords);

        // If subject has custom cursor, let it draw it
        if (config.type == CURSOR_CUSTOM) {
            CUSTOM_DRAW(cursor->subject, cursor, screen, spot);
            return;
        }

        // Otherwise, draw default cursor
        screen_draw_cursor(screen, spot, config);
    }
    
}

