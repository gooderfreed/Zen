#include "../inc/solitare.h"
#include <fcntl.h>
#include <sys/select.h>

#define FPS_UPDATE_INTERVAL (CLOCKS_PER_SEC / 100)

/*
 * Calculate FPS
 */
static char* calculate_fps(char *buffer) {
    static clock_t last_time = 0;
    static int frame_count = 0;
    static float fps = 0.0f;
    static float max_fps = 0.0f;
    static float min_fps = 1000000.0f;

    clock_t current_time = clock();
    frame_count++;

    if (current_time - last_time >= FPS_UPDATE_INTERVAL) {
        fps = (float)frame_count * (float)(CLOCKS_PER_SEC / (current_time - last_time));
        frame_count = 0;
        last_time = current_time;
    }
    
    max_fps = fps > max_fps ? fps : max_fps;
    min_fps = fps > 0.0f && fps < min_fps ? fps : min_fps;

    sprintf(buffer, "FPS: %.2f | max: %.2f | min: %.2f", (double)fps, (double)max_fps, (double)min_fps);

    return buffer;
}

int main(void) {
    srand((unsigned int)time(NULL));
    
    Screen screen = init_screen(COLOR_GREEN, COLOR_RESET, ' ');
    Container cursor_container = container_init();
    Core core = {0};

    // init objects
    Deck  deck  = generate_deck();
    Field field = init_field(&deck);
    Stock stock = init_stock();

    MapLayer game_layer = game_layer_init(&deck, &field, &stock);
    
    Menu  menu  = init_menu();
    MapLayer menu_layer = menu_layer_init(&menu, &core);

    WinScreen win_screen = init_win_screen();
    MapLayer win_layer = win_layer_init(&win_screen, &core);

    // create map
    Map map = {
        .layers = {
            [MENU_ID] = menu_layer,
            [GAME_ID] = game_layer,
            [WIN_ID]  = win_layer,
        },
        .global_coords = (Coords) {.x = 0, .y = 0, .z = 0}
    };

    // init core
    MapObject object = map_get_current_object(&map);
    Cursor    cursor = init_cursor(object.object, GET_DEFAULT_COORDS(object.object), &cursor_container);

    // set game context
    StockContext stock_context = {
        .deck  = &deck,
        .field = &field,
        .core  = &core,
        .cursor_container = &cursor_container,
    };

    // set context for game objects
    SET_UPDATE_CONTEXT(&stock,   &stock_context);
    SET_BUTTON_CONTEXT(&deck, 0, &cursor_container);


    core_set_cursor(&core, &cursor);
    core_set_screen(&core, &screen);
    core_set_map(&core, &map);

    char buffer[50];
    // main loop
    set_noncanonical_mode();
    while (true) {
        // update fps
        insert_text(&screen, 0, 0, calculate_fps(buffer), COLOR_BOLD, COLOR_BLACK);
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

            switch (ch) {
                case L'q': case L'й':            core_change_layer(&core, MENU_ID);     break;
                case L'a': case L'ф':            core_local_move(&core, CURSOR_LEFT);   break;
                case L'd': case L'в':            core_local_move(&core, CURSOR_RIGHT);  break;
                case L'w': case L'ц':            core_local_move(&core, CURSOR_UP);     break;
                case L's': case L'ы': case L'і': core_local_move(&core, CURSOR_DOWN);   break;
                case KEY_SPACE: case KEY_ENTER:  core_action(&core);                    break;
                case KEY_CTRL_A:                 core_global_move(&core, CURSOR_LEFT);  break;
                case KEY_CTRL_D:                 core_global_move(&core, CURSOR_RIGHT); break;
            }
        }
    }
}
