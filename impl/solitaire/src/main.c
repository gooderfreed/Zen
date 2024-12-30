#include "../inc/solitaire.h"
#include <fcntl.h>
#include <sys/select.h>

/*
 * Initialize game
 * Initializes the game with all necessary components
 */
static void init_game(Core *core) {
    static Screen screen;
    static Container cursor_container;
    static MapLayer game_layer, menu_layer, win_layer;
    static Map map;
    static Cursor cursor;

    screen = init_screen(COLOR_GREEN, COLOR_RESET, ' ');
    cursor_container = container_init();
    
    game_layer = game_layer_init(core, &cursor_container);
    Game *game  = (Game *)game_layer.layer_main_object;
    menu_layer = menu_layer_init(core, game);
    win_layer  = win_layer_init(core, game);

    // create map
    map = (Map) {
        .layers = {
            [MENU_ID] = menu_layer,
            [GAME_ID] = game_layer,
            [WIN_ID]  = win_layer,
        },
        .global_coords = (Coords) {.x = 0, .y = 0, .z = 0}
    };

    MapObject object = map_get_current_object(&map);
    cursor = init_cursor(object.object, GET_DEFAULT_COORDS(object.object), &cursor_container);

    core_set_cursor(core, &cursor);
    core_set_screen(core, &screen);
    core_set_map(core, &map);
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

        struct timespec ts = {0, 1000000000 / 120};  // 120 fps | why 120fps for console? Because
        nanosleep(&ts, NULL);
    }
}

int main(void) {
    srand((unsigned int)time(NULL));
    Core core = {0};
    
    init_game(&core);
    game_loop(&core);
}
