#include "../inc/solitare.h"
#include <fcntl.h>
#include <sys/select.h>

int main(void) {
    srand((unsigned int)time(NULL));
    
    Screen screen = init_screen(COLOR_GREEN, COLOR_RESET, ' ');
    Container cursor_container = container_init();
    Core core = {0};

    MapLayer game_layer = game_layer_init(&core, &cursor_container);
    MapLayer menu_layer = menu_layer_init(&core);
    MapLayer win_layer = win_layer_init(&core);

    // create map
    Map map = {
        .layers = {
            [MENU_ID] = menu_layer,
            [GAME_ID] = game_layer,
            [WIN_ID]  = win_layer,
        },
        .global_coords = (Coords) {.x = 0, .y = 0, .z = 0}
    };

    MapObject object = map_get_current_object(&map);
    Cursor    cursor = init_cursor(object.object, GET_DEFAULT_COORDS(object.object), &cursor_container);

    core_set_cursor(&core, &cursor);
    core_set_screen(&core, &screen);
    core_set_map(&core, &map);

    // main loop
    set_noncanonical_mode();
    while (true) {
        core_update_screen(&core);

        core_update(&core);

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
                core_shutdown(&core);
                break;
            }

            core_manage_loop(&core, ch);
        }

        struct timespec ts = {0, 1000000000 / 120};
        nanosleep(&ts, NULL);
    }
}
