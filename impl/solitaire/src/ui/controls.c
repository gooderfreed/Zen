#include "../../inc/solitaire.h"

// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Print controls
 * Draws the controls on the screen
 */
static void print_controls(const void *controls_pointer, Screen *screen, const Cursor *cursor) {
    (void)controls_pointer;
    (void)cursor;
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_WHITE);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_NONE, COLOR_WHITE, fat_border);

    int text_y = 18;
    int text_x = 12;
    Color text_color = COLOR_NONE;
    Color background_color = COLOR_NONE;

    insert_text(screen, text_y + 0, text_x + 10, " Controls:",                          text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 1, text_x + 0, "  w            - move up",             text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 2, text_x + 0, "  a            - move left",           text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 3, text_x + 0, "  s            - move down",           text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 4, text_x + 0, "  d            - move right",          text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 5, text_x + 0, "  space\\enter  - action",             text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 6, text_x + 0, "  q            - menu",                text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 7, text_x + 0, "  esc          - exit",                text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 8, text_x + 0, "  ctrl+a       - move left globally",  text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 9, text_x + 0, "  ctrl+d       - move right globally", text_color, background_color, Effect_Bold);

    insert_text(screen, text_y + 11, text_x + 12, " Back", text_color, background_color, Effect_Bold);
}




// =========================
//  CURSOR_INTERACTABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Place cursor in controls
 * Places the cursor in the controls
 */
static void place_cursor_in_controls(const void *controls_pointer, const Coords cursor_coords, Coords *target_coords) {
    (void)controls_pointer;
    (void)cursor_coords;
    target_coords->y = 29;
    target_coords->x = SCREEN_WIDTH/2 - 6 + 1;
}

/*
 * Move in controls
 * Moves the cursor in the controls
 */
static void move_in_controls(const void *controls_pointer, Coords *coords, const Coords delta) {
    (void)controls_pointer;
    (void)coords;
    (void)delta;
    return;
}

/*
 * Get default coords
 * Gets the default coords of the controls
 */
static Coords get_default_coords(const void *controls_pointer) {
    Controls *controls = (Controls *)controls_pointer;
    SET_DRAWABLE_ACTIVE(controls, true);
    return (Coords) {.x = 0, .y = 0};
}

/*
 * Get cursor config
 * Gets the cursor config of the controls
 */
static CursorConfig get_cursor_config_in_controls(const void *controls_pointer, const Coords cursor_coords) {
    (void)controls_pointer;
    (void)cursor_coords;
    return (CursorConfig) {
        .type = CURSOR_RIGHT_SLIM,
        .background = COLOR_NONE,
        .foreground = COLOR_WHITE,
        .effect = Effect_Bold,
    };
}




// =========================
//  BUTTONS IMPLEMENTATION
// =========================
/*
 * On back click
 * Handles the back button click
 */
static void on_back_click(void *controls_pointer, void *context) {
    (void)context;

    Controls *controls = (Controls *)controls_pointer;
    SET_DRAWABLE_ACTIVE(controls, false);
    CORE_GLOBAL_MOVE(controls_pointer, CURSOR_LEFT);
}




/*
 * Init controls
 * Initializes the controls
 */
Controls *init_controls(Arena *arena) {
    Controls *controls = (Controls *)arena_alloc(arena, sizeof(Controls));
    
    INTERFACES(arena, controls, {
        DRAWABLE_INACTIVE(print_controls);
        CURSOR_INTERACTABLE(place_cursor_in_controls, move_in_controls, get_default_coords, get_cursor_config_in_controls);
        BUTTONS_HANDLER({
            BUTTONS_GROUP({
                BUTTON(on_back_click);
            });
        });
        CORE_DEPENDENT();
    });
    
    return controls;
}
