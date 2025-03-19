#include "../../inc/snake.h"

/*
 * Prepare game screen
 */
static void prepare_game_screen(Screen *screen) {
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLUE, COLOR_WHITE, fat_border);
    add_separator(screen, SNAKE_AREA_OFFSET_Y, 0, COLOR_BLUE, COLOR_WHITE, fat_border);
}

/*
 * Game loop
 * Handles game loop
 */
static void game_loop(Core *core, wint_t key) {
    switch (key) {
        case L'q': case L'й': core_shutdown(core); exit(0); break;
    }
}

/*
 * Initialize game layer
 * Creates game layer with required interfaces
 */
MapLayer *game_layer_init(Arena *arena) {
    Snake *snake = init_snake(arena);
    ScoreCounter *score_counter = init_score_counter(arena);

    MapLayer *game_layer = NULL;
    MAP_LAYER(arena, game_layer, {
        prepare_screen = prepare_game_screen;
        loop = game_loop;
    }, {
        OBJECT(score_counter, COORDS(0, 0));
        OBJECT(snake, COORDS(0, 1), {is_main = true;});
    });

    return game_layer;
}
