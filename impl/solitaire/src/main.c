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

/*
 * Prepare menu screen
 */
static void prepare_menu_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_RESET);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLACK, COLOR_WHITE, fat_border);
}

/*
 * Prepare game screen
 */
static void prepare_game_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_GREEN, COLOR_RESET);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLACK, COLOR_BLUE, fat_border);
    add_separator(screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, COLOR_BLACK, COLOR_BLUE, fat_border);
}

/*
 * Prepare win screen
 */
static void prepare_win_screen(Screen *screen) {
    int y = (SCREEN_HEIGHT - WIN_SCREEN_HEIGHT) / 2;
    int x = (SCREEN_WIDTH - WIN_SCREEN_WIDTH) / 2;

    fill_area(screen, y, x, WIN_SCREEN_HEIGHT, WIN_SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_RESET);
    add_borders(screen, y, x, WIN_SCREEN_HEIGHT, WIN_SCREEN_WIDTH, COLOR_BLACK, COLOR_WHITE, fat_border);
}

int main(void) {
    srand((unsigned int)time(NULL));
    
    Screen screen = init_screen(COLOR_GREEN, COLOR_RESET, ' ');
    Container cursor_container = container_init();

    // init objects
    Deck  deck  = generate_deck();
    Field field = init_field(&deck);
    Stock stock = init_stock();

    Menu  menu  = init_menu();
    WinScreen win_screen = init_win_screen();

    // create map
    Map map = {
        .layers = {
            [MENU_ID] = (MapLayer) {
                .prepare_screen = prepare_menu_screen,
                .default_layer_coords = MENU_DEFAULT_COORDS,
                .objects = {
                    [0][0] = {.object = &menu}
                }
            },
            [GAME_ID] = (MapLayer) {
                .prepare_screen = prepare_game_screen,
                .default_layer_coords = GAME_DEFAULT_COORDS,
                .objects = {
                    [0][0] = {.object = &deck},
                    [0][1] = {.object = &field},
                    [0][2] = {.object = &stock}
                }
            },
            [WIN_ID] = (MapLayer) {
                .prepare_screen = prepare_win_screen,
                .default_layer_coords = WIN_DEFAULT_COORDS,
                .objects = {
                    [0][0] = {.object = &win_screen}
                }
            },
        },
        .global_coords = (Coords) {.x = 0, .y = 0, .z = 0}
    };

    // init core
    MapObject object = map_get_current_object(&map);
    Cursor    cursor = init_cursor(object.object, GET_DEFAULT_COORDS(object.object), &cursor_container);
    Core      core   = init_core(&map, &cursor, &screen);

    // set game context
    StockContext stock_context = {
        .deck = &deck,
        .field = &field,
        .cursor_container = &cursor_container,
        .core = &core
    };

    // set context for game objects
    SET_UPDATE_CONTEXT(&stock,   &stock_context);
    SET_BUTTON_CONTEXT(&deck, 0, &cursor_container);

    // set context for menu objects
    SET_BUTTON_CONTEXT(&menu, 0, &core);
    SET_BUTTON_CONTEXT(&menu, 1, &core);
    SET_BUTTON_CONTEXT(&menu, 4, &core);

    // set context for win screen objects
    SET_BUTTON_CONTEXT(&win_screen, 0, &core);
    SET_BUTTON_CONTEXT(&win_screen, 1, &core);
    SET_BUTTON_CONTEXT(&win_screen, 2, &core);


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
