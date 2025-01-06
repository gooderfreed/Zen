
#include "../inc/solitaire.h"

/*
 * Print win screen
 * Draws the win screen on the screen
 */
static void print_win_screen(const void *win_screen_pointer, Screen *screen, const Cursor *cursor) {
    (void)win_screen_pointer;
    (void)cursor;

    int text_y = (SCREEN_HEIGHT - WIN_SCREEN_HEIGHT) / 2 + 2;
    int text_x = (SCREEN_WIDTH  - WIN_SCREEN_WIDTH)  / 2 + 1;

    Color text_color = COLOR_BOLD;
    Color background_color = COLOR_BLACK;

    insert_text(screen, text_y + 0, text_x + 8, "Yb  dP  dP\"Yb  88   88", text_color, background_color);
    insert_text(screen, text_y + 1, text_x + 8, " YbdP  dP   Yb 88   88", text_color, background_color);
    insert_text(screen, text_y + 2, text_x + 8, "  8P   Yb   dP Y8   8P", text_color, background_color);
    insert_text(screen, text_y + 3, text_x + 8, " dP     YbodP  `YbodP'", text_color, background_color);
    
    insert_text(screen, text_y + 5, text_x + 3, "db   d8b   db d888888b d8b   db", text_color, background_color);
    insert_text(screen, text_y + 6, text_x + 3, "88   I8I   88   `88'   888o  88", text_color, background_color);
    insert_text(screen, text_y + 7, text_x + 3, "88   I8I   88    88    88V8o 88", text_color, background_color);
    insert_text(screen, text_y + 8, text_x + 3, "Y8   I8I   88    88    88 V8o88", text_color, background_color);
    insert_text(screen, text_y + 9, text_x + 3, "`8b d8'8b d8'   .88.   88  V888", text_color, background_color);
    insert_text(screen, text_y + 10,text_x + 3, " `8b8' `8d8'  Y888888P VP   V8P", text_color, background_color);

    insert_text(screen, text_y + 13,text_x + 13, " New Game", text_color, background_color);
    insert_text(screen, text_y + 14,text_x + 13, " Menu",     text_color, background_color);
    insert_text(screen, text_y + 15,text_x + 13, " Exit",     text_color, background_color);
}

/*
 * Place cursor in win screen
 * Places the cursor in the win screen
 */
static void place_cursor_in_menu(const void *win_screen_pointer, const Coords cursor_coords, Coords *target_coords) {
    (void)win_screen_pointer;
    target_coords->y = (SCREEN_HEIGHT - WIN_SCREEN_HEIGHT) / 2 + 15 + cursor_coords.y;
    target_coords->x = (SCREEN_WIDTH  - WIN_SCREEN_WIDTH)  / 2 + 14;
}

/*
 * Move in win screen
 * Moves the cursor in the win screen
 */
static void move_in_menu(const void *win_screen_pointer, Coords *coords, const Coords delta) {
    (void)win_screen_pointer;

    if (delta.x != 0) return;
    short new_y = coords->y + delta.y;
    // if (new_y < 0) new_y = 2;
    coords->y = (new_y + 3) % 3;
}

/*
 * Get default coords
 * Returns the default coords for the win screen
 */
static Coords get_default_coords(const void *win_screen_pointer) {
    (void)win_screen_pointer;
    return (Coords) {.x = 0, .y = 0};
}

/*
 * Get cursor config
 * Gets the cursor config of the win screen
 */
static CursorConfig get_cursor_config_in_menu(const void *win_screen_pointer, const Coords cursor_coords) {
    (void)win_screen_pointer;
    (void)cursor_coords;
    return (CursorConfig) {.type = CURSOR_RIGHT_SLIM};
}

/*
 * On new game click
 * Handles the new game button click
 */
static void on_new_game_click(void *win_screen_pointer, void *context) {
    Game *game = (Game *)context;
    game_reset(game);
    CORE_CHANGE_LAYER(win_screen_pointer, GAME_ID);
}

/*
 * On menu click
 * Handles the menu button click
 */
static void on_menu_click(void *win_screen_pointer, void *context) {
    (void)context;
    CORE_CHANGE_LAYER(win_screen_pointer, MENU_ID);
}

/*
 * On exit click
 * Handles the exit button click
 */
static void on_exit_click(void *win_screen_pointer, void *context) {
    (void)context;
    CORE_SHUTDOWN(win_screen_pointer);
    exit(0);
}

/*
 * Prepare win screen
 */
static void prepare_win_screen(Screen *screen) {
    int y = (SCREEN_HEIGHT - WIN_SCREEN_HEIGHT) / 2;
    int x = (SCREEN_WIDTH - WIN_SCREEN_WIDTH) / 2;

    fill_area(screen, y, x, WIN_SCREEN_HEIGHT, WIN_SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_RESET);
    add_borders(screen, y, x, WIN_SCREEN_HEIGHT, WIN_SCREEN_WIDTH, COLOR_BLACK, COLOR_WHITE, fat_border);
}

/*
 * Win loop
 * Handles the win screen loop
 */
static void win_loop(Core *core, wint_t key) {
    switch (key) {
        case L'w': case L'ц':            core_local_move(core, CURSOR_UP);   break;
        case L's': case L'ы': case L'і': core_local_move(core, CURSOR_DOWN); break;
        case KEY_SPACE: case KEY_ENTER:  core_action(core);                  break;
    }
}

/*
 * Initialize win screen
 * Creates the win screen with required interfaces
 */
static WinScreen init_win_screen(void) {
    WinScreen win_screen = {0};

    static Drawable drawable = {
        .is_active = true,
        .print = print_win_screen
    };

    static const Interactable interactable = {
        .place_cursor        = place_cursor_in_menu,
        .move                = move_in_menu,
        .get_default_coords  = get_default_coords,
        .get_cursor_config   = get_cursor_config_in_menu
    };

    static Button new_game_button = {
        .coords = {.x = 0, .y = 0},
        .on_click = on_new_game_click
    };

    static Button menu_button = {
        .coords = {.x = 0, .y = 1},
        .on_click = on_menu_click
    };

    static Button exit_button = {
        .coords = {.x = 0, .y = 2},
        .on_click = on_exit_click
    };

    static ButtonHandler button_handler = {
        .buttons_count = 3,
        .buttons = {
            [0] = &new_game_button,
            [1] = &menu_button,
            [2] = &exit_button
        }
    };

    win_screen.interfaces = (ObjectInterfaces) {
        .name = "WinScreen",
        .capabilities = {
            .is_drawable     = true,
            .is_interactable = true,
            .have_buttons    = true,
            .requires_core   = true
        },
        .drawable       = &drawable,
        .interactable   = &interactable,
        .button_handler = &button_handler
    };

    return win_screen;
}

/*
 * Prepare win screen
 * Prepares the win screen for drawing
 */
MapLayer win_layer_init(Game *game) {
    static WinScreen win_screen = {0};
    win_screen = init_win_screen();

    SET_BUTTON_CONTEXT(&win_screen, 0, game);

    return (MapLayer) {
        .prepare_screen = prepare_win_screen,
        .default_layer_coords = WIN_DEFAULT_COORDS,
        .layer_loop = win_loop,
        .objects = {
            [0][0] = {.object = &win_screen}
        }
    };
}

