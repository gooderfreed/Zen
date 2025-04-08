/* 
 * Can your Rust do that? 
 * I don't think so!
 */
#include "../inc/solitaire.h"

/*
 * Initialize game
 * Initializes the game with all necessary components
 */
static void init_game(Zen *zen) {
    Arena *arena = zen->arena;
    Screen *screen = init_screen(arena, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_GREEN, COLOR_WHITE, ' ');
    Container *cursor_container = container_init(arena, CONTAINER_SIZE);
    Cursor *cursor = init_cursor(arena, cursor_container);
    
    MapLayer *game_layer = game_layer_init(arena, cursor);
    Game *game = (Game *)game_layer->layer_main_object;
    MapLayer *menu_layer = menu_layer_init(arena, game);
    MapLayer *win_layer  = win_layer_init(arena, game);

    // create map
    Map *map = init_map(arena, GAME_LAYERS, (Coords) {.x = 0, .y = 0, .z = 0});
    map_set_layer(map, menu_layer, MENU_ID);
    map_set_layer(map, game_layer, GAME_ID);
    map_set_layer(map, win_layer,  WIN_ID);

    MapObject object = map_get_current_object(map);
    cursor_set_subject(cursor, object.object);

    zen_set_cursor(zen, cursor);
    zen_set_screen(zen, screen);
    zen_set_map(zen, map);
}

/*
 * Game loop
 * Main game loop
 */
static void game_loop(Zen *zen) {
    zen_set_target_fps(zen, 60);
    zen_set_ticks_per_second(zen, 20);

    while (!zen_should_close(zen)) {
        if (zen_has_input()) {
            wint_t ch = getwchar();

            if (ch == KEY_ESC) {
                zen_shutdown(zen);
                break;
            }

            zen_manage_loop(zen, ch);
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    size_t size = 1024 * 1024;
    Arena *arena = arena_new_dynamic(size);

    Zen *zen = zen_init(arena);
    
    init_game(zen);
    game_loop(zen);
}
