#include "../../inc/minesweeper.h"

/*
 * Prepare game screen
 */
static void prepare_game_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCALE_FACTOR_Y, screen->width - SCALE_FACTOR_X, L' ', COLOR_WHITE, COLOR_BLACK);
    fill_area(screen, 0, 0, screen->height - SCALE_FACTOR_Y, SCALE_FACTOR_X, L' ', COLOR_WHITE, COLOR_BLACK);
    
    fill_area(screen, screen->height - SCALE_FACTOR_Y, SCALE_FACTOR_X, SCALE_FACTOR_Y, screen->width - SCALE_FACTOR_X, L' ', COLOR_DARK_GRAY, COLOR_BLACK);
    fill_area(screen, SCALE_FACTOR_Y, screen->width - SCALE_FACTOR_X, screen->height - SCALE_FACTOR_Y, SCALE_FACTOR_X, L' ', COLOR_DARK_GRAY, COLOR_BLACK);
    
    fill_area(screen, BORDER_SIZE_Y, screen->width - BORDER_SIZE_X, FIELD_HEIGHT, SCALE_FACTOR_X, L' ', COLOR_WHITE, COLOR_BLACK);
    fill_area(screen, screen->height - BORDER_SIZE_Y, BORDER_SIZE_X, SCALE_FACTOR_Y, FIELD_WIDTH + SCALE_FACTOR_X, L' ', COLOR_WHITE, COLOR_BLACK);
    
    fill_area(screen, BORDER_SIZE_Y - SCALE_FACTOR_Y, BORDER_SIZE_X - SCALE_FACTOR_X, FIELD_HEIGHT + SCALE_FACTOR_Y, SCALE_FACTOR_X, L' ', COLOR_DARK_GRAY, COLOR_BLACK);
    fill_area(screen, BORDER_SIZE_Y - SCALE_FACTOR_Y, BORDER_SIZE_X, SCALE_FACTOR_Y, FIELD_WIDTH, L' ', COLOR_DARK_GRAY, COLOR_BLACK);

    put_pixel(screen, screen->height - SCALE_FACTOR_Y, 0, L' ', COLOR_WHITE, COLOR_BLACK, Effect_None);
    put_pixel(screen, screen->height - SCALE_FACTOR_Y, 1, L' ', COLOR_DARK_GRAY, COLOR_BLACK, Effect_None);
    put_pixel(screen, screen->height - BORDER_SIZE_Y, BORDER_SIZE_X - SCALE_FACTOR_X + 0, L' ', COLOR_DARK_GRAY, COLOR_BLACK, Effect_None);
    put_pixel(screen, screen->height - BORDER_SIZE_Y, BORDER_SIZE_X - SCALE_FACTOR_X + 1, L' ', COLOR_WHITE, COLOR_BLACK, Effect_None);

    put_pixel(screen, 0, screen->width - SCALE_FACTOR_X + 0, L' ', COLOR_WHITE, COLOR_BLACK, Effect_None);
    put_pixel(screen, 0, screen->width - SCALE_FACTOR_X + 1, L' ', COLOR_DARK_GRAY, COLOR_BLACK, Effect_None);
    put_pixel(screen, BORDER_SIZE_Y - SCALE_FACTOR_Y, screen->width - BORDER_SIZE_X + 0, L' ', COLOR_DARK_GRAY, COLOR_BLACK, Effect_None);
    put_pixel(screen, BORDER_SIZE_Y - SCALE_FACTOR_Y, screen->width - BORDER_SIZE_X + 1, L' ', COLOR_WHITE, COLOR_BLACK, Effect_None);

}

/*
 * Game loop
 * Handles game loop
 */
static void game_loop(Zen *zen, wint_t key) {
    switch (key) {
        case L'q': case L'й': zen_shutdown(zen); exit(0); break;
    }
}

/*
 * Game cursor loop
 * Handles game cursor loop
 */
static bool game_cursor_loop(Zen *zen, wint_t key) {
    switch (key) {
        case L'a': case L'ф':            zen_local_move(zen, CURSOR_LEFT);   break;
        case L'd': case L'в':            zen_local_move(zen, CURSOR_RIGHT);  break;
        case L'w': case L'ц':            zen_local_move(zen, CURSOR_UP);     break;
        case L's': case L'ы': case L'і': zen_local_move(zen, CURSOR_DOWN);   break;
        case KEY_SPACE: case KEY_ENTER:  zen_action(zen);                    break;
    }
    return false;
}

/*
 * Initialize game layer
 * Creates game layer with required interfaces
 */
MapLayer *game_layer_init(Arena *arena) {
    MinesweeperBoard *field = init_field(arena, COORDS(10, 10), 20);

    MapLayer *game_layer = NULL;
    MAP_LAYER(arena, game_layer, {
        prepare_screen = prepare_game_screen;
        loop = game_loop;
        cursor_loop = game_cursor_loop;
    }, {
        OBJECT(field, COORDS(0, 0));
    });

    return game_layer;
}
