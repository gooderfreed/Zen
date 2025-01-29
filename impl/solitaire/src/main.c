#include "../inc/solitaire.h"
#include <fcntl.h>
#include <sys/select.h>

/*
 * Initialize game
 * Initializes the game with all necessary components
 */
static void init_game(Core *core) {
    Arena *arena = core->arena;
    Screen *screen = init_screen(arena, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_GREEN, COLOR_RESET, ' ');
    Container *cursor_container = container_init(arena, CONTAINER_SIZE);
    MapLayer *game_layer = game_layer_init(arena, cursor_container);
    Game *game  = (Game *)game_layer->layer_main_object;
    MapLayer *menu_layer = menu_layer_init(arena, game);
    MapLayer *win_layer  = win_layer_init(arena, game);

    // create map
    Map *map = init_map(arena, GAME_LAYERS, (Coords) {.x = 0, .y = 0, .z = 0});
    map_set_layer(map, menu_layer, MENU_ID);
    map_set_layer(map, game_layer, GAME_ID);
    map_set_layer(map, win_layer,  WIN_ID);

    MapObject object = map_get_current_object(map);
    Cursor *cursor = init_cursor(arena, object.object, GET_DEFAULT_COORDS(object.object), cursor_container);

    core_set_cursor(core, cursor);
    core_set_screen(core, screen);
    core_set_map(core, map);
}

/*
 * Game loop
 * Main game loop
 */
static void game_loop(Core *core) {

    while (true) {
        core_update_screen(core);

        core_update(core);

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        // check for input
        if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0) {
            wint_t ch = getwchar();

            if (ch == KEY_ESC) {
                core_shutdown(core);
                break;
            }

            core_manage_loop(core, ch);
        }

        struct timespec ts = {0, 1000000000 / 120};  // 120 fps | why 120fps for console? Because I can
        nanosleep(&ts, NULL);
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    size_t size = 1024 * 1024;
    Arena *arena = arena_new_dynamic(size);

    Core *core = core_init(arena);
    
    init_game(core);
    game_loop(core);
}
