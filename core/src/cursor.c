/*
 * Cursor implementation
 * Handles cursor state and visualization
 */
#include "../inc/core.h"


/*
 * Initialize cursor with starting object and position
 * Returns cursor structure ready for use
 */
Cursor init_cursor(void *start_object, Coords start_coords, Container *cursor_cards) {
    Cursor cursor = {
        .coords = start_coords,
        .cards = cursor_cards,
        .subject = start_object,
    };

    return cursor;
}

/*
 * Print cursor on screen
 * Handles cursor visualization based on current state and position
 */
void print_cursor(Cursor *cursor, Screen *screen) {
    // Get base coordinates for cursor
    Coords base_coords = {
        .x = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2), 
        .y = CARD_HEIGHT
    };

    // If subject is interactable, let it place the cursor
    if (IS_INTERACTABLE(cursor->subject)) {
        PLACE_CURSOR(cursor->subject, cursor->coords, &base_coords);

        CursorConfig config = GET_CURSOR_CONFIG(cursor->subject, cursor->coords);

        // If subject has custom cursor, let it draw it
        if (config.type == CURSOR_CUSTOM) {
            CUSTOM_DRAW(cursor->subject, cursor, screen, base_coords);
            return;
        }

        // Otherwise, draw default cursor
        screen_draw_cursor(screen, base_coords, config.type);
    }
    
}

