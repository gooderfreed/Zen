#include "../inc/solitaire.h"

/*
 * Print menu
 * Draws the menu on the screen
 */
static void print_menu(const void *menu_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Menu *menu = (Menu *)menu_pointer;

    int text_offset = 1;
    int text_y = 18;

    Color text_color = COLOR_BOLD;
    Color background_color = COLOR_BLACK;

    insert_text(screen, text_y + 0, text_offset, ".dP\"Y8  dP\"Yb  88     88 888888    db    88 88\"\"Yb 88888", text_color, background_color);
    insert_text(screen, text_y + 1, text_offset, "`Ybo.\" dP   Yb 88     88   88     dPYb   88 88__dP 88__ ",    text_color, background_color);
    insert_text(screen, text_y + 2, text_offset, "o.`Y8b Yb   dP 88  .o 88   88    dP__Yb  88 88\"Yb  88\"\" ",  text_color, background_color);
    insert_text(screen, text_y + 3, text_offset, "8bodP'  YbodP  88ood8 88   88   dP\"\"\"\"Yb 88 88  Yb 88888", text_color, background_color);

    if (menu->start_game) {
        insert_text(screen, text_y + 8, SCREEN_WIDTH/2 - 6, " Start", text_color, background_color);
    }
    else {
        insert_text(screen, text_y + 7, SCREEN_WIDTH/2 - 6, " Continue", text_color, background_color);
        insert_text(screen, text_y + 8, SCREEN_WIDTH/2 - 6, " New Game", text_color, background_color);
    }
    insert_text(screen, text_y + 9,  SCREEN_WIDTH/2 - 6, " Controls", text_color, background_color);
    insert_text(screen, text_y + 10, SCREEN_WIDTH/2 - 6, " Exit",     text_color, background_color);
}

/*
 * Place cursor in menu
 * Places the cursor in the menu
 */
static void place_cursor_in_menu(const void *menu_pointer, const Coords cursor_coords, Coords *target_coords) {
    (void)menu_pointer;
    target_coords->y = 25 + cursor_coords.y;
    target_coords->x = SCREEN_WIDTH/2 - 6;
}

/*
 * Move in menu
 * Moves the cursor in the menu
 */
static void move_in_menu(const void *menu_pointer, Coords *coords, const Coords delta) {
    Menu *menu = (Menu *)menu_pointer;

    if (delta.x != 0) return;
    short new_y = coords->y + delta.y;
    if (new_y == 0 && menu->start_game) new_y = -1;
    if (new_y < 0) new_y = BUTTON_HANDLER_SIZE - 1;
    coords->y = (new_y + BUTTON_HANDLER_SIZE) % BUTTON_HANDLER_SIZE;
    coords->y = coords->y == 0 ? (short)menu->start_game: coords->y;
}

/*
 * Get default coords
 * Gets the default coords of the menu
 */
static Coords get_default_coords(const void *menu_pointer) {
    Menu *menu = (Menu *)menu_pointer;
    SET_DRAWABLE_ACTIVE(menu, true);
    return (Coords) {.x = 0, .y = menu->start_game ? 1 : 0};
}

/*
 * Get cursor config
 * Gets the cursor config of the menu
 */
static CursorConfig get_cursor_config_in_menu(const void *menu_pointer, const Coords cursor_coords) {
    (void)menu_pointer;
    (void)cursor_coords;
    return (CursorConfig) {.type = CURSOR_RIGHT_SLIM};
}

/*
 * On continue click
 * Handles the continue button click
 */
static void on_continue_click(void *menu_pointer, void *context) {
    (void)context;
    CORE_CHANGE_LAYER(menu_pointer, GAME_ID);
}

/*
 * On start click
 * Handles the start button click
 */
static void on_start_click(void *menu_pointer, void *context) {
    Menu *menu = (Menu *)menu_pointer;
    menu->start_game = false;

    Game *game = (Game *)context;
    game_reset(game);
    CORE_CHANGE_LAYER(menu_pointer, GAME_ID);
}

/*
 * On exit click
 * Handles the exit button click
 */
static void on_exit_click(void *menu_pointer, void *context) {
    (void)context;
    CORE_SHUTDOWN(menu_pointer);
    exit(0);
}

/*
 * On controls click
 * Handles the controls button click
 */
static void on_controls_click(void *menu_pointer, void *context) {
    (void)context;
    
    Menu *menu = (Menu *)menu_pointer;
    SET_DRAWABLE_ACTIVE(menu, false);
    prepare_menu_screen(CORE_GET_SCREEN(menu_pointer));
    CORE_GLOBAL_MOVE(menu_pointer, CURSOR_RIGHT);
}

/*
 * Init menu
 * Initializes the menu
 */
Menu init_menu(void) {
    Menu menu = {
        .start_game = true
    };

    static Drawable drawable = {
        .is_active = true,
        .print = print_menu
    };

    static const Interactable interactable = {
        .place_cursor        = place_cursor_in_menu,
        .move                = move_in_menu,
        .get_default_coords  = get_default_coords,
        .get_cursor_config   = get_cursor_config_in_menu
    };

    static Button continue_button = {
        .coords = {.x = 0, .y = 0},
        .on_click = on_continue_click
    };

    static Button start_button = {
        .coords = {.x = 0, .y = 1},
        .on_click = on_start_click
    };

    static Button controls_button = {
        .coords = {.x = 0, .y = 2},
        .on_click = on_controls_click
    };

    static Button exit_button = {
        .coords = {.x = 0, .y = 3},
        .on_click = on_exit_click
    };

    static ButtonHandler button_handler = {
        .buttons_count = 4,
        .buttons = {
            [0] = &continue_button,
            [1] = &start_button,
            [2] = &controls_button,
            [3] = &exit_button
        }
    };

    menu.interfaces = (ObjectInterfaces) {
        .name = "Menu",
        .capabilities = {
            .is_drawable     = true,
            .have_buttons    = true,
            .is_interactable = true,
            .requires_core   = true
        },
        .drawable       = &drawable,
        .interactable   = &interactable,
        .button_handler = &button_handler
    };
    
    return menu;
}
