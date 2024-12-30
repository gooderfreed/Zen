#include "../inc/solitaire.h"

/*
 * Print controls
 * Draws the controls on the screen
 */
static void print_controls(const void *controls_pointer, Screen *screen, const Cursor *cursor) {
    (void)controls_pointer;
    (void)cursor;

    int text_y = 18;
    int text_x = 12;

    insert_text(screen, text_y + 0, text_x + 10, " Controls:",                          COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 1, text_x + 0, "  w            - move up",             COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 2, text_x + 0, "  a            - move left",           COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 3, text_x + 0, "  s            - move down",           COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 4, text_x + 0, "  d            - move right",          COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 5, text_x + 0, "  space\\enter  - action",             COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 6, text_x + 0, "  q            - menu",                COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 7, text_x + 0, "  esc          - exit",                COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 8, text_x + 0, "  ctrl+a       - move left globally",  COLOR_BOLD, COLOR_BLACK);
    insert_text(screen, text_y + 9, text_x + 0, "  ctrl+d       - move right globally", COLOR_BOLD, COLOR_BLACK);

    insert_text(screen, text_y + 11, text_x + 12, " Back", COLOR_BOLD, COLOR_BLACK);
}

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
    return (CursorConfig) {.type = CURSOR_RIGHT_SLIM};
}

/*
 * On back click
 * Handles the back button click
 */
static void on_back_click(void *controls_pointer, void *context) {
    Controls *controls = (Controls *)controls_pointer;
    Core *core = (Core *)context;
    SET_DRAWABLE_ACTIVE(controls, false);
    prepare_menu_screen(core->screen);
    core_global_move(core, CURSOR_LEFT);
}


/*
 * Init controls
 * Initializes the controls
 */
Controls init_controls(void) {
    Controls controls = {0};

    static Drawable drawable = {
        .is_active = false,
        .print = print_controls
    };

    static const Interactable interactable = {
        .place_cursor        = place_cursor_in_controls,
        .move                = move_in_controls,
        .get_default_coords  = get_default_coords,
        .get_cursor_config   = get_cursor_config_in_controls
    };

    static Button back_button = {
        .coords = {.x = 0, .y = 0},
        .on_click = on_back_click
    };

    static ButtonHandler button_handler = {
        .buttons_count = 1,
        .buttons = {
            [0] = &back_button,
        }
    };

    controls.interfaces = (ObjectInterfaces) {
        .name = "Controls",
        .capabilities = {
            .is_drawable     = true,
            .have_buttons    = true,
            .is_interactable = true,
        },
        .drawable       = &drawable,
        .interactable   = &interactable,
        .button_handler = &button_handler
    };
    
    return controls;
}
