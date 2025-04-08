
#include "../../inc/solitaire.h"

/*
 * Prepare win screen
 */
static void prepare_win_screen(Screen *screen) {
    int y = (SCREEN_HEIGHT - WIN_SCREEN_HEIGHT) / 2;
    int x = (SCREEN_WIDTH - WIN_SCREEN_WIDTH) / 2;

    fill_area(screen, y, x, WIN_SCREEN_HEIGHT, WIN_SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_NONE);
    add_borders(screen, y, x, WIN_SCREEN_HEIGHT, WIN_SCREEN_WIDTH, COLOR_NONE, COLOR_WHITE, fat_border);
}

/*
 * Win loop
 * Handles the win screen loop
 */
static bool win_cursor_loop(Zen *zen, wint_t key) {
    switch (key) {
        case L'w': case L'ц':            zen_local_move(zen, CURSOR_UP);   break;
        case L's': case L'ы': case L'і': zen_local_move(zen, CURSOR_DOWN); break;
        case KEY_SPACE: case KEY_ENTER:  zen_action(zen);                  break;
    }
    return false;
}

/*
 * Prepare win screen
 * Prepares the win screen for drawing
 */
MapLayer *win_layer_init(Arena *arena, Game *game) {
    WinScreen *win_screen = init_win_screen(arena);

    SET_BUTTON_CONTEXT(win_screen, "on_new_game_click", game);

    MapLayer *win_layer = NULL;
    MAP_LAYER(arena, win_layer, {
        prepare_screen = prepare_win_screen;
        cursor_loop = win_cursor_loop;
        main_object = win_screen;
    }, {
        OBJECT(win_screen, COORDS(0, 0));
    });
    return win_layer;
}

