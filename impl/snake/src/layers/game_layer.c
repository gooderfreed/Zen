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

    MapLayer *game_layer = create_map_layer(arena, 2, 1, SNAKE_DEFAULT_COORDS);

    game_layer->prepare_screen = prepare_game_screen;
    game_layer->layer_loop = game_loop;
    game_layer->layer_main_object = snake;

    game_layer->objects[0][0].object = score_counter;
    game_layer->objects[1][0].object = snake;

    return game_layer;
}
