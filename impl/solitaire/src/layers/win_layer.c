
#include "../inc/solitaire.h"

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

