#include "../inc/solitare.h"

/*
 * Prepare game screen
 */
static void prepare_game_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_GREEN, COLOR_RESET);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLACK, COLOR_BLUE, fat_border);
    add_separator(screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, COLOR_BLACK, COLOR_BLUE, fat_border);
}

/*
 * Initialize game layer
 * Creates game layer with required interfaces
 */
MapLayer game_layer_init(Deck *deck, Field *field, Stock *stock) {
    return (MapLayer) {
        .prepare_screen = prepare_game_screen,
        .default_layer_coords = GAME_DEFAULT_COORDS,
        .objects = {
            [0][0] = {.object = deck},
            [0][1] = {.object = field},
            [0][2] = {.object = stock}
        }
    };
}

