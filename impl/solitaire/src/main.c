#include "../inc/solitare.h"

static void prepare_menu_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_RESET);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLACK, COLOR_WHITE, fat_border);
}

static void prepare_game_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_GREEN, COLOR_RESET);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLACK, COLOR_BLUE, fat_border);
    add_separator(screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, COLOR_BLACK, COLOR_BLUE, fat_border);
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

    // create map
    Map map = {
        .layers = {
            [0] = (MapLayer) {
                .prepare_screen = prepare_menu_screen,
                .default_layer_coords = MENU_DEFAULT_COORDS,
                .objects = {
                    [0][0] = {.object = &menu}
                }
            },
            [1] = (MapLayer) {
                .prepare_screen = prepare_game_screen,
                .default_layer_coords = GAME_DEFAULT_COORDS,
                .objects = {
                    [0][0] = {.object = &deck},
                    [0][1] = {.object = &field},
                    [0][2] = {.object = &stock}
                }
            },
            [2] = (MapLayer) {0},
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
        .cursor_container = &cursor_container
    };

    SET_UPDATE_CONTEXT(&stock,   &stock_context);
    SET_BUTTON_CONTEXT(&deck, 0, &cursor_container);
    SET_BUTTON_CONTEXT(&menu, 0, &core);
    SET_BUTTON_CONTEXT(&menu, 3, &core);

    // main loop
    core_update_screen(&core);
    set_noncanonical_mode();
    bool need_screen_update;
    while (true) {
        wint_t ch = getwchar();
        need_screen_update = true;
        switch (ch) {
            case L'q': case L'й': case KEY_ESC: core_change_layer(&core, 0);      break;
            case L'a': case L'ф': core_local_move(&core, CURSOR_LEFT);            break;
            case L'd': case L'в': core_local_move(&core, CURSOR_RIGHT);           break;
            case L'w': case L'ц': core_local_move(&core, CURSOR_UP);              break;
            case L's': case L'ы': case L'і': core_local_move(&core, CURSOR_DOWN); break;
            case KEY_SPACE: case KEY_ENTER: core_action(&core);                   break;
            case KEY_CTRL_A: core_global_move(&core, CURSOR_LEFT);                break;
            case KEY_CTRL_D: core_global_move(&core, CURSOR_RIGHT);               break;
            default: need_screen_update = false;
        }

        if (need_screen_update) {
            core_update_screen(&core);
        }
    }
}
