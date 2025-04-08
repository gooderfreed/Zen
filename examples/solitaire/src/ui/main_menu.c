#include "../../inc/solitaire.h"

// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Print menu
 * Draws the menu on the screen
 */
static void print_menu(const void *menu_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Menu *menu = (Menu *)menu_pointer;
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_WHITE);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_NONE, COLOR_WHITE, fat_border);

    int text_offset = 1;
    int text_y = 18;

    Color text_color = COLOR_NONE;
    Color background_color = COLOR_NONE;

    insert_text(screen, text_y + 0, text_offset, ".dP\"Y8  dP\"Yb  88     88 888888    db    88 88\"\"Yb 88888", text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 1, text_offset, "`Ybo.\" dP   Yb 88     88   88     dPYb   88 88__dP 88__ ",    text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 2, text_offset, "o.`Y8b Yb   dP 88  .o 88   88    dP__Yb  88 88\"Yb  88\"\" ",  text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 3, text_offset, "8bodP'  YbodP  88ood8 88   88   dP\"\"\"\"Yb 88 88  Yb 88888", text_color, background_color, Effect_Bold);

    if (menu->start_game) {
        insert_text(screen, text_y + 8, SCREEN_WIDTH/2 - 6, " Start", text_color, background_color, Effect_Bold);
    }
    else {
        insert_text(screen, text_y + 7, SCREEN_WIDTH/2 - 6, " Continue", text_color, background_color, Effect_Bold);
        insert_text(screen, text_y + 8, SCREEN_WIDTH/2 - 6, " New Game", text_color, background_color, Effect_Bold);
    }
    insert_text(screen, text_y + 9,  SCREEN_WIDTH/2 - 6, " Controls", text_color, background_color, Effect_Bold);
    insert_text(screen, text_y + 10, SCREEN_WIDTH/2 - 6, " Exit",     text_color, background_color, Effect_Bold);
}




// =========================
//  CURSOR_INTERACTABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Place cursor in menu
 * Places the cursor in the menu
 */
static Coords place_cursor_in_menu(const void *menu_pointer, const Coords cursor_coords) {
    (void)menu_pointer;
    return (Coords) {
        .y = 25 + cursor_coords.y,
        .x = SCREEN_WIDTH/2 - 6
    };
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
    CORE_GLOBAL_MOVE(menu_pointer, CURSOR_RIGHT);
}




/*
 * Init menu
 * Initializes the menu
 */
Menu *init_menu(Arena *arena) {
    Menu *menu = (Menu *)arena_alloc(arena, sizeof(Menu));
    *menu = (Menu) {
        .start_game = true
    };

    INTERFACES(arena, menu, {
        DRAWABLE(print_menu);
        CURSOR_INTERACTABLE(place_cursor_in_menu, move_in_menu, get_default_coords, get_cursor_config_in_menu);
        BUTTONS_HANDLER({
            BUTTONS_GROUP({
                BUTTON(on_continue_click);
                BUTTON(on_start_click);
                BUTTON(on_controls_click);
                BUTTON(on_exit_click);
            });
        });
        CORE_DEPENDENT();
    });
    
    return menu;
}
